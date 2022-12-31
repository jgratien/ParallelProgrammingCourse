/*
 * helloworld.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: gratienj
 */

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <mpi.h>
#include <string>
#include <vector>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/LU>
#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"

#include "Utils/Timer.h"
#define min(a,b) (((a)<(b))?(a):(b))


void generate_offsets (int nrows, int nprocs, int* kcol, int* counts,int* starts,int* row_starts,int* row_counts)
{
	int res = nrows%nprocs; 
	int div = nrows/nprocs;
	int start=0;
	for (int i=0;i<nprocs; i++)
	{
		starts[i]=kcol[start];
		counts[i]=kcol[start+div+min(res,1)]-kcol[start];
		row_starts[i] = start;
		start+=div+min(res,1);
		row_counts[i] = div + min(res, 1)+1;
		if (res !=0)
			res-=1;
	}
}

int main(int argc, char** argv)
{
	using namespace boost::program_options;
	options_description desc;
	desc.add_options()
		("help", "produce help")
		("nb-threads", value<int>()->default_value(0), "nb threads")
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

	Timer timer;
	Timer timers[nb_proc];
	Timer timers_nc[nb_proc];
	Timer timer2;
	MatrixGenerator generator;
	if (vm["eigen"].as<int>() == 1)
	{
		typedef Eigen::SparseMatrix<double> MatrixType;
		typedef Eigen::VectorXd             VectorType;
		MatrixType matrix;
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


		std::size_t nrows = matrix.rows();
		VectorType x(nrows);

		for (std::size_t i = 0; i < nrows; ++i)
			x(i) = i + 1;

		VectorType y;
		{
			 Timer::Sentry sentry(timer,"EigenSpMV") ;
			y = matrix*x;
		}
		timer.printInfo();

		double normy = PPTP::norm2(y);
		std::cout << "||y||=" << normy << std::endl;
	}

	//	variables 
	std::size_t nrows;
	std::vector<double> x, y, y2,y3;

	int counts[nb_proc];
	int starts[nb_proc];
	int row_counts[nb_proc];
	int row_starts[nb_proc];
	CSRMatrix matrix;
	// initialisation of the matrix and computation of sequential spmv
	if (my_rank == 0)
	{
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
		
		nrows = matrix.nrows();
		x.resize(nrows);
		y.resize(nrows);
		y2.resize(nrows);
		y3.resize(nrows);

		for (std::size_t i = 0; i < nrows; ++i)
			x[i] = i + 1;
		{
			Timer::Sentry sentry(timer2,"SpMV") ;
			matrix.mult(x, y);
		}
		double normy = PPTP::norm2(y);
		timer2.printInfo();

		std::cout << "||y||=" << normy << std::endl;

		generate_offsets(nrows, nb_proc, matrix.get_kcol(), counts, starts, row_starts, row_counts);

	}
	//collective  operations 
	{
		Timer::Sentry sentry(timers[my_rank], "MPI_SpMV");

		MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
		if (my_rank != 0)
		{
			x.resize(nrows);
		}
		MPI_Bcast(&x[0], nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&counts, nb_proc, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
		MPI_Bcast(&starts, nb_proc, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
		MPI_Bcast(&row_counts, nb_proc, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
		MPI_Bcast(&row_starts, nb_proc, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
		std::vector<int> local_k_col(row_counts[my_rank]);
		std::vector<int> local_m_cols(counts[my_rank]);
		std::vector<double> local_values(counts[my_rank]);
		MPI_Scatterv(matrix.get_kcol(), row_counts, row_starts, MPI_INT, local_k_col.data(), row_counts[my_rank], MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Scatterv(matrix.get_cols(), counts, starts, MPI_INT, local_m_cols.data(), counts[my_rank], MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Scatterv(matrix.get_values(), counts, starts, MPI_DOUBLE, local_values.data(), counts[my_rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
		{
			Timer::Sentry sentry(timers_nc[my_rank], "MPI_SpMV without communication time");

			int s = local_k_col[0];

			for (int i = 0; i < row_counts[my_rank]; i++)
			{
				local_k_col[i] -= s;
			}
			CSRMatrix local_matrix;
			y2.resize(row_counts[my_rank] - 1);
			local_matrix.setFromvectors(row_counts[my_rank] - 1, local_k_col, local_m_cols, local_values);

			local_matrix.mult(x, y2);


			for (int i = 0; i < nb_proc; i++)
				row_counts[i] -= 1;
		}
		MPI_Gatherv(y2.data(), row_counts[my_rank], MPI_DOUBLE, y3.data(), row_counts, row_starts, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		

	}
	// print results 
	if (my_rank == 0)
	{
		timers[0].printInfo();
		timers_nc[0].printInfo();
		double normy2 = PPTP::norm2(y3);
		std::cout << "||y3||=" << normy2 << std::endl;
	}
	MPI_Finalize() ;

  return 0 ;
}
