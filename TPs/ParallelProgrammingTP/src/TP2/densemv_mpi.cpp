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
#include "tbb/tbb.h"

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
    std::vector<double> const &values = matrix.get_values();
    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;

    {

      // SEND GLOBAL SIZE
      int nrows_to_send = nrows;
      MPI_Bcast(&nrows_to_send, 1, MPI_INT, 0, MPI_COMM_WORLD);

      


      // SEND MATRIX
      int begin =0;
      int nrows_local_proc_zero;
      {
	nrows_local_proc_zero = nrows/nb_proc;
	int r = nrows%nb_proc;
	if(r>0) nrows_local_proc_zero ++;
	begin += nrows_local_proc_zero;
      }

      for (int i=1; i<nb_proc;++i)
      {

        // SEND LOCAL SIZE to PROC I
	int nrows_local = nrows/nb_proc;
	int r = nrows%nb_proc;
	if(i<r) {
		nrows_local += 1;
	}
	MPI_Send(&nrows_local, 1, MPI_INT, i, 1000, MPI_COMM_WORLD);

        // SEND MATRIX DATA
	double const * ptr = values.data() + begin*nrows;
	MPI_Send(ptr, nrows_local*nrows, MPI_DOUBLE, i, 2000, MPI_COMM_WORLD);
	begin += nrows_local; 
	
      }
	
      // BROAD CAST VECTOR X
      int vector_size = nrows;
      double *ptr_vec = x.data();
      MPI_Bcast(ptr_vec, vector_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
     

      double total_sum=0;
      {
	Timer::Sentry sentry(timer,"DenseMV_PARALLEL") ;


      // COMPUTE LOCAL MULT OF PROC 0
      std::vector<double> local_y(nrows_local_proc_zero);
      {
	
	for(int i=0; i<nrows_local_proc_zero; i++)
	{
	  double sum=0;
          for(int j=0; j<nrows; j++)
          {
	    sum += values[nrows*i+j]*x[j];
          }
	  local_y[i] = sum;
	  total_sum += sum*sum;
	}
      }
 


      // RECEIVE LOCAL_Y FROM PROC I
      MPI_Status status;
      for(int i=1; i<nb_proc; i++)
      {
        
	int nrows_local = nrows/nb_proc;
	int r = nrows%nb_proc;
	if(i<r) {
		nrows_local += 1;
	}
	std::vector<double> local_y_to_receive(nrows_local);

	MPI_Recv(local_y_to_receive.data(), nrows_local, MPI_DOUBLE, i, 3000, MPI_COMM_WORLD, &status);

	for(int k=0; k<local_y_to_receive.size(); k++)
	{
          total_sum += local_y_to_receive[k]*local_y_to_receive[k];
	}
      }
     }

      // COMPUTE NORM
      double norm = std::sqrt(total_sum);
      std::cout << "||y_parallel||=" << norm << std::endl;
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

    timer.printInfo() ;


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
      int nrows_to_receive;
      MPI_Bcast(&nrows_to_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);
      

      // RECV LOCAL SIZE
      int nrows_local_to_receive;
      MPI_Status status;
      MPI_Recv(&nrows_local_to_receive, 1, MPI_INT, 0, 1000, MPI_COMM_WORLD, &status);
      


      // RECV MATRIX DATA

      std::vector<double> local_values (nrows_local_to_receive*nrows_to_receive);
      MPI_Recv(local_values.data(), nrows_local_to_receive*nrows_to_receive, MPI_DOUBLE, 0, 2000, MPI_COMM_WORLD, &status);
      

      // BROAD CAST VECTOR X
      std::vector<double> x(nrows_to_receive);
      MPI_Bcast(x.data(), nrows_to_receive, MPI_DOUBLE, 0, MPI_COMM_WORLD);

      // COMPUTE PARALLEL DENSEMV
      std::vector<double> local_y(nrows_local_to_receive);
      {

	for(int i=0; i<nrows_local_to_receive; i++)
	{
	  double sum=0;
          for(int j=0; j<nrows_to_receive; j++)
          {
	    sum += local_values[nrows_to_receive*i+j]*x[j];
          }
	  local_y[i] = sum;
	}
      }
      
      // SEND EACH LOCAL_Y TO PROC 0
      MPI_Send(local_y.data(), nrows_local_to_receive, MPI_DOUBLE, 0, 3000, MPI_COMM_WORLD);

    }

  }
  MPI_Finalize();
  return 0 ;
}
