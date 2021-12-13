
#include <mpi.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include "omp.h"
#include "tbb/tbb.h"

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
  int nx;
  MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;
  std::size_t localSize;
  std::size_t n_nnz;
  std::size_t first_nnz;
  std::vector<double> y_local;
  std::size_t nrows;
  double mpi_time = 1;  // Inicialization to prevent error after
  double mpi_finalTime;

  std::vector<int> tab_local_sizes(nb_proc);
  std::vector<int> displacements(nb_proc);

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


  if (my_rank == 0)
  {
    CSRMatrix matrix ;
    if(vm.count("file"))
    {
      std::string file = vm["file"].as<std::string>() ;
      generator.readFromFile(file,matrix) ;
    }
    else
    {
      nx = vm["nx"].as<int>() ;
      generator.genLaplacian(nx,matrix) ;
    }
    // CSRMatrix matrix = CSRMatrix(2, 3);
    std::size_t nrows = matrix.nrows();

    int *local_kcol_ptr = matrix.kcol();
    first_nnz = local_kcol_ptr[(nrows / nb_proc)] - local_kcol_ptr[0];
    local_kcol_ptr = local_kcol_ptr + (nrows / nb_proc); // first lines of process 0
    double *local_values_ptr = matrix.values() + first_nnz;
    int *local_cols_ptr = matrix.cols() + first_nnz;

    std::vector<double> x,y ;
    x.resize(nrows) ;
    y.resize(nrows) ;

    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;

    {
      Timer::Sentry sentry(timer,"SpMV") ;
      matrix.mult(x,y) ;

      //double normy = PPTP::norm2(y) ;
      //std::cout<<"Standard value: ||y||="<<normy<<std::endl ;
    }

    {

      // SEND GLOBAL SIZE
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

      // SEND MATRIX
      for (int i=1; i<nb_proc;++i)
      {

        int r = nrows % nb_proc;
        int q = nrows / nb_proc;
        localSize = q + 1;
        if (i<=r) {localSize++;};

        int idx_start = local_kcol_ptr[0];
        int idx_end = local_kcol_ptr[localSize - 1] - 1;
        n_nnz = idx_end - idx_start + 1;

        // SEND LOCAL SIZE to PROC I
        MPI_Ssend(&localSize, 1, MPI_UNSIGNED_LONG, i, 10, MPI_COMM_WORLD) ;
        MPI_Ssend(&n_nnz, 1, MPI_UNSIGNED_LONG, i, 20, MPI_COMM_WORLD) ;
        tab_local_sizes[i] = (int) localSize - 1;

        MPI_Ssend(local_kcol_ptr, localSize, MPI_INT, i, 50, MPI_COMM_WORLD);
        MPI_Ssend(local_cols_ptr, n_nnz, MPI_INT, i, 40, MPI_COMM_WORLD);
        MPI_Ssend(local_values_ptr, n_nnz, MPI_DOUBLE, i, 30, MPI_COMM_WORLD);

        local_kcol_ptr = local_kcol_ptr + localSize - 1;
        local_cols_ptr = local_cols_ptr + n_nnz;
        local_values_ptr = local_values_ptr + n_nnz;

      }
    }

    CSRMatrix local_matrix ;

    {
      // BROAD CAST VECTOR X
    MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
    }

    {
      // EXTRACT LOCAL DATA FROM MASTER PROC

      // COMPUTE LOCAL SIZE
      localSize = (nrows / nb_proc) + 1;
      tab_local_sizes[0] = (int) localSize - 1;

      // EXTRACT LOCAL MATRIX DATA  // REDO
      local_matrix.init(localSize, first_nnz);
      double *local_values_ptr = local_matrix.values();
      int *local_cols_ptr = local_matrix.cols();
      int *local_kcol_ptr = local_matrix.kcol();

      for (unsigned int i=0; i<first_nnz; i++) {
        local_values_ptr[i] = matrix.values()[i];
        local_cols_ptr[i] = matrix.cols()[i];
      }

      for (unsigned int i=0; i<localSize; i++) {
        local_kcol_ptr[i] = matrix.kcol()[i];
      }
    }

    y_local.resize(localSize - 1);
    {
      // compute parallel SPMV
      Timer::Sentry sentry(timer,"SpMV_mpi") ;
      local_matrix.mult(x, y_local);
    }


  }
else
{
  CSRMatrix local_matrix ;
  // RECV DATA FROM MASTER PROC

  {
  // RECV GLOBAL SIZE
  MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

  // RECV LOCAL SIZE
  MPI_Recv(&localSize,1,MPI_UNSIGNED_LONG,0,10,MPI_COMM_WORLD, MPI_STATUS_IGNORE) ; // ,*status // Srecv
  MPI_Recv(&n_nnz,1,MPI_UNSIGNED_LONG,0,20,MPI_COMM_WORLD, MPI_STATUS_IGNORE) ; // ,*status // Srecv

  //RECV MATRIX DATA
  local_matrix.init(localSize, n_nnz);
  int *local_kcol_ptr = local_matrix.kcol();
  int *local_cols_ptr = local_matrix.cols();
  double *local_values_ptr = local_matrix.values();

  MPI_Recv(local_kcol_ptr, localSize, MPI_INT, 0, 50, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  MPI_Recv(local_cols_ptr, n_nnz, MPI_INT, 0, 40, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  MPI_Recv(local_values_ptr, n_nnz, MPI_DOUBLE, 0, 30, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  int offset = local_kcol_ptr[0];
  for (int i=0; i<localSize; i++){
    local_kcol_ptr[i] = local_kcol_ptr[i] - offset;
  }


  }

  std::vector<double> x;
  {
  // BROAD CAST VECTOR X
  x.resize(nrows) ;
  MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
  }

  {
    Timer::Sentry sentry(timer,"SpMV_mpi") ;
    y_local.resize(localSize - 1);
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

MPI_Gatherv(y_local.data(), localSize-1, MPI_DOUBLE, y_final.data(), tab_local_sizes.data(), displacements.data(),
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
  //timer.printInfo();

  std::ofstream outfile;
  outfile.open("Benchmark.log", std::ios_base::app);
  outfile << "SPMV  np: " << nb_proc << "  nx: " << nx << "  Time SPMV: "
          << timer.getTime(0) << " Time MPI: " << mpi_finalTime
          << " Acceleration: " << timer.getTime(0)/mpi_finalTime << std::endl;

}

  MPI_Finalize();
  return 0 ;
}
