/*#include <mpi.h>
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

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    using namespace boost::program_options;
    options_description desc;
    desc.add_options()
        ("help", "produce help")
        ("nrows", value<int>()->default_value(0), "matrix size")
        ("nx", value<int>()->default_value(0), "nx grid size")
        ("file", value<std::string>(), "file input")
        ("eigen", value<int>()->default_value(0), "use eigen package");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        MPI_Finalize();
        return 1;
    }

    int my_rank = 0;
    int nb_proc = 1;
    MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    using namespace PPTP;
    Timer timer;
    MatrixGenerator generator;
    int nx = vm["nx"].as<int>();

    // Variables for matrix and vectors
    DenseMatrix matrix;
    std::vector<double> x, y;
    std::size_t nrows = 0, local_nrows = 0;

    if (my_rank == 0) {
        // Generate or read the matrix on the master process
        if (vm.count("file")) {
            std::string file = vm["file"].as<std::string>();
            generator.readFromFile(file, matrix);
        } else {
            generator.genLaplacian(nx, matrix);
        }

        nrows = matrix.nrows();
        x.resize(nrows);
        for (std::size_t i = 0; i < nrows; ++i)
            x[i] = i + 1;

        // Send global size to all processes
        MPI_Bcast(&nrows, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

        // Send matrix row chunks to each process
        for (int i = 1; i < nb_proc; ++i) {
            std::size_t start_row = (i * nrows) / nb_proc;
            std::size_t end_row = ((i + 1) * nrows) / nb_proc;
            local_nrows = end_row - start_row;

            // Send local size
            MPI_Send(&local_nrows, 1, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);

            // Send local matrix data
            matrix.sendToProcess(start_row, end_row, i);
        }
    } else {
        // Receive global size
        MPI_Bcast(&nrows, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

        // Receive local matrix data
        MPI_Recv(&local_nrows, 1, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        matrix.recvFromProcess(local_nrows, 0);
    }

    // Broadcast the input vector to all processes
    x.resize(nrows);
    MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Allocate space for local result vector
    std::vector<double> local_y(local_nrows, 0.0);

    // Perform local matrix-vector multiplication
    {
        Timer::Sentry sentry(timer, "MPI_DenseMV");
        matrix.mult(x, local_y);
    }

    // Gather results to the master process
    if (my_rank == 0) {
        y.resize(nrows);
    }
    MPI_Gather(local_y.data(), local_nrows, MPI_DOUBLE, y.data(), local_nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // On the master process, compute and print the norm of the result
    if (my_rank == 0) {
        double normy = PPTP::norm2(y);
        std::cout << "||y||=" << normy << std::endl;
    }

    timer.printInfo(my_rank);
    MPI_Finalize();
    return 0;
}*/


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
    DenseMatrix matrix ;

    if(vm.count("file"))
    {
      std::string file = vm["file"].as<std::string>() ;
      generator.readFromFile(file,matrix) ;
    }
    else
    {
      int nx = vm["nx"].as<int>() ;
      generator.genLaplacian(nx,matrix) ;
    }


    std::size_t nrows = matrix.nrows();
    std::vector<double> x;
    x.resize(nrows) ;

    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;

    {

      // SEND GLOBAL SIZE
      MPI_Bcast(&nrows,1,MPI_UNSIGNED,0,MPI_COMM_WORLD) ;


      // SEND MATRIX
      for (int i=1; i<nb_proc;++i)
      {
        std::cout<<" SEND MATRIX DATA to proc "<<i<<std::endl ;

        // SEND LOCAL SIZE to PROC I
        int local_size = i*nrows/nb_proc ;
        MPI_Send(&local_size,1,MPI_INT,i,0,MPI_COMM_WORLD) ;

        // SEND MATRIX DATA
        MPI_Send(matrix.data()+local_size,nrows/nb_proc,MPI_DOUBLE,i,0,MPI_COMM_WORLD) ;

      }
    }

    {
      // BROAD CAST VECTOR X
      MPI_Bcast(x.data(),nrows,MPI_DOUBLE,0,MPI_COMM_WORLD) ;
    }

    {
      std::vector<double> y(nrows);
      {
        Timer::Sentry sentry(timer,"DenseMV") ;
        matrix.mult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"||y||="<<normy<<std::endl ;
    }


    // COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
    DenseMatrix local_matrix ;
    std::size_t local_nrows ;

    {
      // EXTRACT LOCAL DATA FROM MASTER PROC
      MPI_Bcast(&local_nrows,1,MPI_UNSIGNED,0,MPI_COMM_WORLD) ;

      // COMPUTE LOCAL SIZE
      std::size_t local_size = nrows/nb_proc ;

      // EXTRACT LOCAL MATRIX DATA
      local_matrix.init(local_size) ;
      local_matrix.copy(matrix) ;
    }

    std::vector<double> local_y(local_nrows);
    {
      // compute parallel SPMV
      local_matrix.mult(x,local_y) ;
    }
  }
  else
  {
    // COMPUTE LOCAL MATRICE LOCAL VECTOR

    DenseMatrix local_matrix ;
    std::size_t nrows ;
    std::size_t local_nrows ;

    {
      // RECV DATA FROM MASTER PROC
      // RECV GLOBAL SIZE
      MPI_Bcast(&nrows,1,MPI_UNSIGNED,0,MPI_COMM_WORLD) ;

      // RECV LOCAL SIZE
      MPI_Recv(&local_nrows,1,MPI_UNSIGNED,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE) ;

      // RECV MATRIX DATA
      local_matrix.init(local_nrows) ;
      MPI_Recv(local_matrix.data(),local_nrows,MPI_DOUBLE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE) ;
    }

    std::vector<double> x;
    {
      // BROAD CAST VECTOR X
      /* ... */
      MPI_Bcast(x.data(),nrows,MPI_DOUBLE,0,MPI_COMM_WORLD) ;
    }

    std::vector<double> local_y(local_nrows);
    {
      // compute parallel SPMV
      local_matrix.mult(x,local_y) ;
    }

  }
  timer.printInfo() ;
  MPI_Finalize() ;
  return 0 ;
}
