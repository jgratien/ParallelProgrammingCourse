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
    std::vector<double> const &values = matrix.get_values();
    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;

    
      //int nrows_to_send = nrows;
      //MPI_Bcast(&nrows_to_send, 1, MPI_INT, 0, MPI_COMM_WORLD);

      // SEND GLOBAL SIZE
      int nrows_int = nrows;
      int nrows_local = nrows/nb_proc;
      int r = nrows % nb_proc; 
      MPI_Bcast(&nrows_int, 1, MPI_INT, 0, MPI_COMM_WORLD);

      // SEND MATRIX
      
      int begin = 0;
      int nrows_local = nrows/nb_proc;
      int r = nrows%nb_proc;

    {
      for (int i=1; i<nb_proc; ++i)
      {
        std::cout<<" SEND MATRIX DATA to proc "<<i<<std::endl ;

        // SEND LOCAL SIZE to PROC I
                               int nrows_local = nrows/nb_proc;
	                        //int r = nrows%nb_proc;
	if(i<r) {
	nrows_local++;}

	MPI_Send(&nrows_local, 1, MPI_INT, i, 1000, MPI_COMM_WORLD);
        // SEND MATRIX DATA
	double const * ptr = values.data() + begin*nrows;
	MPI_Send(ptr, nrows_local*nrows, MPI_DOUBLE, i, 2000, MPI_COMM_WORLD);
	begin += nrows_local;
      }
    }

    {
      // BROAD CAST VECTOR X
      
         std::vecttor<double> x_to_send;
       MPI_Bcast(&x_to_send, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

 
     
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
    std::vector<double> result;
    result.reserve(nrows);

    {
      // EXTRACT LOCAL DATA FROM MASTER PROC
      // double s ;
      double const * ptr0 ;
      int beg = 0 ;
      for(int i = 0; i<nrows_local; i++)
      {   
	      s = 0 ;
	      for(j=0; j<nrows; j++)
	  {
		  ptr0 = values.data() + beg;
		  s += (*ptr0) * x.at(j) ;
		  beg++;
	  }

            result.at(i) = s;    }

      // COMPUTE LOCAL SIZE
      


      // EXTRACT LOCAL MATRIX DATA
    }

    std::vector<double> local_y(local_nrows);
    {
      // compute parallel SPMV
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
      
      // RECV Global Size
      int nrows_to_receive = 0;
      MPI_Bcast(&nrows_to_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);
      std::cout<<" receive Broadcasted "<<my_rank<<" value="<<nrows_to_receive;
      

      // RECV LOCAL SIZE
      int nrows_local_receive;
      MPI_Status status;
      MPI_Recv(&nrows_local_receive, 1, MPI_INT, 0, 1000, MPI_COMM_WORLD, &status);
      std::cout<<" receive local_rows"<<my_rank<<" value="<< nrows_local_receive<< std::endl;
      
      //RECV MATRIX DATA
      std::vector<double> local_values (nrows_local_receive*nrows_to_receive);
      MPI_Recv(local_values.data(), nrows_local_receive*nrows_to_receive, MPI_DOUBLE, 0, 2000, MPI_COMM_WORLD, &status);
    }

   
    {
      // BROAD CAST VECTOR X
      //* ... */
      std::vector<double> x_to_receive;
      MPI_Bcast(&x_to_receive, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
      double const * ptr0 ;
      int beg = 0 ;
      for(int i = 0; i<nrows_local; i++)
      {   
	      s = 0 ;
	      for(j=0; j<nrows; j++)
	  {
		  ptr0 = values.data() + beg;
		  s += (*ptr0) * x_to_receive.at(j) ;
		  beg++;
	  }


      
    }

    std::vector<double> local_y(local_nrows);
    {
      // compute parallel SPMV
    }

  }
  timer.printInfo() ;
  return 0 ;
}
