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
    // Timer::Sentry sentry(timer,"ifDenseMV") ;
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
    // matrix.printline(0);
    // matrix.printline(1);
    // matrix.printline(2);
    // matrix.printline(3);
    // matrix.printline(4);
    std::vector<double> x;
    x.resize(nrows) ;
    double * matrix_ptr = matrix.data() ;
    double *local_matrix_ptr = matrix_ptr + (nrows * (nrows / nb_proc)); // first lines of process 0
    // std::cout<<"Global matrix"<<std::endl ;
    // for (size_t i=0; i < (matrix.size() / nrows); ++i) {
    //  matrix.printline(i);
    // }
    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;
    {
      // SEND GLOBAL SIZE
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
      //std::cout<<"Sent Globalsize: "<<nrows<<std::endl ;
      // SEND MATRIX
      for (int i=1; i<nb_proc;++i)
      {
        // std::cout<<" SEND MATRIX DATA to proc "<<i<<std::endl ;
        int r = nrows % nb_proc;
        int q = nrows / nb_proc;
        localSize = q;
        if (i<=r) {localSize++;};
        // SEND LOCAL SIZE to PROC I
        MPI_Ssend(&localSize, 1, MPI_UNSIGNED_LONG, i, 10, MPI_COMM_WORLD) ;
        tab_local_sizes[i] = (int) localSize;
        // std::cout<<"Sent localsize: "<<localSize<<std::endl ;
        // SEND MATRIX DATA
        //int epsilon = 0;
        //if (r>0) epsilon = 1;
                // nrows = nombre de colonnes. q+epsilon = nombre de lignes de chaque processus
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
        // Timer::Sentry sentry(timer,"DenseMV") ;
        Timer::Sentry sentry(timer,"DenseMV_seq") ;
        matrix.mult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"Complete matrix result ||y||="<<normy<<std::endl ;
    }
    // COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
    DenseMatrix local_matrix ;
    //std::size_t localSize ;
    {
      // EXTRACT LOCAL DATA FROM MASTER PROC
      // COMPUTE LOCAL SIZE
      localSize = nrows / nb_proc;
      tab_local_sizes[0] = (int) localSize;//XXXXXXXXXXXXXXXXXX
      // EXTRACT LOCAL MATRIX DATA  // REDO
      local_matrix.init(localSize, nrows);
      local_matrix_ptr = local_matrix.data();
      for (unsigned int i=0; i<localSize*nrows; i++, matrix_ptr++) {
        local_matrix_ptr[i] = *matrix_ptr;
      }
      // std::cout<<"Matrix "<<my_rank<<std::endl ;
      // for (size_t i=0; i < (local_matrix.size() / nrows); ++i) {
      //  local_matrix.printline(i);
      // }
    }
    y_local.resize(localSize);
    {
      // compute parallel SPMV
      std::string s("DenseMV_MPI_"+std::to_string(my_rank));
      Timer::Sentry sentry(timer, s);
      // Timer::Sentry sentry(timer,"LocalDenseMV") ;
      local_matrix.mult(x, y_local);
    }
  }
  else
  {
    // Timer::Sentry sentry(timer,"elseDenseMV") ;
    // COMPUTE LOCAL MATRICE LOCAL VECTOR
    DenseMatrix local_matrix ;
    // std::size_t localSize ;
    {
      // RECV DATA FROM MASTER PROC
      // RECV GLOBAL SIZE
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
      //std::cout<<"Received Global size: " << nrows << std::endl ;
      // RECV LOCAL SIZE
      MPI_Recv(&localSize,1,MPI_UNSIGNED_LONG,0,10,MPI_COMM_WORLD, MPI_STATUS_IGNORE) ; // ,*status // Srecv
      //std::cout << "localSize" << localSize << std::endl;
      //RECV MATRIX DATA
      local_matrix.init(localSize, nrows);
      double *local_matrix_ptr = local_matrix.data();
      MPI_Recv(local_matrix_ptr, nrows * localSize, MPI_DOUBLE, 0, 50, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      //std::cout << "x[0] " << local_matrix_ptr[0] << std::endl;
      //std::cout << "Process " << my_rank << std::endl;
      // std::cout<<"Matrix "<<my_rank<<std::endl ;
      // for (size_t i=0; i < (local_matrix.size() / nrows); ++i) {
      //  local_matrix.printline(i);
      // }
    }
      std::vector<double> x;
    {
      // BROAD CAST VECTOR X
      x.resize(nrows) ;
      MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
      //std::cout << "x[0]" << x[0] << std::endl;
      //std::cout << "x[1]" << x[1] << std::endl;
    }
    {
      //double const *local_matrix_ptr = local_matrix.data();
      y_local.resize(localSize);
      // Timer::Sentry sentry(timer,"LocalDenseMV") ;
      std::string s("DenseMV_MPI_"+std::to_string(my_rank));
      Timer::Sentry sentry(timer, s);
      local_matrix.mult(x, y_local);
    }
  }
  std::vector<double> y_final;
  //std::cout << "localSize: " << localSize << "  ,  actual size: " << y_local.size() << " , Rank:" << my_rank << std::endl;
  //std::cout << "y_final size avant: " << y_final.size() << " , Rank:" << my_rank << std::endl;
  if (my_rank == 0) {
    y_final.resize(nrows);
    displacements[0] = 0;
    //std::cout<<"||displacements|| = "<< displacements[0] << "  tab_local_sizes: " << tab_local_sizes[0] << "  Rank: " << my_rank <<std::endl ;
    for (int i = 1; i < nb_proc; ++i) {
      displacements[i] = (int) (displacements[i-1] + tab_local_sizes[i-1]);
      //std::cout<<"||displacements|| = "<< displacements[i] << "  tab_local_sizes: " << tab_local_sizes[i] << "  Rank: " << my_rank <<std::endl ;
    }
  }
  //double normy_final = PPTP::norm2(y_local) ;
  //std::cout<<"||y_local||=" << normy_final << " , Rank:" << my_rank << std::endl;
  //MPI_Gather(y_local.data(), localSize, MPI_DOUBLE, y_final.data(), localSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Gatherv(y_local.data(), localSize, MPI_DOUBLE, y_final.data(), tab_local_sizes.data(), displacements.data(),
              MPI_DOUBLE, 0, MPI_COMM_WORLD); //XXXXXXXXXXXXXXXXXX

  if (my_rank == 0) {
    double normy_final = PPTP::norm2(y_final) ;
    std::cout<<"MPI result ||y||=" << normy_final << " , Rank:" << my_rank << std::endl;
    //std::cout << "y_final size: " << y_final.size() << " , Rank:" << my_rank << std::endl;
  }
  MPI_Finalize();
  timer.printInfo() ;
  return 0 ;
}
