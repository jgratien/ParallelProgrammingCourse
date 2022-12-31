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
#define min(a,b) (((a)<(b))?(a):(b))


void generate_offsets(size_t nrows, int nprocs, size_t* counts, size_t* starts)
{
	size_t res = nrows%nprocs;
	size_t div = nrows / nprocs;
	size_t start = 0;
	for (int i = 0; i<nprocs; i++)
	{
		starts[i] = start*nrows;
		counts[i] = (div + min(res, 1));
		if (res != 0)
			res -= 1;
		start += counts[i];
	}
}


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
	Timer timer, timer2;
	double normy2;
	MatrixGenerator generator;
	int nx = vm["nx"].as<int>();
	if (vm["eigen"].as<int>() == 1)
	{
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType;
		typedef Eigen::Matrix<double, Eigen::Dynamic, 1>              EigenVectorType;
		std::size_t nrows = nx*nx;
		EigenMatrixType matrix(nrows, nrows);
		generator.genLaplacian(nx, matrix);
		EigenVectorType x(nrows);
		for (std::size_t i = 0; i<nrows; ++i)
			x(i) = i + 1;
		EigenVectorType y;
		{
			Timer::Sentry sentry(timer, "EigenDenseMV");
			y = matrix*x;
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
		std::vector<double> x,y2;
		x.resize(nrows);
		
		size_t counts[nb_proc];
		size_t starts[nb_proc];
		generate_offsets(nrows, nb_proc, counts, starts);

		for (std::size_t i = 0; i<nrows; ++i)
			x[i] = i + 1;

		{
			std::vector<double> y(nrows);
			{
				Timer::Sentry sentry(timer, "Seq DenseMV");
				matrix.mult(x, y);
			}
			timer.printInfo();
			double normy = PPTP::norm2(y);

			std::cout << "||y||=" << normy << std::endl;
		}
		{
			Timer::Sentry sentry(timer2, "MPI DenseMV");

			{
				MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
				// SEND MATRIX
				for (int i = 1; i < nb_proc; ++i)
				{

					// SEND LOCAL SIZE to PROC I		
					std::size_t local_nrows = nrows / nb_proc;
					MPI_Send(&counts[i], 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);
					// SEND MATRIX DATA
					std::size_t start = i * local_nrows*nrows;
					double const* matrix_ptr = matrix.data() + starts[i];
					MPI_Send(matrix_ptr, counts[i] * nrows, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
				}
			}
			{
				// BROAD CAST VECTOR X
				MPI_Bcast(&x[0], nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			}

			// COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
			DenseMatrix local_matrix;
			std::size_t local_nrows;
			{
				// EXTRACT LOCAL DATA FROM MASTER PROC

				// COMPUTE LOCAL SIZE
				local_nrows = counts[0];
				local_matrix.init(local_nrows, nrows);
				// EXTRACT LOCAL MATRIX DATA
				for (auto i = 0; i < local_nrows; i++)
				{
					for (auto j = 0; j < nrows; j++)
					{
						local_matrix(i, j) = matrix(i, j);
					}
				}
			}
			std::vector<double> local_y(local_nrows);
			y2.resize(nrows);
			{
				local_matrix.mult(x, local_y);
			}
			// SAVING LOCAL RESULTS TO THE GLOBAL Y
			for (auto i = 0; i < local_nrows; i++)
			{
				y2[i] = local_y[i];
			}

			for (auto i = 1; i < nb_proc; i++)
			{
				MPI_Status status;
				// RECV RESULTS FROM OTHER PROCS
				MPI_Recv(y2.data() + starts[i] / nrows, counts[i], MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
			}
		}
		timer2.printInfo();

		normy2 = PPTP::norm2(y2);

		std::cout << "||y2||=" << normy2 << std::endl;

	}
	else
	{
		// COMPUTE LOCAL MATRICE LOCAL VECTOR
		DenseMatrix local_matrix;
		std::size_t nrows;
		std::size_t local_nrows;
		{
			// RECV DATA FROM MASTER PROC
			MPI_Status status;
			// RECV GLOBAL SIZE
			MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
			// RECV LOCAL SIZE
			MPI_Recv(&local_nrows, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, &status);
			local_matrix.init(local_nrows, nrows);
			// RECV MATRIX DATA
			MPI_Recv(local_matrix.data(), local_nrows*nrows, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
		}
		std::vector<double> x1(nrows);
		{
			// RECV VECTOR X
			MPI_Bcast(&x1[0], nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		}

		std::vector<double> local_y(local_nrows);
		{
			local_matrix.mult(x1, local_y);
		}
		// SEND LOCAL RESULTS
		MPI_Send(local_y.data(), local_nrows, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}