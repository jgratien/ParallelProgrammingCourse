/*
 * helloworld.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: gratienj
 */

#include <mpi.h>
#include <iostream>
#include <fstream>
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

	Timer timer_seq, timer;
	MatrixGenerator generator;
	int nx = vm["nx"].as<int>();
	if (vm["eigen"].as<int>() == 1)
	{
		typedef Eigen::SparseMatrix<double> MatrixType;
		typedef Eigen::VectorXd             VectorType;
		MatrixType matrix;

		std::size_t nrows = nx * nx;

		generator.genLaplacian(nx, matrix);

		VectorType x(nrows);

		for (std::size_t i = 0; i < nrows; ++i)
			x(i) = i + 1;

		VectorType y;
		double normy = PPTP::norm2(y);
		std::cout << "||y||=" << normy << std::endl;
	}

	if (my_rank == 0)
	{
		CSRMatrix matrix;

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
		std::vector<double> x, y;
		x.resize(nrows);
		y.resize(nrows);

		for (std::size_t i = 0; i < nrows; ++i) {
			x[i] = i + 1;
		}
		// compute the sequential sparse matrix
		{
			Timer::Sentry sentry(timer_seq, "Seq SPMV");
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
		MPI_Bcast(&local_size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

		std::vector<int> matrix_row = matrix.kcol();
		std::vector<int> matrix_col = matrix.col();
		std::vector<double> matrix_val = matrix.val();

		std::vector<double> result(nrows);

		{
			Timer::Sentry sentry(timer, "MPI SPMV");
			// SEND MATRIX
			for (int i = 1; i < nb_proc; ++i)
			{
				/*if(i == (nb_proc -1)) {
					local_size = nrows - (nrows/nb_proc)*(nb_proc-1);
				}*/
				std::size_t i_start = i * local_size;
				std::size_t i_end = (i + 1) * local_size;
				std::size_t i_element = matrix_row[i_start];
				int nb_element = matrix_row[i_end] - matrix_row[i_start];
				MPI_Send(&matrix_row[i_start], local_size + 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&matrix_col[i_element], nb_element, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&matrix_val[i_element], nb_element, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
			}


			// BROAD CAST VECTOR X
			MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

			CSRMatrix local_matrix;
			// EXTRACT LOCAL MATRIX DATA
			std::vector<int> matrix_row_local(local_size + 1);
			for (std::size_t i = 0; i < local_size + 1; i++) {
				matrix_row_local[i] = matrix_row[i];
			}
			int nb_element_local = matrix_row_local[local_size] - matrix_row_local[0];
			std::vector<int> matrix_col_local(nb_element_local);
			std::vector<double> matrix_val_local(nb_element_local);
			for (std::size_t i = 0; i < nb_element_local; i++) {
				matrix_col_local[i] = matrix_col[i];
				matrix_val_local[i] = matrix_val[i];
			}
			local_matrix.set_value(matrix_row_local, matrix_col_local, matrix_val_local, nrows);

			std::vector<double> local_y(local_size);

			// compute parallel SPMV
			local_matrix.mult2(x, local_y, local_size);

			//reveive the result of all threads
			/*for (std::size_t i = 0; i<local_size;i++){
				result[i] = local_y[i];
			}
			for (std::size_t i = 1; i<nb_proc;i++){
				if(i == (nb_proc -1)) {
					 local_size = nrows - (nrows/nb_proc)*(nb_proc-1);
				 }
				MPI_Status status;
				MPI_Recv(local_y.data(),local_size,MPI_DOUBLE,i,1,MPI_COMM_WORLD, &status);
				for (std::size_t j = 0; j<local_size; j++) {
					result[i*local_size + j] = local_y[j];
				}
			}*/
			MPI_Gather(&local_y[0], local_size, MPI_DOUBLE, result.data(), local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		}

		double normy2 = PPTP::norm2(result);
		std::cout << "||y_mpi||=" << normy2 << std::endl;

		//save the time in a file
		double time_seq = timer_seq.d_print_time();
		double time_mpi = timer.d_print_time();
		std::ofstream fs;
		fs.open("time_spmv.csv", std::ios_base::app);
		fs << nb_proc << ";" << nx << ";" << time_seq << ";" << time_mpi << ";" << time_seq / time_mpi << "\n";
		fs.close();
		timer_seq.print_time();
		timer.print_time();
	}
	else
	{
		// COMPUTE LOCAL MATRICE LOCAL VECTOR
		CSRMatrix local_matrix_ext;
		std::size_t nrows;
		std::size_t local_nrows;
		// RECV DATA FROM MASTER PROC
		// RECV GLOBAL SIZE
		MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
		// RECV LOCAL SIZE
		MPI_Bcast(&local_nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

		// RECV MATRIX DATA
		std::vector<int> local_row(local_nrows + 1);
		MPI_Status status;
		MPI_Recv(local_row.data(), local_nrows + 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		int nb_element_local = local_row[local_nrows] - local_row[0];
		std::vector<int> local_col(nb_element_local);
		std::vector<double> local_val(nb_element_local);
		MPI_Recv(local_col.data(), nb_element_local, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(local_val.data(), nb_element_local, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

		local_matrix_ext.set_value(local_row, local_col, local_val, nrows);
		std::vector<double> x(nrows);
		// BROAD CAST VECTOR X
		MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		std::vector<double> local_y(local_nrows);

		// compute parallel SPMV
		local_matrix_ext.mult2(x, local_y, local_nrows);

		//Send the result
		//MPI_Send(local_y.data(), local_nrows, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		MPI_Gather(local_y.data(), local_nrows, MPI_DOUBLE, NULL, 0, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}
	timer.printInfo();
	MPI_Finalize();
	return 0;
}