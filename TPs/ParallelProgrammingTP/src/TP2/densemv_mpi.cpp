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
#include <tbb/tbb.h>

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

  {
	std::string name_timer = "MPIDenseMV";
     	name_timer = name_timer + std::to_string(my_rank);
  	Timer::Sentry sentry(timer, name_timer);
  }
  {
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
      			int nrows_sent=nrows;
			MPI_Bcast(&nrows_sent, 1, MPI_INT, 0, MPI_COMM_WORLD); 

		      	/******* SEND MATRIX ******/
		      	int begin =0;
      			{
	      			int nrows_local = nrows/nb_proc;
			        int reste = nrows%nb_proc;
	      			if(0<reste){
	      				nrows_local++;
	     	 		}
	      			begin +=nrows_local;
      			}

      			for (int i=1; i<nb_proc;++i)
      			{
        			//std::cout<<" SEND MATRIX DATA to proc "<<i<<std::endl ;

	        		// SEND LOCAL SIZE to PROC I
				int nrows_local = nrows/nb_proc;
			       	int r= nrows%nb_proc;
				if(i<r){
					nrows_local++;
				}
			MPI_Send(&nrows_local, 1, MPI_INT, i,1000, MPI_COMM_WORLD);
        		// SEND MATRIX DATA
			const double * ptr = values.data() + begin*nrows;
			MPI_Send(ptr, nrows_local*nrows, MPI_DOUBLE, i, 2000, MPI_COMM_WORLD);	
			begin += nrows_local;
	      		}
    		}
	   	{	
      			/***** BROAD CAST VECTOR X ******/
			MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        		//std::cout<<" SEND VECTOR DATA to the procs"<<std::endl ;
    		}	

		/****** COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0******/
    
		std::size_t local_nrows ;	
    		{
			//COMPUTE LOCAL SIZE
			int nrows_local = nrows/nb_proc;
		       	int r= nrows%nb_proc;
			if(0<r){
				nrows_local++;
			}
		    	local_nrows=(size_t)nrows_local;
		}
    
	    	/***** COMPUTE LOCAL Y ON PROC 0  ******/
		std::vector<double> local_y(local_nrows);
    
    		{
		      	// compute parallel SPMV      
			const double * ptr = values.data();
			for(std::size_t irow=0; irow<local_nrows;++irow)
			{
				double value =0;
				for(std::size_t jcol =0; jcol<nrows;++jcol)
				{
					value += ptr[jcol]*x[jcol];
				}
				local_y[irow] = value;
				ptr +=nrows;
			}
	    	}

		/****** COMPUTE FINAL Y ******/
		std::vector<double> y;
		//int pos =local_nrows;
   		{
			//y.insert(y.begin(), local_y.begin(), local_y.end());
			y=local_y;	
      			for (int i=1; i<nb_proc;++i)
      			{
				int nrows_local = nrows/nb_proc;
		       		int r= nrows%nb_proc;
				if(i<r){
					nrows_local++;
				}
		
				std::vector<double> local_values (nrows_local*nrows);	
				MPI_Status status;

	
				MPI_Recv(local_values.data(), nrows_local, MPI_DOUBLE, i, 4000, MPI_COMM_WORLD, &status);

		        	//std::cout<<" RECEIVE LOCAL Y from proc "<<i<<" size "<<nrows_local<<std::endl ;

				y.insert(y.end(), local_values.begin(), local_values.end());
				//pos+=nrows_local;
			}
		 }
	      	double normy = PPTP::norm2(y) ;
	      	std::cout<<"||y||="<<normy<<std::endl ;
		 
  		timer.printInfo() ;
  	} 
  
  	else
  	{

    		//DenseMatrix local_matrix ;
	    	std::size_t nrows ;
    		std::size_t local_nrows ;
		std::vector<double> local_matrix_values; 

    		{
      			/***** RECV DATA FROM MASTER PROC*****/
      
      			// RECV GLOBAL SIZE

			int nrows_to_receive;
			MPI_Bcast(&nrows_to_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);
			//std::cout<<" receive Bcast"<<my_rank<<" value="<< nrows_to_receive<< std::endl;
	     		nrows = (size_t)nrows_to_receive;	
	
			// RECV LOCAL SIZE
			int nrows_local_receive;
			MPI_Status status;	
			MPI_Recv(&nrows_local_receive, 1, MPI_INT, 0,1000, MPI_COMM_WORLD, &status);
        
			//std::cout<<" receive local_rows"<<my_rank<<" value="<< nrows_local_receive<< std::endl;
      
	     		local_nrows =(size_t)nrows_local_receive; 

			// RECV MATRIX DATA
			std::vector<double> local_values (nrows_local_receive*nrows_to_receive);
			MPI_Recv(local_values.data(), nrows_local_receive*nrows_to_receive, MPI_DOUBLE, 0, 2000, MPI_COMM_WORLD, &status);
			local_matrix_values = local_values;
    		}

	    	std::vector<double> x; 
    		x.resize(nrows) ;
	
	    	{
      			/******** BROAD CAST VECTOR X******/
			MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD); 
        		//std::cout<<" RECEIVE VECTOR DATA from the master "<<std::endl ;
	    	}

    		/****** COMPUTE LOCAL Y in current proc *****/
	    	std::vector<double> local_y(local_nrows);
	    	{
      			// compute parallel SPMV
			const double * ptr = local_matrix_values.data();
			for(std::size_t irow=0; irow<local_nrows;++irow)
			{
				double value =0;
				for(std::size_t jcol =0; jcol<nrows;++jcol)
				{
					value += ptr[jcol]*x[jcol];
				}
				local_y[irow] = value;
				ptr +=nrows;
			}
	    	}
	
    		/****** SEND THE LOCAL Y TO THE MASTER PROC TO COMPUTE THE GLOBAL Y *****/
	    	MPI_Send(local_y.data(), local_nrows, MPI_DOUBLE, 0, 4000, MPI_COMM_WORLD);

	        //std::cout<<" SEND LOCAL Y from proc "<<my_rank<<" size "<<local_nrows<<std::endl ;
	  }
  }

  //Finalyze MPI
  MPI_Finalize();

  return 0 ;
}
