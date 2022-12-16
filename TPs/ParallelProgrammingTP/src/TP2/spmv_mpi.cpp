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
 * I have removed some parts of the code that were not being used for this exercise
 * 
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
	std::stringstream ss;

	MPI_Request req1, req2, req3, req4, req5;

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
//===================INIT======================
			MatrixGenerator generator ;
			// Generate data
			int nx = vm["nx"].as<int>() ;
			generator.genLaplacian(nx, matrix) ;
			nrows = matrix.nrows();
			data = matrix.data();
			ptrs = matrix.rowptrs();
			cols = matrix.cols();
			std::size_t nnz = matrix.nnz();

			x.resize(nrows);
			y.resize(nrows);

			for(std::size_t i=0;i<nrows;++i)
				x[i] = i + 1;
//===================INIT======================

//===================SEQ=======================
			{
			  Timer::Sentry sentry(timer,"SparseMV") ;
			  matrix.mult(x, y) ;
			}
			double normy = PPTP::norm2(y);
			std::cout<<"||seq_y||="<<normy<<std::endl;
			timer.printInfo();
//===================SEQ=======================

//===========MPI START but even rank 0 starts the clock at the broadcast on line 140============================			
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
				MPI_Isend(&scounts_1[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &req3);
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
			// receive the local number of offests or row pointers
			MPI_Irecv(&local_nptrs, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &req3);
	}

//============================================================ALL RANKS==============================================================
	MPI_Barrier(MPI_COMM_WORLD);
	mpi_st = MPI_Wtime();
	// broadcast vector X and its size
	MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
	x.resize(nrows);
	MPI_Ibcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD, &req2);
	// scatter CSR to all ranks
	if (my_rank != 0)
		MPI_Wait(&req3, MPI_STATUS_IGNORE);
	local_ptrs.resize(local_nptrs);
	
	MPI_Scatterv(ptrs, scounts_1, displs_1, MPI_INT, local_ptrs.data(), local_nptrs, MPI_INT, 0, MPI_COMM_WORLD);
	
	// compute local_nnz
 	local_nnz = local_ptrs[local_nptrs - 1] - local_ptrs[0];
	// scatter column indices and data to all ranks
	// Note that I am using a non-blocking calls below to resize the vectors while data is being received.
	local_cols.resize(local_nnz);
	MPI_Iscatterv(cols, scounts_2, displs_2, MPI_INT, local_cols.data(), local_nnz, MPI_INT, 0, MPI_COMM_WORLD, &req4);
	local_data.resize(local_nnz);
	MPI_Iscatterv(data, scounts_2, displs_2, MPI_DOUBLE, local_data.data(), local_nnz, MPI_DOUBLE, 0, MPI_COMM_WORLD, &req5);
//===================LOCAL SPMV=====================	
	MPI_Wait(&req4, MPI_STATUS_IGNORE);
	MPI_Wait(&req5, MPI_STATUS_IGNORE);
	// I created a new function called initLocal() for applying the move semantics to the data without copying it
	local_matrix.initLocal(local_nptrs-1, local_nnz, std::move(local_ptrs), std::move(local_cols), std::move(local_data));
	local_y.resize(local_nptrs-1);
	if (my_rank != 0)
		MPI_Wait(&req2, MPI_STATUS_IGNORE);

	local_matrix.mult(x, local_y);

//===================LOCAL SPMV=====================
	
	// gather the results back
	MPI_Gatherv(local_y.data(), local_nptrs-1, MPI_DOUBLE, y.data(), scounts_1, displs_1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	// I am calling MPI_Barrier as a sanity check even though Gatherv above is a blocking call to all MPI ranks.
	// still Rank 0 does not obtain the Maximum elapsed time.
	MPI_Barrier(MPI_COMM_WORLD);
	mpi_end = MPI_Wtime();
//============================================================ALL RANKS==============================================================	
	if (my_rank == 0) {
		
		double normy = PPTP::norm2(y);
		std::cout<<"||mpi_y||=" << normy << std::endl;
		std::cout << "SparseMVMPI:" << (mpi_end - mpi_st) << std::endl;
	}
	
	// finalize
	delete[] displs_1;
	delete[] scounts_1;
	delete[] displs_2;
	delete[] scounts_2;

	MPI_Finalize();
	
	return 0;
}	
