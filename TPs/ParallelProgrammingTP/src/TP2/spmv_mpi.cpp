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
      std::cout << desc << "\n";
      return 1;
  }
  MPI_Init(&argc,&argv) ;

  int my_rank = 0 ;
  int nb_proc = 1 ;
  MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;

  using namespace PPTP ;

  Timer timer_seq, timer_mpi, timer_mpi_debug;
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
      // Timer::Sentry sentry(timer,"EigenDenseMV") ;
      y = matrix*x ;
    }

    double normy = PPTP::norm2(y) ;
    std::cout<<"||y||="<<normy<<std::endl ;

  }

  if(my_rank==0)
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

    std::vector<double> x;
    x.resize(nrows);

    for (std::size_t i = 0;i < nrows;++i)
        x[i] = i + 1;

    std::vector<double> y(nrows), z(nrows);
    {
      // SEQUENTIAL SPARSE MATRIX MULTIPLICATION
      Timer::Sentry sentry(timer_seq, "Sequential SparseMV");
      matrix.mult(x, y);
    }
    double normy = PPTP::norm2(y);
    std::cout << "||y_SEQ||=" << normy << std::endl;

    {
        // MPI SPARSE MATRIX MULTIPLICATION
        MPI_Status status; MPI_Request request_kcol, request_cols_matrix;
        std::vector<int> kcol = matrix.kcol(); std::vector<int> cols = matrix.cols(); std::vector<double> values = matrix.values(); 
	    int const* kcol_ptr = matrix.data_kcol(); int const* cols_ptr = matrix.data_cols(); double const* matrix_ptr = matrix.data();

        std::size_t q = (kcol.size() - 1) / nb_proc; std::size_t r = (kcol.size() - 1) % nb_proc;
        
	    std::vector<int> displs(nb_proc); std::vector<int> recvcounts(nb_proc);
	    std::vector<int> displs_kcol(nb_proc); std::vector<int> recvcounts_kcol(nb_proc);
	    std::vector<int> displs_cols_matrix(nb_proc); std::vector<int> recvcounts_cols_matrix(nb_proc);

	    displs[0] = 0; recvcounts[0] = q;
	    displs_kcol[0] = 0; recvcounts_kcol[0] = q+1;
	    displs_cols_matrix[0] = 0; recvcounts_cols_matrix[0] = kcol[q];

        for (int destination = 1; destination < nb_proc; ++destination)
        {
            // DISPLACEMENTS & COUNTS FOR EACH PROC
	        std::size_t local_nkcol = q; if (destination <= r) local_nkcol++;
            displs[destination] = displs[destination - 1] + recvcounts[destination - 1]; recvcounts[destination] = local_nkcol;
            displs_kcol[destination] = displs_kcol[destination-1] + recvcounts[destination-1]; recvcounts_kcol[destination] = local_nkcol+1;
            displs_cols_matrix[destination] = displs_cols_matrix[destination-1] + recvcounts_cols_matrix[destination-1]; recvcounts_cols_matrix[destination] = kcol[displs[destination]+local_nkcol] - kcol[displs[destination]];
	    }

        // BCAST GLOBAL & LOCAL SIZE
		MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
		MPI_Bcast(&q, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
		MPI_Bcast(&r, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

        // BCAST X
		MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        {
            Timer::Sentry sentry(timer_mpi, "MPI SparseMV");

            // SCATTER CSR MATRIX DATA
		    std::vector<int> local_kcol(recvcounts[my_rank]+1);
		    MPI_Scatterv(matrix.data_kcol(), recvcounts_kcol.data(), displs_kcol.data(), MPI_INT, local_kcol.data(), recvcounts[my_rank]+1, MPI_INT, 0, MPI_COMM_WORLD);
		    
		    std::vector<int> local_cols(local_kcol[recvcounts[my_rank]]-local_kcol[0]);
		    std::vector<double> local_values(local_kcol[recvcounts[my_rank]]-local_kcol[0]);
		    MPI_Iscatterv(matrix.data_cols(), recvcounts_cols_matrix.data(), displs_cols_matrix.data(), MPI_INT, local_cols.data(), local_kcol[recvcounts[my_rank]]-local_kcol[0], MPI_INT, 0, MPI_COMM_WORLD, &request_kcol);
		    MPI_Iscatterv(matrix.data(), recvcounts_cols_matrix.data(), displs_cols_matrix.data(), MPI_DOUBLE, local_values.data(), local_kcol[recvcounts[my_rank]]-local_kcol[0], MPI_DOUBLE, 0, MPI_COMM_WORLD, &request_cols_matrix);
            MPI_Wait(&request_kcol, &status); MPI_Wait(&request_cols_matrix, &status);

            // PROC 0
		    std::vector<double> local_y(recvcounts[my_rank]);    
            matrix.mult(x, local_y, my_rank, recvcounts[my_rank]);

            // GATHER
            MPI_Gatherv(local_y.data(), recvcounts[my_rank], MPI_DOUBLE, z.data(), recvcounts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        double normz = PPTP::norm2(z);
        std::cout << "||y_MPI||=" << normz << std::endl;
    }

    // TIME OUTPUT
    std::ofstream outfile;
    outfile.open("time.txt", std::ios_base::app);
    if (nb_proc == 1 && nx == 50)
    {
        outfile << "nx	" << "np	" << "Time_MPI	" << "Time_SEQ	" << "Speed_Up" << std::endl;
    }
    outfile << nx << "	" << nb_proc << "	" << timer_mpi.sum_time() << "	" << timer_seq.sum_time() << "	" << timer_seq.sum_time() / timer_mpi.sum_time() << std::endl;
  }
  else
  {
        // SLAVE PROC
        MPI_Status status; MPI_Request request_kcol, request_cols_matrix;
        std::size_t nrows, local_nkcol, r;
        int source=0;

        // RECEIVE GLOBAL & LOCAL SIZE
        MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
        MPI_Bcast(&local_nkcol, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
        MPI_Bcast(&r, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
        if(my_rank<=r) local_nkcol++;

        // RECEIVE X
        std::vector<double> x(nrows);
        MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // RECEIVE CSR MATRIX DATA
        std::vector<int> local_kcol(local_nkcol+1);
        MPI_Scatterv(NULL, NULL, NULL, MPI_INT, local_kcol.data(), local_nkcol+1, MPI_INT, 0, MPI_COMM_WORLD); 
        std::vector<int> local_cols(local_kcol[local_nkcol]-local_kcol[0]);
        std::vector<double> local_values(local_kcol[local_nkcol]-local_kcol[0]);
        MPI_Iscatterv(NULL, NULL, NULL, MPI_INT, local_cols.data(), local_kcol[local_nkcol]-local_kcol[0], MPI_INT, 0, MPI_COMM_WORLD, &request_kcol);
        MPI_Iscatterv(NULL, NULL, NULL, MPI_DOUBLE, local_values.data(), local_kcol[local_nkcol]-local_kcol[0], MPI_DOUBLE, 0, MPI_COMM_WORLD, &request_cols_matrix);
        MPI_Wait(&request_kcol, &status); MPI_Wait(&request_cols_matrix, &status);

        // COMPUTE LOCAL PRODUCT
        std::vector<double> local_y(local_nkcol);
        mult(x, local_y, local_nkcol, local_kcol, local_cols, local_values);

        // GATHER DATA
        MPI_Gatherv(local_y.data(), local_nkcol, MPI_DOUBLE, NULL, NULL, NULL, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  }
  
  MPI_Finalize() ;
   
  return 0 ;
}