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
  std::size_t n_nnz;
  std::size_t first_nnz;
  std::vector<double> y_local;
  std::size_t nrows;
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
      int nx = vm["nx"].as<int>() ;
      generator.genLaplacian(nx,matrix) ;
    }
    //std::cout<<"cols "<<matrix.cols().size() <<" values "<<matrix.values().size() <<" kcols "<<matrix.kcol().size()<<std::endl ;
    std::size_t nrows = matrix.nrows();
    int *local_kcol_ptr = matrix.kcol(); // first lines of process 0
    first_nnz = local_kcol_ptr[(nrows / nb_proc)] - local_kcol_ptr[0];
    local_kcol_ptr = local_kcol_ptr + (nrows / nb_proc);
    double *local_values_ptr = matrix.values() + first_nnz;
    int *local_cols_ptr = matrix.cols() + first_nnz;
    std::vector<double> x,y ;
    x.resize(nrows) ;
    y.resize(nrows) ;
    //std::cout<<"first nnz= "<< first_nnz <<std::endl ;
    //for (int i=0; i<matrix.cols().size(); i++){std::cout<<"cols = "<< cols_ptr[i] << " values" << values_ptr[i]  <<std::endl ;}
    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;
    {
      // std::string s("DenseMV_MPI_"+std::to_string(my_rank));
      // Timer::Sentry sentry(timer, s);
      // // Timer::Sentry sentry(timer,"SpMV") ;
      // matrix.mult(x,y) ;
      // double normy = PPTP::norm2(y) ;
      // std::cout<<"Standard value: ||y||="<<normy<<std::endl ;

      Timer::Sentry sentry(timer,"SpMV_seq") ;
      matrix.mult(x,y) ;
    }
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
        std::cout<<"r="<<r<<"q="<<q<<std::endl ;
        localSize = q + 1;
        if (i<=r) {localSize++;}; // localSize = taille de local_kcols_ptr
        int idx_start = local_kcol_ptr[0];
        int idx_end = local_kcol_ptr[localSize - 1] - 1;
        n_nnz = idx_end - idx_start + 1;
        std::cout<< "LocalSize " << localSize <<" idx_start "<<idx_start << " idx_end " << idx_end << " n_nnz " << n_nnz<<std::endl ;
        // SEND LOCAL SIZE to PROC I
        MPI_Ssend(&localSize, 1, MPI_UNSIGNED_LONG, i, 10, MPI_COMM_WORLD) ;
        MPI_Ssend(&n_nnz, 1, MPI_UNSIGNED_LONG, i, 20, MPI_COMM_WORLD) ;
        tab_local_sizes[i] = (int) localSize - 1;
        //std::cout << "Sent sizes Rank:" << my_rank << std::endl;
        MPI_Ssend(local_kcol_ptr, localSize, MPI_INT, i, 50, MPI_COMM_WORLD);
        //std::cout << "sending n_local_kcol " << n_nnz << " Rank:" << my_rank << std::endl;
        MPI_Ssend(local_cols_ptr, n_nnz, MPI_INT, i, 40, MPI_COMM_WORLD);
        MPI_Ssend(local_values_ptr, n_nnz, MPI_DOUBLE, i, 30, MPI_COMM_WORLD);
        //std::cout << "Already sent n_nnz " << n_nnz << " Rank:" << my_rank << std::endl;
        local_kcol_ptr = local_kcol_ptr + localSize - 1;
        local_cols_ptr = local_cols_ptr + n_nnz;
        local_values_ptr = local_values_ptr + n_nnz;
      }
    }
    CSRMatrix local_matrix ;
    //for (int i=0; i<matrix.cols().size(); i++){std::cout<<"cols = "<< cols_ptr[i] << " values " << values_ptr[i]  <<std::endl ;}
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
      // for (int i=0; i<matrix.cols().size(); i++){std::cout<<"cols = "<< cols_ptr[i] << " values" << cols_ptr[i]  <<std::endl ;}
      // std::cout <<  "  " << local_matrix.kcol() << " " << localSize << " " << local_matrix.values().size() << " " << first_nnz<< std::endl;
      std::cout << "localsize>" << localSize << "first_nnz" << first_nnz<< std::endl;
      for (unsigned int i=0; i<first_nnz; i++) {
        local_values_ptr[i] = matrix.values()[i];
        local_cols_ptr[i] = matrix.cols()[i];
        //std::cout << "Mvalues: " << matrix.values()[i] << " Mcols: " << matrix.cols()[i] << std::endl;
        //std::cout << "values: " << local_values_ptr[i] << " cols: " << local_cols_ptr[i] << std::endl;
      }
      for (unsigned int i=0; i<localSize; i++) {
        local_kcol_ptr[i] = matrix.kcol()[i];
        //std::cout << "Mkcol: " << matrix.kcol()[i] << std::endl;
        //std::cout << "kcol: " << local_kcol_ptr[i] << std::endl;
      }
    }
    y_local.resize(localSize - 1);
    //std::cout << "y_local" << y_local.size() << " Rank:" << my_rank << std::endl;
    {
      // compute parallel SPMV
      std::string s("SpMV_MPI_"+std::to_string(my_rank));
      Timer::Sentry sentry(timer, s);
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
  //std::cout<<"Received Global size: " << nrows << std::endl ;
  // RECV LOCAL SIZE
  MPI_Recv(&localSize,1,MPI_UNSIGNED_LONG,0,10,MPI_COMM_WORLD, MPI_STATUS_IGNORE) ;
  MPI_Recv(&n_nnz,1,MPI_UNSIGNED_LONG,0,20,MPI_COMM_WORLD, MPI_STATUS_IGNORE) ;
  //std::cout << "localSize " << localSize << " Rank:" << my_rank << std::endl;
  //RECV MATRIX DATA
  local_matrix.init(localSize, n_nnz);
  int *local_kcol_ptr = local_matrix.kcol();
  int *local_cols_ptr = local_matrix.cols();
  double *local_values_ptr = local_matrix.values();
  //std::cout << "Avant receive Rank:" << my_rank << std::endl;
  MPI_Recv(local_kcol_ptr, localSize, MPI_INT, 0, 50, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  MPI_Recv(local_cols_ptr, n_nnz, MPI_INT, 0, 40, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  MPI_Recv(local_values_ptr, n_nnz, MPI_DOUBLE, 0, 30, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  int offset = local_kcol_ptr[0];
  // std::cout << "offset=" << offset << "localSize=" << localSize << "Avant receive Rank:" << my_rank << std::endl;
  for (int i=0; i<localSize; i++){
    // if (i == localSize -1 || i == localSize -2) {std::cout << "local_kcol_ptr[i]=" << local_kcol_ptr[i] << "localSize-i=" << localSize-i << std::endl;}
    local_kcol_ptr[i] = local_kcol_ptr[i] - offset;
    //std::cout << "kcol: " << local_kcol_ptr[i] << " Rank" << my_rank << std::endl;
  }
  // std::cout << "offset=" << offset << "localSize=" << localSize << "Avant receive Rank:" << my_rank << std::endl;
  }
  std::vector<double> x;
  {
  // BROAD CAST VECTOR X
  x.resize(nrows) ;
  MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
  }
  {
    y_local.resize(localSize - 1);
    std::string s("SpMV_MPI_"+std::to_string(my_rank));
    Timer::Sentry sentry(timer, s);
    local_matrix.mult(x, y_local);
  }
}
std::vector<double> y_final;
if (my_rank == 0) {
  y_final.resize(nrows);
  displacements[0] = 0;
  //std::cout<<"||displacements|| = "<< displacements[0] << "  tab_local_sizes: " << tab_local_sizes[0] << "  Rank: " << my_rank <<std::endl ;
  for (int i = 1; i < nb_proc; ++i) {
    displacements[i] = (int) (displacements[i-1] + tab_local_sizes[i-1]);
    //std::cout<<"||displacements|| = "<< displacements[i] << "  tab_local_sizes: " << tab_local_sizes[i] << "  Rank: " << my_rank <<std::endl ;
  }
}
double normy_final = PPTP::norm2(y_local) ;
//std::cout<<"||y_local||= " << normy_final << "  size: "<< y_local.size() << " , Rank:" << my_rank << std::endl;
MPI_Gatherv(y_local.data(), localSize-1, MPI_DOUBLE, y_final.data(), tab_local_sizes.data(), displacements.data(),
           MPI_DOUBLE, 0, MPI_COMM_WORLD);
if (my_rank == 0) {
  double normy_final = PPTP::norm2(y_final) ;
  std::cout<<"MPI result ||y||=" << normy_final << " , Rank:" << my_rank << std::endl;
  //std::cout << "y_final size: " << y_final.size() << " , Rank:" << my_rank << std::endl;
}
  MPI_Finalize();
  timer.printInfo();
  // ofstream outFile("temp.txt");
  // outFile<<normy_final;
  // outFile<<endl;
  return 0 ;
}
