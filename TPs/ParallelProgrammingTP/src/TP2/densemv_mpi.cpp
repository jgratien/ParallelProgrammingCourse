/*
 * helloworld.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: gratienj
 */
#include <mpi.h>
#include "tbb/tbb.h"
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

using namespace tbb;
#include "MatrixVector/DenseMatrix.h"
#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"

#include "Utils/Timer.h"

int main(int argc, char** argv)
{
  using namespace boost::program_options ;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
      ("nrows",value<int>()->default_value(0), "matrix size")
      ("nx",value<int>()->default_value(0), "nx grid size")
      ("file",value<std::string>(), "file input")
      ("eigen",value<int>()->default_value(0), "use eigen package") ;
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help"))
  {
      std::cout << desc << "\n";
      return 1;
  }
  MPI_Init(&argc,&argv) ;

  int my_rank = 0 ;
  int nb_proc = 1 ;
  MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;

  using namespace PPTP ;

  Timer timer ;
  MatrixGenerator generator ;
  int nx = vm["nx"].as<int>() ;
  if(vm["eigen"].as<int>()==1)
  {
    typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType ;
    typedef Eigen::Matrix<double, Eigen::Dynamic, 1>              EigenVectorType;


    std::size_t nrows = nx*nx ;
    EigenMatrixType matrix(nrows,nrows) ;

    generator.genLaplacian(nx,matrix) ;

    EigenVectorType x(nrows) ;

    for(std::size_t i=0;i<nrows;++i)
      x(i) = i+1 ;

    EigenVectorType y ;
    {
      Timer::Sentry sentry(timer,"EigenDenseMV") ;
      y = matrix*x ;
    }

    double normy = PPTP::norm2(y) ;
    std::cout<<"||y||="<<normy<<std::endl ;

  }


  if(my_rank==0)
  {
    DenseMatrix matrix;

    if(vm.count("file"))
    {
      std::string file = vm["file"].as<std::string>() ;
      generator.readFromFile(file, matrix) ;
    }
    else
    {
      int nx = vm["nx"].as<int>() ;
      generator.genLaplacian<DenseMatrix>(nx, matrix) ;
    }

    // Generate vector X for M*X product
    int nrows = matrix.nrows();
    std::vector<double> x(nrows);
    std::vector<double> y(nrows);
    for(std::size_t i = 0; i < nrows; i++) x[i] = i+1 ;

    // Timer scope
    {
      // Start timer
      Timer::Sentry sentry(timer, "DenseMV_MPI");

      // SEND VECTOR X
      {
        // SEND GLOBAL SIZE
        MPI_Bcast(&nrows, 1, MPI_INT, 0, MPI_COMM_WORLD);
        // BROAD CAST VECTOR X
        MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      }

      // SEND MATRIX M
      int proc_nrows = nrows / nb_proc;
      int r = nrows % nb_proc;
      {
        for (int i = 1; i < nb_proc; i++)
        {
          // SEND LOCAL SIZE TO PROC I
          proc_nrows = nrows / nb_proc;
          if (i < r) proc_nrows++;
          MPI_Send(&proc_nrows, 1, MPI_INT, i, 1000, MPI_COMM_WORLD);

          // SEND MATRIX DATA
          MPI_Send(matrix.getValues()->data() + i * proc_nrows * nrows,
                   proc_nrows * nrows, MPI_DOUBLE, i, 2000, MPI_COMM_WORLD);
        }
      }

      // COMPUTE local matrix by vector product on Proc 0
      // and put it directly in the global Y vector
      {
        DenseMatrix* local_matrix = new DenseMatrix();
        // COMPUTE LOCAL SIZE
        if (r > 0 && nb_proc > 1) proc_nrows++;
        local_matrix->setNrows(proc_nrows);
        local_matrix->getValues()->reserve(proc_nrows * nrows);

        // EXTRACT LOCAL MATRIX DATA
        std::vector<double>* locmat_data =
          new std::vector<double>(
            matrix.getValues()->begin(),
            matrix.getValues()->begin() + proc_nrows * nrows);
        local_matrix->setValues(*locmat_data);

        // compute parallel SPMV
        local_matrix->mult(x, y);
      }

      // Receive process-computed vectors
      {
        MPI_Status status;
        for (int i = 1; i < nb_proc; i++)
        {
          proc_nrows = nrows / nb_proc;
          if (i < r) proc_nrows++;
          MPI_Recv(y.data() + i * proc_nrows, proc_nrows,
                   MPI_DOUBLE, i, 3000, MPI_COMM_WORLD, &status);
        }
      }
    // end of timer scope
    }
    timer.printInfo() ;
    double normy = PPTP::norm2(y);
    std::cout << "||y|| = " << normy << std::endl;
  }
  else
  {
    // Receive Vector X
    std::vector<double>* x;
    int x_nrows;
    {
      // Recieve size of X
      MPI_Bcast(&x_nrows, 1, MPI_INT, 0, MPI_COMM_WORLD);

      // Recieve contents of Vector X
      x = new std::vector<double>(x_nrows);
      MPI_Bcast(x->data(), x_nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // Recieve MATRIX M
    int local_nrows ;
    DenseMatrix* local_matrix = new DenseMatrix();
    {
      // Receive size of local matrix M
      MPI_Status status;
      MPI_Recv(&local_nrows, 1, MPI_INT, 0, 1000,
               MPI_COMM_WORLD, &status);
      local_matrix->setNrows(local_nrows);
      local_matrix->getValues()->reserve(local_nrows * x_nrows);

      // Receive contents of matrix M
      MPI_Recv(local_matrix->getValues()->data(),
               local_nrows * x_nrows, MPI_DOUBLE,
               0, 2000, MPI_COMM_WORLD, &status);
    }

    std::vector<double>* local_y =
      new std::vector<double>(local_nrows);
    {
      // compute parallel SPMV
      local_matrix->mult(*x, *local_y);
    }

    // Send local_y to Proc 0
    MPI_Send(local_y->data(), local_nrows, MPI_DOUBLE,
             0, 3000, MPI_COMM_WORLD);
  }
  MPI_Finalize();
  return 0 ;
}
