/*
 * helloworld.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: gratienj
 */
#include <mpi.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>

#include <string>
#include <vector>
#include <fstream>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/LU>

#include "MatrixVector/DenseMatrix.h"
#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"

#include "Utils/Timer.h"

int main(int argc, char **argv)
{
  using namespace boost::program_options;
  options_description desc;
  desc.add_options()("help", "produce help")("nrows", value<int>()->default_value(0), "matrix size")("nx", value<int>()->default_value(0), "nx grid size")("file", value<std::string>(), "file input")("eigen", value<int>()->default_value(0), "use eigen package");
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << "\n";
    return 1;
  }
  MPI_Init(&argc, &argv);

  int my_rank = 0;
  int nb_proc = 1;
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  using namespace PPTP;

  Timer timer;
  MatrixGenerator generator;
  int nx = vm["nx"].as<int>();
  if (vm["eigen"].as<int>() == 1)
  {
    typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType;
    typedef Eigen::Matrix<double, Eigen::Dynamic, 1> EigenVectorType;

    std::size_t nrows = nx * nx;
    EigenMatrixType matrix(nrows, nrows);

    generator.genLaplacian(nx, matrix);

    EigenVectorType x(nrows);

    for (std::size_t i = 0; i < nrows; ++i)
      x(i) = i + 1;

    EigenVectorType y;
    {
      Timer::Sentry sentry(timer, "EigenDenseMV");
      y = matrix * x;
    }

    double normy = PPTP::norm2(y);
    std::cout << "||y||=" << normy << std::endl;
  }

  if (my_rank == 0)
  {
    DenseMatrix matrix;

    if (vm.count("file"))
    {
      std::string file = vm["file"].as<std::string>();
      generator.readFromFile(file, matrix);
    }
    else
    {
      int nx = vm["nx"].as<int>();
      generator.genLaplacian(nx, matrix);
    }

    std::size_t nrows = matrix.nrows();
    std::vector<double> x;
    x.resize(nrows);

    for (std::size_t i = 0; i < nrows; ++i)
      x[i] = i + 1;

    std::vector<double> const &values = matrix.get_values();
    {

      // SEND GLOBAL SIZE
      int nrows_to_send = nrows;
      MPI_Bcast(&nrows_to_send, 1, MPI_INT, 0, MPI_COMM_WORLD);

      // SEND MATRIX
      int begin = 0;
      {
        int nrows_local = nrows / nb_proc;
        int reste = nrows % nb_proc;
        if (0 < reste)
        {
          nrows_local++;
        }
        begin += nrows_local;
      }
    
      Timer::Sentry sentry(timer, "DenseMV Parallel : ");
      
      for (int i = 1; i < nb_proc; ++i)
      {

        // SEND LOCAL SIZE to PROC I
        int nrows_local = nrows / nb_proc;
        int r = nrows % nb_proc;
        if (i < r)
        {
          nrows_local++;
        }
        MPI_Send(&nrows_local, 1, MPI_INT, i, 1000, MPI_COMM_WORLD);
        // SEND MATRIX DATA
        const double *ptr = values.data() + begin * nrows;
        MPI_Send(ptr, nrows_local * nrows, MPI_DOUBLE, i, 2000, MPI_COMM_WORLD);
        begin += nrows_local;
      }
    }

    {
      // BROAD CAST VECTOR X
      MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
    DenseMatrix local_matrix;
    int local_nrows = nrows / nb_proc;

    std::vector<double> local_y(local_nrows);
    {
      // compute parallel SPMV for PROC 0
      for (int i = 0; i < local_nrows; i++)
      {
        for (int j = 0; j < nrows; j++)
          local_y[i] += values[nrows * i + j] * x[j];
      }
    }

    // Gather all the local y into the final y
    MPI_Status status;
    std::vector<double> y_global;
    y_global.reserve(nrows);
    {
      for (int i; i < nb_proc; i++)
      {
        int local_nrows_i = nrows / nb_proc;
        int r = nrows % nb_proc;
        if (i < r)
        {
          local_nrows_i++;
        }
        
        if (i == 0)
        {
          for (int j = 0; j < local_nrows_i; j++)
            y_global.push_back(local_y[j]);
        }
        else
        {
          std::vector<double> local_y_to_receive(local_nrows_i);
          MPI_Recv(local_y_to_receive.data(), local_nrows_i, MPI_DOUBLE, i, 4000, MPI_COMM_WORLD, &status);

          for (int j = 0; j < local_nrows_i; j++)
            y_global.push_back(local_y_to_receive[j]);
        }
      }
      // Calculate GLOBAL Norm
      double normy = PPTP::norm2(y_global);
      std::cout << " Parallel norm ||y||=" << normy << std::endl;
      {
	Timer::Sentry sentry(timer, "DenseMV Seq : ");
        std::vector<double> y_seq(nrows);
        {
          matrix.mult(x, y_seq);
        }
        double normy_seq = PPTP::norm2(y_seq);
        std::cout << "Base norm ||y||=" << normy_seq << std::endl;
      }
      timer.printInfo();
    }
  }
  else
  {
    // COMPUTE LOCAL MATRICE LOCAL VECTOR
    int nrows;
    int local_nrows;
    std::vector<double> local_matrix;

    {
      // RECV DATA FROM MASTER PROC
      // RECV GLOBAL SIZE
      MPI_Bcast(&nrows, 1, MPI_INT, 0, MPI_COMM_WORLD);

      // RECV LOCAL SIZE
      MPI_Status status;
      MPI_Recv(&local_nrows, 1, MPI_INT, 0, 1000, MPI_COMM_WORLD, &status);

      // RECV MATRIX DATA
      local_matrix.reserve(local_nrows * nrows);
      MPI_Recv(local_matrix.data(), local_nrows * nrows, MPI_DOUBLE, 0, 2000, MPI_COMM_WORLD, &status);
    }

    std::vector<double> x(nrows);
    {
      // BROAD CAST VECTOR X
      MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    std::vector<double> local_y(local_nrows);
    {
      // compute parallel SPMV
      for (int i = 0; i < local_nrows; i++)
      {
        for (int j = 0; j < nrows; j++)
          local_y[i] += local_matrix[nrows * i + j] * x[j];
      }
    }
    // SEND EACH LOCAL_Y TO PROC 0
    MPI_Send(local_y.data(), local_nrows, MPI_DOUBLE, 0, 4000, MPI_COMM_WORLD);
  }
  MPI_Finalize();

  return 0;
}


