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

// step 1 : include mpi
#include <mpi.h>
// --------------------

#include <string>
#include <vector>
#include <fstream>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/LU>
#include <numeric>


#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"

#include "Utils/Timer.h"

void scatterCSRMatrix(
    const PPTP::CSRData& full_data, 
    PPTP::CSRData& local_data, 
    std::vector<int>& row_counts, std::vector<int>& row_displs,
    int rank, int size, 
    MPI_Comm comm
) {
    std::size_t local_nrows;
    std::vector<int> nnz_counts(size, 0);

    int total_size = 3 * size; // 3 arrays with 'size' elements each
    std::vector<int> combined_buffer(total_size);
    if (rank == 0) {
      // Partition rows
      local_nrows = full_data.nrows / size;
      std::size_t remainder = full_data.nrows % size;

      std::fill(row_counts.begin(), row_counts.end(), local_nrows);
      for (int i = 0; i < remainder; ++i) row_counts[i]++;
      std::partial_sum(row_counts.begin(), row_counts.end(), row_displs.begin() + 1);
    
      for (int i = 0; i < size; ++i) {
        nnz_counts[i] = full_data.kcol[row_displs[i] + row_counts[i]] - full_data.kcol[row_displs[i]];
      }

      std::copy(row_counts.begin(), row_counts.end(), combined_buffer.begin());
      std::copy(row_displs.begin(), row_displs.end(), combined_buffer.begin() + size);
      std::copy(nnz_counts.begin(), nnz_counts.end(), combined_buffer.begin() + 2 * size);
    }

    // Broadcast counts and displacements
    MPI_Bcast(combined_buffer.data(), total_size, MPI_INT, 0, comm);

    if (rank != 0) {
      row_counts.assign(combined_buffer.begin(), combined_buffer.begin() + size);
      row_displs.assign(combined_buffer.begin() + size, combined_buffer.begin() + 2 * size);
      nnz_counts.assign(combined_buffer.begin() + 2 * size, combined_buffer.end());
    }


    // Prepare local data
    local_data.nrows = row_counts[rank];
    local_data.kcol.resize(local_data.nrows + 1);
    
    MPI_Scatterv(
        full_data.kcol.data(), 
        row_counts.data(), 
        row_displs.data(), 
        MPI_INT, 
        local_data.kcol.data(), 
        row_counts[rank] + 1,  
        MPI_INT, 
        0, comm
    );

    if (!local_data.kcol.empty()) {
        int initial = local_data.kcol[0];
        for (int& k : local_data.kcol) k -= initial;
        local_data.kcol[local_data.kcol.size() - 1] = nnz_counts[rank];
    }

    std::vector<int> nnz_displs(size, 0);
    std::partial_sum(nnz_counts.begin(), nnz_counts.end() - 1, nnz_displs.begin() + 1);

    // Scatter columns and values
    local_data.cols.resize(nnz_counts[rank]);
    local_data.values.resize(nnz_counts[rank]);

    MPI_Scatterv(
        full_data.cols.data(), 
        nnz_counts.data(), 
        nnz_displs.data(), 
        MPI_INT, 
        local_data.cols.data(), 
        nnz_counts[rank], 
        MPI_INT, 
        0, comm
    );

    MPI_Scatterv(
        full_data.values.data(), 
        nnz_counts.data(), 
        nnz_displs.data(), 
        MPI_DOUBLE, 
        local_data.values.data(), 
        nnz_counts[rank], 
        MPI_DOUBLE, 
        0, comm
    );

    // Update nnz count
    local_data.nnz = nnz_counts[rank];
}


int main(int argc, char** argv)
{
  using namespace boost::program_options ;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
      ("nb-threads",value<int>()->default_value(0), "nb threads")
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

  MPI_Init(&argc, &argv); 

  int world_size;
  int world_rank;

  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  using namespace PPTP ;

  Timer timer ;
  MatrixGenerator generator ;
  if(vm["eigen"].as<int>()==1)
  {
    typedef Eigen::SparseMatrix<double> MatrixType ;
    typedef Eigen::VectorXd             VectorType ;
    MatrixType matrix ;
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


    std::size_t nrows = matrix.rows();
    VectorType x(nrows);

    for(std::size_t i=0;i<nrows;++i)
      x(i) = i+1 ;

    VectorType y ;
    {
      Timer::Sentry sentry(timer,"EigenSpMV") ;
       y = matrix*x ;
    }

    double normy = PPTP::norm2(y) ;
    std::cout<<"||y||="<<normy<<std::endl ;
  }
  else
  {

    CSRMatrix local_matrix;
    CSRData full_data, local_data;
    std::size_t global_nrows;
    std::vector<double> x;

    if(world_rank == 0)
    {
      CSRMatrix matrix;
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


      global_nrows = matrix.nrows();
      std::vector<double> y;
      x.resize(global_nrows);
      y.resize(global_nrows);

      for(std::size_t i=0;i<global_nrows;++i)
        x[i] = i+1 ;

      {
        Timer::Sentry sentry(timer,"SpMV") ;
        matrix.mult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"||y||="<<normy<<std::endl ;
      
      full_data = matrix.data();

    }

    
    std::vector<double> y;
    {
      Timer::Sentry sentry(timer,"SpMV " + std::to_string(world_rank)) ;
      // gloal_matrix_size
      MPI_Bcast(&global_nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

      // vector x
      x.resize(global_nrows);
      MPI_Bcast(x.data(), global_nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

      std::vector<int> row_counts(world_size, 0), row_displs(world_size, 0);

      scatterCSRMatrix(full_data, local_data,row_counts,
                    row_displs, world_rank, world_size, MPI_COMM_WORLD);

      local_matrix.copyCSRMatrixFromCSRData(local_data);
      
      // Step 7 : Computing the local multiplication
      std::vector<double> local_y(local_matrix.nrows());
      {
        local_matrix.mult(x,local_y);
      }    

      // Gather the results back to process 0
      if (world_rank == 0) {
          y.resize(full_data.nrows);  // Resize on rank 0 to hold the entire result
      } 

      MPI_Gatherv(
          local_y.data(),             // Local buffer
          local_y.size(),             // Number of elements to send
          MPI_DOUBLE,                 // Data type
          y.data(),                   // Global buffer (on rank 0)
          row_counts.data(),          // Counts of rows per process
          row_displs.data(),          // Displacements
          MPI_DOUBLE,                 // Data type
          0,                          // Root process
          MPI_COMM_WORLD                       // Communicator
      );
    }

    if (world_rank == 0)
    {
      double normy2 = PPTP::norm2(y);
      std::cout<<"||MPI - y||="<<normy2<<std::endl;
    }
    
  }


  timer.printInfo();

  MPI_Finalize();
  return 0 ;
}
