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

          
    size_t local_size = nrows/nb_proc ;
    int rest = nrows%nb_proc ;

    {

      // SEND GLOBAL SIZE
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;
      size_t offset = 0 ;
      {
        size_t local_nrows = local_size ;
        if(0 < rest) local_nrows ++ ;
        offset += local_size*nrows ;
      }


      // SEND MATRIX
      for (int i=1; i<nb_proc;++i)
      {
        std::cout<<" SEND MATRIX DATA to proc "<<i<<std::endl ;

        // SEND LOCAL SIZE to PROC I
        size_t local_nrows = local_size ;
        if(i < rest) local_nrows ++ ;
        MPI_Send(&local_nrows, 1, MPI_UNSIGNED_LONG, i, 100, MPI_COMM_WORLD) ;
        std::cout << "send local size = " << local_nrows << " to " << i << std::endl ;

        // SEND MATRIX DATA
        MPI_Send(matrix.data()+offset, local_nrows*nrows, MPI_DOUBLE, i, 101, MPI_COMM_WORLD) ;
        offset += local_nrows*nrows ;
      }
    }

    {
      // BROAD CAST VECTOR X
      /* ... */
      MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ; 
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
    std::vector<double> fuse_y(nrows) ;

    {
      // EXTRACT LOCAL DATA FROM MASTER PROC

      // COMPUTE LOCAL SIZE
      local_nrows = local_size ;
      if(0 < rest) local_nrows ++ ;

      // EXTRACT LOCAL MATRIX DATA
    }

    std::vector<double> local_y(local_nrows);
    {
      // compute parallel SPMV
      double const* matrix_ptr = matrix.data() ;
      for(std::size_t irow =0; irow<local_nrows;++irow)
      {
        double value = 0 ;
        for(std::size_t jcol =0; jcol<nrows;++jcol)
        {
        value += matrix_ptr[jcol]*x[jcol] ;
        }
        local_y[irow] = value ;
        matrix_ptr += nrows ;
      }
    }
    fuse_y = local_y ;

    {
        MPI_Status status ;
        for(int i=1; i<nb_proc; ++i)
        {
            size_t local_nrows = local_size ;
            if(i < rest) local_nrows ++ ;
            std::vector<double> local_y(local_nrows);

            MPI_Recv(local_y.data(), local_nrows, MPI_DOUBLE, i, 102 ,MPI_COMM_WORLD, &status) ;
            fuse_y.insert(fuse_y.end(), local_y.begin(), local_y.end());
        }
    }

    double normy = PPTP::norm2(fuse_y) ;
    std::cout<<"||y||="<<normy<<std::endl ;
  }
  else
  {
    // COMPUTE LOCAL MATRICE LOCAL VECTOR

    DenseMatrix local_matrix ;
    std::size_t nrows ;
    std::size_t local_nrows ;
    MPI_Status status;

    {
      // RECV DATA FROM MASTER PROC

      // RECV GLOBAL SIZE
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;
      std::cout << "global size value receive by " << my_rank << " is " << nrows << std::endl ;

      // RECV LOCAL SIZE
      MPI_Recv(&local_nrows, 1, MPI_UNSIGNED_LONG, 0, 100, MPI_COMM_WORLD, &status) ;
      std::cout << "local size value receive by " << my_rank << " is " << local_nrows << std::endl ;

      // RECV MATRIX DATA
      local_matrix.init(local_nrows*nrows) ;
      MPI_Recv(local_matrix.data(), local_nrows*nrows, MPI_DOUBLE, 0, 101, MPI_COMM_WORLD, &status) ;
    }

    std::vector<double> x;
    {
      // BROAD CAST VECTOR X
      /* ... */
      x.resize(nrows) ;
      MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ; 
    }

    std::vector<double> local_y(local_nrows) ;
    {
      // compute parallel SPMV
      double const* matrix_ptr = local_matrix.data() ;
      for(std::size_t irow =0; irow<local_nrows;++irow)
      {
        double value = 0 ;
        for(std::size_t jcol =0; jcol<nrows;++jcol)
        {
        value += matrix_ptr[jcol]*x[jcol] ;
        }
        local_y[irow] = value ;
        matrix_ptr += nrows ;
      }
    }

    MPI_Send(local_y.data(), local_nrows, MPI_DOUBLE, 0, 102, MPI_COMM_WORLD) ;

  }
  timer.printInfo() ;
  MPI_Finalize() ;
  return 0 ;
}