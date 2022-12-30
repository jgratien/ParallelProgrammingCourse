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
  
//______________________________________________________________________________________  
    
  
  MPI_Init(&argc,&argv) ;

  int my_rank = 0 ;
  int nb_proc ;
  
  std::size_t local_nrows;
  std::vector<double> local_y;
  std::size_t nrows;
  
  MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;
  
  double mpi_time = 1;  
  double mpi_finalTime;
    
//______________________________________________________________________________________  
  
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

//______________________________________________________________________________________  

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
    std::vector<double> x,y;
	x.resize(nrows) ;
	y.resize(nrows) ;

    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;
  
    {
	    Timer::Sentry sentry(timer,"DenseMV") ;   
        matrix.mult(x,y) ;
    }  
	  
	double normy = PPTP::norm2(y) ;
    std::cout<<"||y_seq||="<<normy<<std::endl ;

    {

      // SEND GLOBAL SIZE
	  MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);


      // SEND MATRIX
	  
      for (int i=1; i<nb_proc;++i)
      {
        std::cout<<" SEND MATRIX DATA to proc "<<i<<std::endl ;
		
		local_nrows = nrows / nb_proc;

        // SEND LOCAL SIZE to PROC I
		MPI_Send(&local_nrows, 1, MPI_UNSIGNED_LONG, i, 10, MPI_COMM_WORLD) ;
		

        // SEND MATRIX DATA
		std::size_t start = i * local_nrows*nrows;
		double const* matrix_ptr = matrix.data() + start ;
		MPI_Send(matrix_ptr, nrows * local_nrows, MPI_DOUBLE, i, 50, MPI_COMM_WORLD);
      }
    }

    {
      // BROAD CAST VECTOR X
      MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
    }


    // COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
    DenseMatrix local_matrix ;
    

    {
      // EXTRACT LOCAL DATA FROM MASTER PROC

      // COMPUTE LOCAL SIZE
	  
	  //int rest = nrows % nb_proc;
	  local_nrows = nrows / nb_proc ;
	  	
	  local_matrix.init(local_nrows, nrows);

      // EXTRACT LOCAL MATRIX DATA
	  
	   for (auto i=0;i<local_nrows;i++)
	  {
		  for (auto j=0;j<nrows;j++)
		  {
			  local_matrix(i,j)=matrix(i,j);
		  }
	  }
	  
    }

    local_y.resize(local_nrows);
	
	std::vector<double> y_final;
    
	{
      std::string s("DenseMV_MPI_"+std::to_string(my_rank));
	  Timer::Sentry sentry(timer,s) ;
	  local_matrix.mult(x,local_y);

    }
	

    // SAVING LOCAL RESULTS TO THE GLOBAL Y
	
	y_final.resize(nrows);
	
	for (auto i=0;i<local_nrows;i++)
	{
		y_final[i]=local_y[i];
	}
	
	for (auto i=1;i<nb_proc;i++)
	{
		MPI_Status status;
		
		// RECV RESULTS FROM OTHER PROCS
		
		MPI_Recv(y_final.data()+i*local_nrows, local_nrows, MPI_DOUBLE, i, 0,MPI_COMM_WORLD,&status);
	}
	
	double normy_final = PPTP::norm2(y_final) ;
    std::cout<<"MPI result ||y_final||=" << normy_final << std::endl;
	
  }
  else
  {
    // COMPUTE LOCAL MATRICE LOCAL VECTOR

    DenseMatrix local_matrix ;


    {
      // RECV DATA FROM MASTER PROC

      // RECV GLOBAL SIZE
	  
	  MPI_Status status;
	  MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

      // RECV LOCAL SIZE
	  
	  MPI_Recv(&local_nrows,1,MPI_UNSIGNED_LONG,0,10,MPI_COMM_WORLD, &status) ; 
	  std::cout<<"local size received\n";
	  

      // RECV MATRIX DATA
	  local_matrix.init(local_nrows, nrows);
	  MPI_Recv(local_matrix.data(), local_nrows*nrows, MPI_DOUBLE, 0, 50, MPI_COMM_WORLD,&status) ;
    }

    std::vector<double> x;
    {
      // BROAD CAST VECTOR X
      x.resize(nrows) ;
      MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
    }

    local_y.resize(local_nrows);
    {
      std::string s("DenseMV_MPI_"+std::to_string(my_rank));
	  Timer::Sentry sentry(timer,s) ;
	  local_matrix.mult(x,local_y);
    }
	
	MPI_Send(local_y.data(), local_nrows, MPI_DOUBLE, 0,0, MPI_COMM_WORLD) ;
  }

 if (my_rank == 0) 
{
	mpi_time = timer.getTime(1);
}
else
{
	mpi_time = timer.getTime(0);
}

MPI_Allreduce(&mpi_time, &mpi_finalTime, 2, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
 
if (my_rank == 0) {
	
  std::ofstream outfile;
  outfile.open("Bench.log",std::ios_base::app);
  outfile << "DenseMV  np: " << nb_proc << "  nx: " << nx << "  Time DenseMV: "
          << timer.getTime(0) << " Time MPI: " << mpi_finalTime
          << " Acceleration: " << timer.getTime(0)/mpi_finalTime << std::endl;

 
 }  
  timer.printInfo() ;

  MPI_Finalize() ;

  return 0 ;
}