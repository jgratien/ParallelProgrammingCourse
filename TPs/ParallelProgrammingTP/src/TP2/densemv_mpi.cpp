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
  /*if(vm["eigen"].as<int>()==1)
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

  }*/

  std::vector<size_t> tab_local_sizes(nb_proc);

  if(my_rank==0) // MASTER NODE
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
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;

      int size = (int) nrows/nb_proc;
      int reste =  nrows % nb_proc;
      size_t local_size;
      double * local_ptr = matrix.data();
      // SEND MATRIX

      tab_local_sizes[0] = (size_t) size;
      for (int i=1; i<nb_proc;++i)
      {
        local_size = size;
        if (reste > 0) {
          reste -= 1;
          local_size += 1;
        }
        tab_local_sizes[i] = local_size;
      }

      MPI_Bcast(tab_local_sizes.data(), nb_proc, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;

      //std::cout << "Master has sent sizes " << std::endl;

      local_ptr += size * nrows;
      int irow = size;
      std::vector<MPI_Request> requests_data(nb_proc-1);
      for (int i=1; i<nb_proc;++i)
      {
        MPI_Isend(local_ptr, nrows * tab_local_sizes[i], MPI_DOUBLE, i, 30, MPI_COMM_WORLD, &requests_data[i-1]) ;
      }

      MPI_Status st;
      for(auto r : requests_data) {
          MPI_Wait(&r, &st);
      }


    }

    {
      // BROAD CAST VECTOR X
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
    //DenseMatrix local_matrix ;
    std::size_t local_size = tab_local_sizes[0] ;

    {
      // EXTRACT LOCAL DATA FROM MASTER PRO

      // COMPUTE LOCAL SIZE

      // EXTRACT LOCAL MATRIX DATA
      matrix.resize(local_size, nrows);
    }

    std::vector<double> local_y(nrows);
    {
      matrix.mult(x, local_y);
      // compute parallel SPMV
      // TIMER HERE
    }

    std::vector<double> y(nrows);
    {
      // RECONSTRUCT Y
      std::vector<int> displacements(nb_proc);
      std::vector<int> int_tab_local_sizes(nb_proc);
      displacements[0] = 0;
      int_tab_local_sizes[0] = (int) tab_local_sizes[0];
      for (int i = 1; i < nb_proc; ++i) {
        displacements[i] = (int) (displacements[i-1] + tab_local_sizes[i-1]);
        int_tab_local_sizes[i] = (int) tab_local_sizes[i];
      }
      MPI_Gatherv(local_y.data(), tab_local_sizes[0], MPI_DOUBLE,
                y.data(), int_tab_local_sizes.data(), displacements.data(),
                MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
  }
  else
  { // ALL NON-MASTER NODES
    // COMPUTE LOCAL MATRICE LOCAL VECTOR

    DenseMatrix local_matrix ;
    std::size_t nrows ;
    std::size_t local_size;
    //std::size_t local_nrows ;

    {
      // RECV DATA FROM MASTER PROC

      // RECV GLOBAL SIZE
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;
      //nrows = (std::size_t) proxy_nrows;
      std::cout << "Noeud " << my_rank << "  global size reçu = " << nrows << std::endl;

      // RECV LOCAL SIZE
      MPI_Status status;

      MPI_Bcast(tab_local_sizes.data(), nb_proc, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;

      local_size = tab_local_sizes[my_rank];
      std::cout << "Noeud " << my_rank << " local size reçu = " << local_size << std::endl;


      // RECV MATRIX DATA
      local_matrix.init(local_size, nrows);
      double* ptr_matrix_data = local_matrix.data() ;
      MPI_Recv(ptr_matrix_data, local_size * nrows, MPI_DOUBLE, 0, 30, MPI_COMM_WORLD, &status);
    }

    std::cout << "Noeud " << my_rank <<" j'ai recu tous les données de la matrice " << std::endl;

    std::vector<double> x;
    x.resize(nrows) ;
    {
      // BROAD CAST VECTOR X
      double* ptr_matrix_x = x.data();
      MPI_Bcast(ptr_matrix_x, nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;

    }

    std::vector<double> local_y(local_size);
    {
      // compute parallel SPMV
      local_matrix.mult(x, local_y);
      // TIMER HERE
      MPI_Gatherv(local_y.data(), tab_local_sizes[my_rank], MPI_DOUBLE,
                NULL, NULL, NULL,
                MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

  }
  timer.printInfo() ;
  MPI_Finalize();
  return 0 ;
}
