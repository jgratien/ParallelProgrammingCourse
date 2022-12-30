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
		return 1;
	}
	
	MPI_Init(&argc,&argv) ;
	int my_rank = 0 ;
	int nb_proc = 1 ;
	MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;
	
	using namespace PPTP ;
	
	Timer timer ;
	Timer timer2;
	
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
  
		EigenVectorType y;
		{
			y = matrix * x;
		}
		
		double normy = PPTP::norm2(y);
		std::cout<<"||y||="<<normy<<std::endl;
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
	
		for(std::size_t i=0;i<nrows;++i)
			x[i] = i+1 ;
		
		std::vector<double> y(nrows);
		{
			Timer::Sentry sentry(timer,"DenseMV") ;
			matrix.mult(x,y) ;
		}    
	   
/*			for(auto i = 0; i<nrows; i++){
				std::cout<<y[i]<<' ';
			}*/
		double normy = PPTP::norm2(y) ;
		std::cout<<"Sequential ||y||="<<normy<<std::endl ;
		timer.printInfo();
//		std::cout<<'\n';
/*------------------------------------ Way to Print the Matrix, we can use it several times in our code. ---------------------------------------
	// Print the Matrix
	for (std::size_t i = 0; i<nrows; i++) {
		for (std::size_t j = 0; j<nrows; j++) {
			std::cout << matrix(i,j) << " ";
		
		}
		std::cout << "\n";
      }
	  	std::cout << "\n";
--------------------------------------------------------------------------------------------------------------------------------------------------*/
		// SEND GLOBAL SIZE
		MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;

		std::size_t local_nrows = nrows/nb_proc ;
		std::vector<double> z(nrows);
		
		{
			Timer::Sentry sentry(timer2,"MPIDenseMV") ;
			// SEND MATRIX
			for (int i=1; i<nb_proc;++i)
			{
				// SEND LOCAL SIZE to PROC I
				MPI_Send(&local_nrows, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD) ;
		
				// SEND MATRIX DATA
				std::size_t start = i * local_nrows * nrows ;
				double const* matrix_ptr = matrix.data() + start ;
		
				MPI_Send(matrix_ptr, local_nrows*nrows, MPI_DOUBLE, i, 0, MPI_COMM_WORLD) ;
			}
		
	
			// BROAD CAST VECTOR X
			MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
		
			// COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
			DenseMatrix local_matrix ;
	
			// EXTRACT LOCAL DATA FROM MASTER PROC
			
			// COMPUTE LOCAL SIZE
			
			// EXTRACT LOCAL MATRIX DATA
			local_matrix.init(local_nrows,nrows) ;
			for (std::size_t i = 0; i<local_nrows; i++) {
				for (std::size_t j = 0; j<nrows; j++) {
					local_matrix(i,j) = matrix(i,j) ;
				}
			}
	
			std::vector<double> local_y(local_nrows);
    
			// compute parallel SPMV
			local_matrix.mult(x,local_y) ;
	
			for (std::size_t i = 0; i<local_nrows; i++) {
				z[i] = local_y[i] ;
			}
	
// ---------------------------------------------------- Receive ----------------------------------------------------------------//
	
			// RECEIVE LOCAL VECTORS Y
			for (int i=1; i<nb_proc; ++i)
			{ 
				MPI_Status status ;
				MPI_Recv(local_y.data(), local_nrows, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status);
	 
				for (std::size_t j = 0; j<local_nrows; j++) {
					z[i*local_nrows+j] = local_y[j] ;
				}
			}
	
/*			for(auto i = 0; i<nrows; i++){
				std::cout<<z[i]<<' ';
			}*/
		}
		double normz = PPTP::norm2(z) ;
		std::cout<<"MPI ||z||="<<normz<<std::endl ;
		
		timer2.printInfo();
//		std::cout<<'\n';

		double timer_seq = timer.sum_time();
		double timer_par = timer2.sum_time();
		std::ofstream fs;
		fs.open("time_dense.csv",std::ios_base::app);
		fs << nb_proc << ";" << nx << ";" << timer_seq << ";" << timer_par << ";" << timer_seq/timer_par <<"\n";
		fs.close();
 
	}
	else
	{
		// COMPUTE LOCAL MATRICE LOCAL VECTOR
		DenseMatrix local_matrix ;
		std::size_t nrows ;
		std::size_t local_nrows ;
	
		// RECV GLOBAL SIZE
		MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;
	
		{
			// RECV LOCAL SIZE
			MPI_Status status ;
			MPI_Recv(&local_nrows, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, &status);
			local_matrix.init(local_nrows, nrows);
	  
			// RECV MATRIX DATA
			std::vector<double> local_coeff(nrows*local_nrows) ;
			MPI_Recv(local_coeff.data(), nrows*local_nrows, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
	
			for (std::size_t i = 0; i<local_nrows; i++) {
				for (std::size_t j = 0; j<nrows; j++) {
					local_matrix(i,j) = local_coeff[i*nrows + j];
				}
			}	 
		}
	
		std::vector<double> x(nrows);
		{
			// BROAD CAST VECTOR X
			MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
		}
	
		std::vector<double> local_y(local_nrows);
		{
			// compute parallel SPMV
			local_matrix.mult(x,local_y) ;
		}
	
		// SEND BACK THE LOCAL VECTOR
		MPI_Send(local_y.data(), local_nrows, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD) ;
	
	}
	MPI_Finalize() ;
	return 0 ;
}

