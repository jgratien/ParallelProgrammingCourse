/*
 * helloworld.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: gratienj
 */

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include "mpi.h"

#include <string>
#include <vector>
#include <fstream>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/LU>

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
      ("nb-threads",value<int>()->default_value(0), "nb threads")
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
  if(vm["eigen"].as<int>()==1)
  {
    typedef Eigen::SparseMatrix<double> MatrixType ;
    typedef Eigen::VectorXd             VectorType ;
    MatrixType matrix ;
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


    std::size_t nrows = matrix.rows();
    VectorType x(nrows);

    for(std::size_t i=0;i<nrows;++i)
      x(i) = i+1 ;

    VectorType y ;
    {
      Timer::Sentry sentry(timer,"EigenSpMV") ;
       y = matrix*x ;
    }

    double normy = PPTP::norm2(y) ;
    std::cout<<"||y||="<<normy<<std::endl ;
  }
 
  if(my_rank==0)
  {
    CSRMatrix matrix ;
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
    std::vector<double> x,y,y2 ;
    x.resize(nrows) ;
    y.resize(nrows) ;
    y2.resize(nrows) ;

    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;

    {
      {
        Timer::Sentry sentry(timer,"SpMV") ;
        matrix.mult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"||y||="<<normy<<std::endl ;
    }
    
    {
      {
        Timer::Sentry sentry(timer,"MPISpMV") ;

        size_t local_size = nrows/nb_proc ;
        int rest = nrows%nb_proc ;

        {
          size_t offset = 0 ;
          {
            size_t local_nrows = local_size ;
            if(0 < rest) local_nrows ++ ;
            offset += local_nrows ;
          }

          MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;

          // SEND MATRIX
          for (int i=1; i<nb_proc;++i)
          {
            // std::cout<<" SEND MATRIX DATA to proc "<<i<<std::endl ;

            // SEND LOCAL SIZE to PROC I
            size_t local_nrows = local_size ;
            if(i < rest) local_nrows ++ ;

            size_t begin = offset ;        
            offset += local_nrows ;

            std::vector<double> local_values(matrix.values() + *(matrix.kcol() + begin), matrix.values() + *(matrix.kcol() + offset)) ;
            std::vector<int> local_cols(matrix.cols() + *(matrix.kcol() + begin), matrix.cols() + *(matrix.kcol() + offset)) ;
            std::vector<int> local_kcol(matrix.kcol() + begin, matrix.kcol() + offset + 1) ;

            // SEND LOCAL SIZE to PROC I
            size_t local_values_size = local_values.size() ;
            size_t local_cols_size = local_cols.size() ;
            size_t local_kcol_size = local_kcol.size() ;
            MPI_Send(&local_values_size, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD) ;
            MPI_Send(&local_cols_size, 1, MPI_UNSIGNED_LONG, i, 1, MPI_COMM_WORLD) ;
            MPI_Send(&local_kcol_size, 1, MPI_UNSIGNED_LONG, i, 2, MPI_COMM_WORLD) ;
            

            // SEND MATRIX DATA

            MPI_Send(local_values.data(), local_values_size, MPI_DOUBLE, i, 3, MPI_COMM_WORLD) ;
            MPI_Send(local_cols.data(), local_cols_size, MPI_INT, i, 4, MPI_COMM_WORLD) ;
            MPI_Send(local_kcol.data(), local_kcol_size, MPI_INT, i, 5, MPI_COMM_WORLD) ;
          }
        }

        {
          // BROAD CAST VECTOR X
          /* ... */
          MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ; 
        }

        {
          // COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
          std::size_t local_nrows ;
          {
            // EXTRACT LOCAL DATA FROM MASTER PROC

            // COMPUTE LOCAL SIZE
            local_nrows = local_size ;
            if(0 < rest) local_nrows ++ ;
          

            std::vector<double> local_y(local_nrows);
            {
              // compute parallel SPMV
              for(std::size_t irow =0; irow<local_nrows;++irow)
              {
                double value = 0 ;
                for( int k = matrix.kcol()[irow]; k < matrix.kcol()[irow+1];++k)
                {
                  value += matrix.values()[k]*x[matrix.cols()[k]] ;
                }
                local_y[irow] = value ;
              }
            }
            y2 = local_y ;

            {
              MPI_Status status ;
              for(int i=1; i<nb_proc; ++i)
              {
                size_t local_nrows = local_size ;
                if(i < rest) local_nrows ++ ;
                std::vector<double> local_y(local_nrows) ;

                MPI_Recv(local_y.data(), local_nrows, MPI_DOUBLE, i, 6, MPI_COMM_WORLD, &status) ;
                y2.insert(y2.end(), local_y.begin(), local_y.end()) ;
              }
            }
          }
        }
      }
      double normy = PPTP::norm2(y2) ;
      std::cout<<"||y2||="<<normy<<std::endl ;
    }
    timer.printInfo() ;
  }
  else
  {

    MPI_Status status ;
    size_t local_values_size ;
    size_t local_cols_size ;
    size_t local_kcol_size ; 
    std::vector<double> local_values ;
    std::vector<int> local_cols ;
    std::vector<int> local_kcol ;
    size_t nrows ;

    {
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;
      // RECV LOCAL SIZE

      MPI_Recv(&local_values_size, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, &status) ;
      MPI_Recv(&local_cols_size, 1, MPI_UNSIGNED_LONG, 0, 1, MPI_COMM_WORLD, &status) ;
      MPI_Recv(&local_kcol_size, 1, MPI_UNSIGNED_LONG, 0, 2, MPI_COMM_WORLD, &status) ;
      // std::cout << "local size value receive by " << my_rank << " is " << local_values_size << std::endl ;
      // std::cout << "local cols value receive by " << my_rank << " is " << local_cols_size << std::endl ;
      // std::cout << "local kcol value receive by " << my_rank << " is " << local_kcol_size << std::endl ;

      // RECV MATRIX DATA
      local_values.resize(local_values_size) ;
      MPI_Recv(local_values.data(), local_values_size, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD, &status) ;

      local_cols.resize(local_cols_size) ;
      MPI_Recv(local_cols.data(), local_cols_size, MPI_INT, 0, 4, MPI_COMM_WORLD, &status) ;
      
      local_kcol.resize(local_kcol_size) ;
      MPI_Recv(local_kcol.data(), local_kcol_size, MPI_INT, 0, 5, MPI_COMM_WORLD, &status) ;
    }

    std::vector<double> x;
    {
      // BROAD CAST VECTOR X
      /* ... */
      x.resize(nrows) ;
      MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
    }

    std::vector<double> local_y(local_kcol_size-1) ;
    {
      // compute parallel SPMV

    for(std::size_t irow =0; irow<local_kcol_size-1;++irow)
      {
        double value = 0 ;
        for( int k = local_kcol[irow]; k < local_kcol[irow+1];++k)
        {
          value += local_values[k-local_kcol[0]]*x[local_cols[k-local_kcol[0]]] ;
        }
        local_y[irow] = value ;
      }
    }
    MPI_Send(local_y.data(), local_kcol_size-1, MPI_DOUBLE, 0, 6, MPI_COMM_WORLD) ;

  }
  
  MPI_Finalize() ;
  return 0 ;
}