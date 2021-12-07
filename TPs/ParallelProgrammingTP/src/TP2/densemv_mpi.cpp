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

  double data[nb_proc - 1];
  double result;
  double result_;

  if(my_rank==0)
  {
    // Timer::Sentry sentry(timer,"Rank 0 Begin") ;
    DenseMatrix matrix ;
    std::size_t local_size_ ;

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

    // std::cout << "nrows : " << matrix.nrows() << '\n';
    // std::cout << "ncols : " << matrix.nrows() << '\n';
    // for(int i = 0; i < matrix.nrows(); i++){
    //   for(int j = 0; j < matrix.nrows(); j++){
    //     std::cout << "Matrix " << i << "; " << j << " : " << matrix(i, j) << '\n';
    //   }
    // }

    std::size_t nrows = matrix.nrows();
    std::vector<double> x;
    x.resize(nrows) ;

    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;

    {

      // SEND GLOBAL SIZE
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

      // SEND MATRIX
      unsigned long q = nrows / nb_proc;
      unsigned long r = nrows % nb_proc;
      double* matrix_ptr = matrix.data();
      int epsilon = 0;
      if (r > 0) epsilon = 1;
      local_size_ = (q + epsilon);
      // std::cout << "local_size_ : " << local_size_ << '\n';
      double* local_ptr = matrix_ptr + nrows * local_size_;
      // std::cout << "nrows : " << nrows << '\n';
      for (int i=1; i<nb_proc;++i)
      {
        // std::cout<<" SEND MATRIX DATA to proc "<<i<<std::endl ;
        // SEND LOCAL SIZE to PROC I
        int local_size = q;
        if(i < r) local_size++;
        MPI_Send(&local_size, 1, MPI_INT, i, 100, MPI_COMM_WORLD);
        // std::cout << "Sent [rank : " << i << ", local_size : " << local_size << "]" << '\n';
        // std::cout << "test : " << test << '\n';
        MPI_Send(local_ptr, local_size * nrows, MPI_DOUBLE, i, 2000, MPI_COMM_WORLD);
        local_ptr += local_size * nrows;
        // SEND MATRIX DATA
      }
    }

    {
      // BROAD CAST VECTOR X
      MPI_Bcast(x.data(), x.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
      // for(double e : x){
      //   std::cout << "Sent : x values : " << e << '\n';
      // }
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
    std::vector<double> y_local(local_size_);

    {
      // EXTRACT LOCAL DATA FROM MASTER PROC
      local_matrix.init(local_size_, nrows);
      double* ptr = local_matrix.data();
      double const* matrix_ptr = matrix.data();
      for(int i = 0; i < (local_size_ * nrows); i++){
        ptr[i] = matrix_ptr[i];
      }

      // COMPUTE LOCAL SIZE
      matrix_ptr = local_matrix.data() ;
      for(std::size_t irow =0; irow<local_size_;++irow){
        double value = 0 ;
        for(std::size_t jcol =0; jcol<nrows;++jcol){
          value += matrix_ptr[jcol]*x[jcol] ;
        }
        y_local[irow] = value ;
        matrix_ptr += nrows ;
      }
      // std::cout << "Rank : " << my_rank << ", Done." << '\n';
      result = 0 ;
      for( auto const & x : y_local) result += x*x ;
      // std::cout << "Rank : " << my_rank << " and " << result << '\n';
      // EXTRACT LOCAL MATRIX DATA
    }
  }
  else
  {
    // COMPUTE LOCAL MATRICE LOCAL VECTOR

    DenseMatrix local_matrix ;
    std::size_t nrows ;
    std::size_t local_size ;

    {
      // RECV DATA FROM MASTER PROC

      // RECV GLOBAL SIZE
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

      // RECV LOCAL SIZE
      MPI_Status status;
      MPI_Recv(&local_size, 1, MPI_INT, 0, 100, MPI_COMM_WORLD, &status);
      // std::cout << "Received [rank: " << my_rank << ", local_size: " << local_size << "]" << '\n';
      local_matrix.init(local_size, nrows);
      double* ptr = local_matrix.data();
      MPI_Recv(ptr, local_size * nrows, MPI_DOUBLE, 0, 2000, MPI_COMM_WORLD, &status);
      // for(int e = 0; e < local_size * nrows; e++){
      //   std::cout << "[Received] Element [e : " << e << ", value : " << ptr[e] << "]" << '\n';
      // }

      // RECV MATRIX DATA
    }

    std::vector<double> x;
    x.resize(nrows);
    {
      // BROAD CAST VECTOR X
      MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      // for(double e : x){
      //   std::cout << "Received : x values : " << e << '\n';
      // }
    }

    std::vector<double> y_local(local_size);
    y_local.resize(local_size);
    {
      double const* matrix_ptr = local_matrix.data() ;
      for(std::size_t irow =0; irow<local_size;++irow){
        double value = 0 ;
        for(std::size_t jcol =0; jcol<nrows;++jcol){
          value += matrix_ptr[jcol]*x[jcol] ;
        }
        y_local[irow] = value ;
        matrix_ptr += nrows ;
      }

      // for(double e : y_local){
      //   std::cout << "[Checked] Element [e : " << e << "]" << '\n';
      // }

      // std::cout << "Rank : " << my_rank << ", Done." << '\n';
      result_ = 0 ;
      for( auto const & x : y_local) result_ += x*x ;
      // std::cout << "Rank : " << my_rank << " and " << result_ << '\n';
      // compute parallel SPMV
    }

  }
  MPI_Gather(&result_, 1, MPI_DOUBLE, data, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
      // Timer::Sentry sentry(timer,"Rank 0 End") ;
      int i;
      for (i=1; i < nb_proc; i++) {
          result += data[i];
      }
      result = std::sqrt(result);
      std::cout << "||y||=" << result << '\n';
  }

  timer.printInfo() ;
  MPI_Finalize() ;
  return 0 ;
}
