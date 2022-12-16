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
	using namespace boost::program_options;
	options_description desc;
	desc.add_options()
		("help", "produce help")
		("nrows", value<int>()->default_value(0), "matrix size")
		("nx", value<int>()->default_value(0), "nx grid size")
		("file", value<std::string>(), "file input")
		("eigen", value<int>()->default_value(0), "use eigen package");
	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << "\n";
		return 1;
	}
	MPI_Init(&argc, &argv);

	int my_rank = 0;
	int nb_proc = 1;
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	using namespace PPTP;

	Timer timer, timer_seq, timer_mat;
	MatrixGenerator generator;
	int nx = vm["nx"].as<int>();
	if (vm["eigen"].as<int>() == 1)
	{
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType;
		typedef Eigen::Matrix<double, Eigen::Dynamic, 1>              EigenVectorType;

		std::size_t nrows = nx * nx;
		EigenMatrixType matrix(nrows, nrows);

		generator.genLaplacian(nx, matrix);

		EigenVectorType x(nrows);

		for (std::size_t i = 0; i < nrows; ++i)
			x(i) = i + 1;

		EigenVectorType y;
		{
			//Timer::Sentry sentry(timer,"EigenDenseMV") ;
			y = matrix * x;
		}
		double normy = PPTP::norm2(y);
		std::cout << "||y||=" << normy << std::endl;
	}

	if (my_rank == 0)
	{
		DenseMatrix matrix;

		if (vm.count("file"))
		{
			std::string file = vm["file"].as<std::string>();
			generator.readFromFile(file, matrix);
		}
		else
		{
			int nx = vm["nx"].as<int>();
			generator.genLaplacian(nx, matrix);
		}

		std::size_t nrows = matrix.nrows();
		std::vector<double> x;
		x.resize(nrows);

		for (std::size_t i = 0; i < nrows; ++i) {
			x[i] = i + 1;
		}

		// Squential dense matrix computation
		std::vector<double> y(nrows);
		{
			Timer::Sentry sentry(timer_seq, "DenseMV_seq");
			matrix.mult(x, y);
		}
		double normy = PPTP::norm2(y);
		std::cout << "||y_seq||=" << normy << std::endl;

		// SEND GLOBAL SIZE
		/*
		  In our test case the the number of row are divided by the number of core,
		  we always have a interger, so it is faster to brodcast the value and scatter the matrix
		  */
		MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

		std::size_t local_size = nrows / nb_proc;
		std::vector<double> result(nrows);

		MPI_Bcast(&local_size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

		{
			Timer::Sentry sentry(timer, "DenseMV");

			// SEND MATRIX - if not nrows/nb_proc is not an integer
			/*for (int i=1; i<nb_proc;++i)
			{
				if(i == (nb_proc -1)) {
					local_size = nrows - (nrows/nb_proc)*(nb_proc-1);
				}
			  std::size_t i_start = i * local_size * nrows;
			  double const* matrix_tosend = matrix.data() + i_start;
			  // SEND LOCAL SIZE to PROC I
			  MPI_Send(&local_size,1,MPI_UNSIGNED_LONG,i,0,MPI_COMM_WORLD);
			  // SEND MATRIX DATA
			  MPI_Send(matrix_tosend, local_size*nrows, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
			}*/

			//scatter the matrix
			std::vector<double> local_m(local_size * nrows);

			MPI_Scatter(matrix.data(), local_size * nrows, MPI_DOUBLE, local_m.data(), local_size * nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

			// BROAD CAST VECTOR X

			MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

			// COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
			DenseMatrix local_matrix;
			//std::size_t local_nrows ;

			// EXTRACT LOCAL DATA FROM MASTER PROC
			local_matrix.init(local_size, nrows);

			for (std::size_t i = 0; i < local_size; i++) {
				for (std::size_t j = 0; j < nrows; j++) {
					local_matrix(i, j) = local_m[i * nrows + j];
				}
			}

			std::vector<double> local_y(local_size);
			// compute parallel SPMV

			local_matrix.mult(x, local_y);

			//receive the y of each thread - if not nrows/nb_proc is not an integer
			 /*for (std::size_t i = 0; i<local_size;i++){
				 result[i] = local_y[i];
			 }
			 for (std::size_t i = 1; i<nb_proc;i++){
				 MPI_Status status;
				 if(i == (nb_proc -1)) {
					  local_size = nrows - (nrows/nb_proc)*(nb_proc-1);
				  }
				 MPI_Recv(local_y.data(),local_size,MPI_DOUBLE,i,1,MPI_COMM_WORLD, &status);
				 for (std::size_t j = 0; j<local_size; j++) {
					 result[i*local_size + j] = local_y[j];
				 }
			 }*/

			MPI_Gather(&local_y[0], local_size, MPI_DOUBLE, result.data(), local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		}

		double normy2 = PPTP::norm2(result);
		std::cout << "||y_mpi||=" << normy2 << std::endl;

		//save the time and speedup in a file
		double time_seq = timer_seq.d_print_time();
		double time_mpi = timer.d_print_time();
		std::ofstream fs;
		fs.open("time_dense.csv", std::ios_base::app);
		fs << nb_proc << ";" << nx << ";" << time_seq << ";" << time_mpi << ";" << time_seq / time_mpi << "\n";
		fs.close();
		//timer_seq.print_time();
		//timer.print_time();
	}
	else
	{
		// COMPUTE LOCAL MATRICE LOCAL VECTOR
		DenseMatrix local_matrix_ext;
		std::size_t nrows;
		std::size_t local_nrows;

		MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

		/*MPI_Status status;
		MPI_Recv(&local_nrows,1,MPI_UNSIGNED_LONG,0,0,MPI_COMM_WORLD, &status);*/
		MPI_Bcast(&local_nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

		local_matrix_ext.init(local_nrows, nrows);
		std::vector<double> local_coeff(nrows * local_nrows);
		//receive the local matrix
		//MPI_Recv(local_coeff.data(), nrows*local_nrows, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Scatter(NULL, NULL, MPI_DOUBLE, local_coeff.data(), local_nrows * nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		for (std::size_t i = 0; i < local_nrows; i++) {
			for (std::size_t j = 0; j < nrows; j++) {
				local_matrix_ext(i, j) = local_coeff[i * nrows + j];
			}
		}

		std::vector<double> x(nrows);
		// BROAD CAST VECTOR X
		MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		std::vector<double> local_y(local_nrows);
		// compute parallel SPMV
		local_matrix_ext.mult(x, local_y);

		//send the local result to the rank 0
		//MPI_Send(local_y.data(), local_nrows, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		MPI_Gather(local_y.data(), local_nrows, MPI_DOUBLE, NULL, 0, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	}
	timer.printInfo();
	timer_seq.printInfo();
	timer_mat.printInfo();
	MPI_Finalize();
	return 0;
}