#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include "omp.h"
#include "mpi.h"
#include "tbb/tbb.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/LU>

#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"

#include "Utils/Timer.h"
/*
SpMV using MPI

1. Initialize Matrix and Vector X on 0th rank
2. Scatter Matrix values and Vector values to different ranks
3. Compute spmv on all ranks
4. 0th rank should wait for others to finish
5. Gather all results on 
*/

int main(int argc, char** argv) {
	
	using namespace boost::program_options ;
	options_description desc;
	desc.add_options()
		("help", "produce help")
		("nrows",value<int>()->default_value(0), "matrix size")
		("nx",value<int>()->default_value(0), "nx grid size");
	
	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);

	if (vm.count("help")) {
	  std::cout << desc << "\n";
	  return 1;
	}
	
	int* cols;
	int* ptrs;
	double* data;

    MPI_Init( & argc, & argv);

    int my_rank = 0;
    int nb_proc = 1;
    
	MPI_Comm_size(MPI_COMM_WORLD, & nb_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, & my_rank);
	
	using namespace PPTP ;

//================COMMON VARS==================	
	std::size_t nrows;
	int local_nptrs;
	std::size_t local_nnz; 
	CSRMatrix local_matrix;
	CSRMatrix matrix; // only for rank 0 to generate data
	std::vector<double> local_data, x, local_y, y;
	std::vector<int> local_ptrs, local_cols;

	int* displs_1 = new int[nb_proc];
	int* scounts_1 = new int[nb_proc];
	int* displs_2 = new int[nb_proc];
	int* scounts_2 = new int[nb_proc];

	Timer timer;
	double mpi_st, mpi_end;
//================COMMON VARS==================	
	if (my_rank == 0) {

			MatrixGenerator generator ;
			// Generate data
			int nx = vm["nx"].as<int>() ;
			generator.genLaplacian(nx, matrix) ;
			nrows = matrix.nrows();
			
			x.resize(nrows);
			y.resize(nrows);
			for(std::size_t i=0;i<nrows;++i)
				x[i] = i + 1;

			{
			  Timer::Sentry sentry(timer,"time spmv") ;
			  matrix.mult(x, y) ;
			}
			double normy = PPTP::norm2(y);
			std::cout<<"||seq_y||="<<normy<<std::endl;

			mpi_st = MPI_Wtime();	
			// broadcast vector X and its size	
			MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
			MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

			
			data = matrix.data();
			ptrs = matrix.rowptrs();
			cols = matrix.cols();
			std::size_t nnz = matrix.nnz();
			
			// p1 and p2 are helping with load balancing aspect for nnz	
			int p1 = 0, p2 = p1 + 1, ulim_local_nnz = (int) nnz / nb_proc;

			// find displacements and strides for each rank and send the count of row pointers that are selected
			for (int i=1; i<nb_proc; ++i) {

				while ((ptrs[p2] - ptrs[p1]) < ulim_local_nnz)
					p2++;

				displs_1[i] = p1;
				scounts_1[i] = p2 - p1;
				std::size_t l_nnz = ptrs[p2 - 1] - ptrs[p1];
				displs_2[i] = ptrs[p1];
				scounts_2[i] = l_nnz;
				p1 = p2 - 1;
				MPI_Send(&scounts_1[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
			// assign remaining work to the rank 0 itself
			displs_1[0] = p1;
			scounts_1[0] =  (nrows + 1) - p1;
			displs_2[0] = ptrs[p1];
			scounts_2[0] = nnz - ptrs[p1];
			local_nptrs = scounts_1[0];
			local_nnz = scounts_2[0];

	}
	else {
			// receive broadcasted vector X and its size
			MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
			x.resize(nrows);	
			MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			// receive the local number of offests or row pointers
			MPI_Recv(&local_nptrs, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	}
	
	// scatter CSR to all ranks
	local_ptrs.resize(local_nptrs);
	MPI_Scatterv(ptrs, scounts_1, displs_1, MPI_INT, local_ptrs.data(), local_nptrs, MPI_INT, 0, MPI_COMM_WORLD);
	
	// compute local_nnz
 	local_nnz = local_ptrs[local_nptrs - 1] - local_ptrs[0];
	local_cols.resize(local_nnz);
	local_data.resize(local_nnz);
	// scatter column indices and data to all ranks
	MPI_Scatterv(cols, scounts_2, displs_2, MPI_INT, local_cols.data(), local_nnz, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatterv(data, scounts_2, displs_2, MPI_DOUBLE, local_data.data(), local_nnz, MPI_DOUBLE, 0, MPI_COMM_WORLD);


//===================LOCAL SPMV=====================

	local_matrix.initLocal(local_nptrs-1, local_nnz, local_ptrs, local_cols, local_data);
	local_y.resize(local_nptrs-1);
	std::stringstream ss;
	ss << "time spmv (rank " << my_rank << ") ";
	{
		Timer::Sentry sentry(timer, ss.str()) ;
		local_matrix.mult(x, local_y);
	}
	timer.printInfo();
//===================LOCAL SPMV=====================
	
	// gather the results back	
	MPI_Gatherv(local_y.data(), local_y.size(), MPI_DOUBLE, y.data(), scounts_1, displs_1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	if (my_rank == 0) {
		mpi_end = MPI_Wtime();
		double normy = PPTP::norm2(y);
		std::cout<<"||mpi_y||=" << normy << std::endl;
	}
	
	// finalize
	delete[] displs_1;
	delete[] scounts_1;
	delete[] displs_2;
	delete[] scounts_2;
	
	MPI_Finalize();
	
	if (my_rank == 0) {
		timer.printInfo();
		std::cout << "time spmv mpi: " << (mpi_end - mpi_st) << std::endl;
	}
	return 0;
}	
