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

/*
 * I have removed some parts of the code that were not being used for this exercise
 * 
 */
int main(int argc, char ** argv) {

		using namespace boost::program_options;
		options_description desc;
		desc.add_options()
			("help", "produce help")
			("nx", value < int > () -> default_value(0), "nx grid size");
		variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);
		notify(vm);

		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 1;
		}
		MPI_Init( & argc, & argv);

		int my_rank = 0;
		int nb_proc = 1;

		MPI_Request req1, req2;

		MPI_Comm_size(MPI_COMM_WORLD, & nb_proc);
		MPI_Comm_rank(MPI_COMM_WORLD, & my_rank);

		using namespace PPTP;

		Timer timer;
		MatrixGenerator generator;
		DenseMatrix matrix, local_matrix;
		std::size_t nrows, local_nrows;
		std::vector < double > x, y, local_y;

		if (my_rank == 0) {

				int nx = vm["nx"].as < int > ();
				matrix.init(nrows, nrows);
				generator.genLaplacian(nx, matrix);

				nrows = matrix.nrows();
				x.resize(nrows);
				y.resize(nrows);
				for (std::size_t i = 0; i < nrows; ++i)
					x[i] = i + 1;

				Timer::Sentry sentry(timer, "DenseMVMPI");
				local_nrows = nrows / nb_proc;
				int modulo = nrows % nb_proc;
				int count = local_nrows * nrows;
				std::size_t bcast_arr[2] = {nrows, local_nrows};
				MPI_Bcast(bcast_arr, 2, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
				MPI_Ibcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD, &req1);
					
				local_matrix.init(modulo + local_nrows, nrows);
				MPI_Iscatter(matrix.data() + (modulo * nrows),
								count, MPI_DOUBLE, local_matrix.data() + (modulo * nrows),
								count, MPI_DOUBLE, 0, MPI_COMM_WORLD, &req2);

				local_y.resize(modulo + local_nrows);
				MPI_Wait(&req2, MPI_STATUS_IGNORE);
				for (int i=0; i<modulo; ++i)
					for(int j=0; j<nrows; ++j)
						local_matrix(i, j) = matrix(i, j);
				
				local_matrix.mult(x, local_y);
				MPI_Gather(local_y.data() + modulo, local_nrows, MPI_DOUBLE,
							y.data() + modulo, local_nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

				for (int i=0; i<modulo; ++i)
						y[i] = local_y[i];

				double normy = PPTP::norm2(y);
				std::cout << "||y2||=" << normy << std::endl;
		}
		else {
				std::size_t bcast_arr[2];
				MPI_Bcast(bcast_arr, 2, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
				nrows = bcast_arr[0];
				local_nrows = bcast_arr[1];
				int count = local_nrows * nrows;
				x.resize(nrows);
				MPI_Ibcast(&x[0], nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD, &req1);

				local_matrix.init(local_nrows, nrows);
				MPI_Iscatter(matrix.data(), count, MPI_DOUBLE, local_matrix.data(),
									count, MPI_DOUBLE, 0, MPI_COMM_WORLD, &req2);

		
				local_y.resize(local_nrows);
				MPI_Wait(&req1, MPI_STATUS_IGNORE);
				MPI_Wait(&req2, MPI_STATUS_IGNORE);
#if 0
				std::cout << "\nmatrix: " << local_matrix.nrows() << " " << local_matrix.ncols() << std::endl;
				for (int i=0; i<local_nrows; ++i) {
					for (int j=0; j<nrows; ++j) {
							std::cout << local_matrix(i, j) <<"\t";
					}
					std::cout << std::endl;
				}
				std::cout << std::endl << std::endl;
				for (int j=0; j<nrows; ++j) {
					std::cout << local_y[j] <<"\t";
				}
				std::cout << std::endl << std::endl;
#endif
				local_matrix.mult(x, local_y);

				MPI_Gather(local_y.data(), local_nrows, MPI_DOUBLE,
							y.data(), local_nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		}



		if (my_rank == 0) {

				// y.assign(nrows, 0);
				{
					Timer::Sentry sentry(timer, "DenseMV");
					matrix.mult(x, y);
				}
				double normy = PPTP::norm2(y);
				std::cout << "||y||=" << normy << std::endl;
				timer.printInfo();
		}

		MPI_Finalize();
		return 0;
}
