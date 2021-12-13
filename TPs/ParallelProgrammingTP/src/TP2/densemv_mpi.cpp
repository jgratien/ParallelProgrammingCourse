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
#include <typeinfo>

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

  int my_rank;
  int nb_proc;
  MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;
  std::size_t localSize;
  std::vector<double> y_local;
  std::size_t nrows;
  double mpi_time = 1;  // Inicialization to prevent error after
  double mpi_finalTime;

  std::vector<int> tab_local_sizes(nb_proc);
  std::vector<int> displacements(nb_proc);

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


    nrows = matrix.nrows();
    std::vector<double> x;
    x.resize(nrows) ;

    double * matrix_ptr = matrix.data() ;
    double *local_matrix_ptr = matrix_ptr + (nrows * (nrows / nb_proc)); // first lines of process 0

    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;

    {

      // SEND GLOBAL SIZE
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

      // SEND MATRIX
      for (int i=1; i<nb_proc;++i)
      {

        int r = nrows % nb_proc;
        int q = nrows / nb_proc;
        localSize = q;
        if (i<=r) {localSize++;};

        // SEND LOCAL SIZE to PROC I
        MPI_Ssend(&localSize, 1, MPI_UNSIGNED_LONG, i, 10, MPI_COMM_WORLD) ;
        tab_local_sizes[i] = (int) localSize;

        MPI_Ssend(local_matrix_ptr, nrows * localSize, MPI_DOUBLE, i, 50, MPI_COMM_WORLD);
        local_matrix_ptr = local_matrix_ptr + nrows*localSize;
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
      std::cout<<"Complete matrix result ||y||="<<normy<<std::endl ;
    }


    // COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
    DenseMatrix local_matrix ;

    {
      // EXTRACT LOCAL DATA FROM MASTER PROC

      // COMPUTE LOCAL SIZE
      localSize = nrows / nb_proc;
      tab_local_sizes[0] = (int) localSize;

      // EXTRACT LOCAL MATRIX DATA  // REDO
      local_matrix.init(localSize, nrows);
      local_matrix_ptr = local_matrix.data();
      for (unsigned int i=0; i<localSize*nrows; i++, matrix_ptr++) {
        local_matrix_ptr[i] = *matrix_ptr;
      }

    }

    y_local.resize(localSize);
    {
      // compute parallel SPMV
      Timer::Sentry sentry(timer,"DenseMV") ;
      local_matrix.mult(x, y_local);
    }
  }
  else
  {
    // COMPUTE LOCAL MATRICE LOCAL VECTOR

    DenseMatrix local_matrix ;

    {
      // RECV DATA FROM MASTER PROC

      // RECV GLOBAL SIZE
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

      // RECV LOCAL SIZE
      MPI_Recv(&localSize,1,MPI_UNSIGNED_LONG,0,10,MPI_COMM_WORLD, MPI_STATUS_IGNORE) ; // ,*status // Srecv

      //RECV MATRIX DATA
      local_matrix.init(localSize, nrows);
      double *local_matrix_ptr = local_matrix.data();
      MPI_Recv(local_matrix_ptr, nrows * localSize, MPI_DOUBLE, 0, 50, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

      std::vector<double> x;
    {
      // BROAD CAST VECTOR X
      x.resize(nrows) ;
      MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
    }
      y_local.resize(localSize);
    {
      Timer::Sentry sentry(timer,"DenseMV") ;
      local_matrix.mult(x, y_local);
    }

  }

  std::vector<double> y_final;


  if (my_rank == 0) {
    y_final.resize(nrows);
    displacements[0] = 0;

    for (int i = 1; i < nb_proc; ++i) {
      displacements[i] = (int) (displacements[i-1] + tab_local_sizes[i-1]);
    }
  }

  MPI_Gatherv(y_local.data(), localSize, MPI_DOUBLE, y_final.data(), tab_local_sizes.data(), displacements.data(),
              MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    mpi_time = timer.getTime(1);
  }else{
    mpi_time = timer.getTime(0);
  }

  //MPI_Barrier(MPI_COMM_WORLD);
  MPI_Allreduce(&mpi_time, &mpi_finalTime, 2, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

  if (my_rank == 0) {
    double normy_final = PPTP::norm2(y_final) ;
    std::cout<<"MPI result ||y||=" << normy_final << " , Rank:" << my_rank << std::endl;
    //timer.printInfo() ;

    std::ofstream outfile;
    outfile.open("Benchmark.log", std::ios_base::app);
    outfile << "DenseMV  np: " << nb_proc << "  nx: " << nx << "  Time DenseMV: "
            << timer.getTime(0) << " Time MPI: " << mpi_finalTime
            << " Acceleration: " << timer.getTime(0)/mpi_finalTime << std::endl;
  }

  MPI_Finalize();
  return 0 ;
}
