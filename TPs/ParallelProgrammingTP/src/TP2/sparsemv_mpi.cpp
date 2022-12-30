/*
 * helloworld.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: gratienj
 *      Modified by Dimitrios Tsitsos in the framework of the MS HPC-AI, on 11/12/2022.
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
		return 1;
	}
  
	MPI_Init(&argc,&argv) ;
	int my_rank = 0;
	int nb_proc;
	MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;  

/*------------------------------ Declaration-------------------------------*/
	std::size_t n_nnz;
	std::size_t first_nnz;
	std::size_t local_nrows;
	std::vector<double> local_y;
	std::size_t nrows;
/*-------------------------------------------------------------------------*/
  
	using namespace PPTP ;
  
	Timer timer; 
	Timer timer2;
  
	MatrixGenerator generator ;
	int nx = vm["nx"].as<int>() ;

	if(vm["eigen"].as<int>()==1)
	{
		typedef Eigen::SparseMatrix<double> MatrixType ;
		typedef Eigen::VectorXd             EigenVectorType ;
	    MatrixType matrix (nrows,nrows);
		std::size_t nrows = matrix.rows();
		generator.genLaplacian(nx, matrix);
		EigenVectorType x(nrows);
	
		for(std::size_t i=0;i<nrows;++i)
			x(i) = i+1 ;
  
		EigenVectorType y;
		{
			y = matrix*x ;
		}
	
		double normy = PPTP::norm2(y) ;
		std::cout << "||y||=" << normy << std::endl;
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
	
		std::size_t nrows = matrix.nrows(); 
		// first lines - processor 0 
		int *local_kcol_ptr = matrix.kcol();
		first_nnz = local_kcol_ptr[(nrows / nb_proc)] - local_kcol_ptr[0]; 
		local_kcol_ptr = local_kcol_ptr + (nrows / nb_proc); 
		double *local_values_ptr = matrix.values() + first_nnz; 
		int *local_cols_ptr = matrix.cols() + first_nnz;
    	
		std::vector<double> x(nrows);
		std::vector<double> y(nrows) ;

		for(std::size_t i=0;i<nrows;++i)
			x[i] = i+1 ;
		
		{
			Timer::Sentry sentry(timer,"SparseMV") ;
			matrix.mult(x,y) ;
		}
	  
		double normy = PPTP::norm2(y) ;
		std::cout<<"Sequential ||y||="<<normy<<std::endl ;
		timer.printInfo();


		// SEND GLOBAL SIZE
		MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);	
		std::vector<double> y_final(nrows);

		{
			Timer::Sentry sentry(timer2,"MPISparseMV") ;	  
			// SEND MATRIX
			for (int i=1; i<nb_proc;++i)
			{	
  
				local_nrows = nrows/nb_proc + 1;		
				int idx_start = local_kcol_ptr[0];
				int idx_end = local_kcol_ptr[ local_nrows - 1 ] - 1;
				n_nnz = idx_end - idx_start + 1;
		        
				// SEND LOCAL SIZE to PROC I	
				MPI_Send(&local_nrows, 1, MPI_UNSIGNED_LONG, i, 10, MPI_COMM_WORLD) ;
				MPI_Send(&n_nnz, 1, MPI_UNSIGNED_LONG, i, 20, MPI_COMM_WORLD) ;
				MPI_Send(local_kcol_ptr, local_nrows, MPI_INT, i, 50, MPI_COMM_WORLD);
				MPI_Send(local_cols_ptr, n_nnz, MPI_INT, i, 40, MPI_COMM_WORLD);
				MPI_Send(local_values_ptr, n_nnz, MPI_DOUBLE, i, 30, MPI_COMM_WORLD);

				local_kcol_ptr = local_kcol_ptr + local_nrows - 1 ;
				local_cols_ptr = local_cols_ptr + n_nnz;
				local_values_ptr = local_values_ptr + n_nnz;
			}
		
	
			// BROAD CAST VECTOR X
			MPI_Bcast(&x[0], nrows, MPI_DOUBLE,0, MPI_COMM_WORLD) ;
	  
			// COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
			CSRMatrix local_matrix ;
	
			// EXTRACT LOCAL DATA FROM MASTER PROC
			
			// COMPUTE LOCAL SIZE
			int div = nrows/nb_proc;
			local_nrows = div + 1;
	  
			// EXTRACT LOCAL MATRIX DATA
			local_matrix.init(local_nrows, first_nnz);
			double *local_values_ptr = local_matrix.values();
			int *local_cols_ptr = local_matrix.cols();
			int *local_kcol_ptr = local_matrix.kcol();
	  
			for (unsigned int i=0;i<first_nnz;i++)
			{
				local_values_ptr[i] = matrix.values()[i];
				local_cols_ptr[i] = matrix.cols()[i];
			}
	  
			for (unsigned int i=0; i<local_nrows; i++)
			{
				local_kcol_ptr[i] = matrix.kcol()[i];
			}

			local_y.resize(local_nrows - 1);
//			std::vector<double> y_final(nrows);
			local_matrix.mult(x, local_y);
	
			// SAVING LOCAL RESULTS TO THE GLOBAL Y
			for (auto i=0;i<local_nrows;i++)
			{
				y_final[i]=local_y[i];
			}
	
			for (auto i=1;i<nb_proc;i++)
			{
				MPI_Status status;
				// RECV RESULTS FROM OTHER PROCS
				MPI_Recv(y_final.data()+i*(local_nrows - 1 ), local_nrows - 1, MPI_DOUBLE, i, 0,MPI_COMM_WORLD,&status);
			}
		}	
		double normy_final = PPTP::norm2(y_final) ;
		std::cout<<"MPI ||z||=" << normy_final << std::endl;	

		timer2.printInfo();
	
		double timer_seq = timer.sum_time();
		double timer_par = timer2.sum_time();
		std::ofstream fs;
		fs.open("time_sparse.csv",std::ios_base::app);
		fs << nb_proc << ";" << nx << ";" << timer_seq << ";" << timer_par << ";" << timer_seq/timer_par <<"\n";
		fs.close();
	}
	else
	{
		CSRMatrix local_matrix ;
		{
			// RECV DATA FROM MASTER PROC 
			MPI_Status status;
			
			// RECV GLOBAL SIZE  
			MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  
			// RECV LOCAL SIZE  
			MPI_Recv(&local_nrows,1,MPI_UNSIGNED_LONG,0,10,MPI_COMM_WORLD, &status) ;
			MPI_Recv(&n_nnz,1,MPI_UNSIGNED_LONG,0,20,MPI_COMM_WORLD, &status) ;
	
			//RECV MATRIX DATA
			local_matrix.init(local_nrows, n_nnz); 
			int *local_kcol_ptr = local_matrix.kcol();
			int *local_cols_ptr = local_matrix.cols();
			double *local_values_ptr = local_matrix.values();
  
			MPI_Recv(local_kcol_ptr, local_nrows, MPI_INT, 0, 50, MPI_COMM_WORLD, &status);
			MPI_Recv(local_cols_ptr, n_nnz, MPI_INT, 0, 40, MPI_COMM_WORLD, &status);
			MPI_Recv(local_values_ptr, n_nnz, MPI_DOUBLE, 0, 30, MPI_COMM_WORLD, &status);
			
			int offset = local_kcol_ptr[0];
  
			for (int i=0; i<local_nrows; i++)
			{
				local_kcol_ptr[i] = local_kcol_ptr[i] - offset;  
			}
		}
		
		std::vector<double> x(nrows);

		{
			// BROAD CAST VECTOR X
			MPI_Bcast(&x[0], nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
		}
   
		local_y.resize(local_nrows - 1);
		
		local_matrix.mult(x, local_y);
  
		MPI_Send(local_y.data(), local_nrows - 1 , MPI_DOUBLE, 0,0, MPI_COMM_WORLD) ; 
	} 
	MPI_Finalize();
	return 0 ;

}