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

#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"

#include "Utils/Timer.h"

void printNonZeroElements(const PPTP::CSRMatrix& matrix) {
    std::cout << "Non-zero elements in the local matrix:" << std::endl;

    const int* kcols = matrix.kcol();   // Row pointers
    const int* cols = matrix.cols();   // Column indices
    const double* values = matrix.values(); // Non-zero values

    for (std::size_t row = 0; row < matrix.nrows(); ++row) {
        int start = kcols[row];       // Starting index for this row
        int end = kcols[row + 1];     // Ending index for this row

        std::cout << "Row " << row << ": ";
        for (int idx = start; idx < end; ++idx) {
            int col = cols[idx];      // Column index of the non-zero element
            double value = values[idx]; // Value of the non-zero element

            std::cout << "(" << row << ", " << col << ") -> " << value << " ";
        }
        std::cout << std::endl;
    }
}


/*MPI_Datatype createCSRRangeType() {
    MPI_Datatype csr_type;
    int block_lengths[5] = {1, 1, 1, 1, 1}; // Each pointer is 1 unit
    MPI_Aint offsets[5];
    MPI_Datatype types[5] = {MPI_INT, MPI_INT, MPI_DOUBLE, MPI_UNSIGNED_LONG,  MPI_UNSIGNED_LONG};

    offsets[0] = offsetof(PPTP::CSRData, kcol);
    offsets[1] = offsetof(PPTP::CSRData, cols);
    offsets[2] = offsetof(PPTP::CSRData, values);
    offsets[3] = offsetof(PPTP::CSRData, nrows);
    offsets[4] = offsetof(PPTP::CSRData, nnz);

    MPI_Type_create_struct(5, block_lengths, offsets, types, &csr_type);
    MPI_Type_commit(&csr_type);

    return csr_type;
}*/

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

  // step 2 : initialize (1) and finalize (2)
  MPI_Init(&argc, &argv); // (1)
  // --------------------

  // step 3 : Initialize Variables
  int world_size;
  int world_rank;

  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  // --------------------

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
    std::cout << "Process " << world_rank + 1 << " in " << world_size <<std::endl;

    std::size_t global_nrows;
    std::vector<double> x, y, local_y;

    CSRMatrix local_matrix;
    std::size_t local_nrows;

    std::size_t remainder;

    //MPI_Datatype csr_type = createCSRRangeType();
    CSRData data;

    //Timer::Sentry sentry(timer,"MPI_SpMV") ;
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
        x.resize(global_nrows);
        //printNonZeroElements(matrix);

        for(std::size_t i=0;i<global_nrows;++i)
            x[i] = i+1 ;

        // Step 5 : Zero Sending local matrix info
        int offset = 0;
        {
          remainder = global_nrows % world_size;
          local_nrows = global_nrows / world_size + (world_rank < remainder ? 1:0);
          offset += local_nrows;

          for(std::size_t i = 1; i < world_size; ++i)
          {
            local_nrows = global_nrows / world_size + (i < remainder ? 1:0);

            // Sending local_nrows
            // MPI_Send(&local_nrows, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);
                
            local_matrix = matrix.extractSubmatrix(offset, offset + local_nrows);

            int nrows = local_matrix.nrows();
            int nnz = local_matrix.nnz();

            // Calculate total size for the buffer
            int total_size = 2 + (nrows + 1) + nnz + nnz; // Metadata + kcols + cols + values

            // Create a buffer for contiguous data
            std::vector<double> buffer(total_size);

            // Pack metadata
            buffer[0] = nrows;
            buffer[1] = nnz;

            // Pack kcols
            std::copy(local_matrix.kcol(), local_matrix.kcol() + local_matrix.nrows()  + 1, buffer.begin() + 2);

            // Pack cols (offset by 2 + (nrows + 1))
            std::copy(local_matrix.cols(), local_matrix.cols() + local_matrix.nnz(), buffer.begin() + 2 + (local_matrix.nrows() + 1));

            // Pack values (offset by 2 + (nrows + 1) + nnz)
            std::copy(local_matrix.values(), local_matrix.values() + local_matrix.nnz(), buffer.begin() + 2 + (local_matrix.nrows() + 1) + local_matrix.nnz());

            // Send the buffer
            MPI_Send(buffer.data(), total_size, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);

            offset += local_nrows;
          }
          
          // zero computing it's data
          local_nrows = global_nrows / world_size + (world_rank < remainder ? 1:0);
          local_matrix = matrix.extractSubmatrix(0, 0 + local_nrows);
        }
        // --------------------

        {
            std::vector<double> y(global_nrows);
            {
            Timer::Sentry sentry(timer,"SpMV") ;
            matrix.mult(x,y) ;
            }
            double normy = PPTP::norm2(y) ;
            std::cout<<"||y||="<<normy<<std::endl ;
        }
    } 

    // Step 4 : Zero Sending global matrix size and x and others Receiving 
    {
        // gloal_matrix_size
        MPI_Bcast(&global_nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
        std::cout << "Global nrows " << global_nrows <<std::endl;

        // vector x
        x.resize(global_nrows);
        MPI_Bcast(x.data(), global_nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        std::cout << "Vector of size " << x.size() <<std::endl;
    }
    // --------------------

    // Step 6 : Receiving local matrix infos
    if(world_rank != 0)
    {
      MPI_Status status;

      // Receiving local_nrows
      //MPI_Recv(&local_nrows, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, &status);
      //std::cout << "Received local nrows " << local_nrows <<std::endl;

      // Probe message size
      MPI_Probe(0, 1, MPI_COMM_WORLD, &status);
      int count;
      MPI_Get_count(&status, MPI_DOUBLE, &count);

      // Allocate buffer
      std::vector<double> buffer(count);

      // Receiving local_matrix_data
      MPI_Recv(buffer.data(), count, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &status);
        
      local_matrix.copyCSRMatrixFromData(buffer);
      std::cout << "Received local matrix data " << local_matrix.data().nrows <<std::endl;
    } else {
      std::cout << "Received local matrix data " << local_matrix.data().nrows <<std::endl;
    }
    // --------------------

    //printNonZeroElements(local_matrix);

    // Step 7 : Computing the local multiplication
    local_y.resize(local_matrix.nrows());
    {
      local_matrix.mult(x,local_y);
    }

    std::cout << "Local y vector: \n";
    for (std::size_t i = 0; i < local_y.size(); ++i) {
      std::cout << local_y[i] << " ";
    }
    std::cout << std::endl;

    // Gather the results back to process 0
    y.resize(global_nrows);


    // Create sendcounts and displacements arrays for MPI_Gatherv
    std::vector<int> sendcounts(world_size, 0);
    std::vector<int> displacements(world_size, 0);

    // Calculate sendcounts and displacements
    int total_send_count = 0;
    for (std::size_t i = 0; i < world_size; ++i) {
        sendcounts[i] = (i < remainder) ? (global_nrows / world_size + 1) : (global_nrows / world_size);
        if (i > 0) {
            displacements[i] = displacements[i - 1] + sendcounts[i - 1];
        }
        total_send_count += sendcounts[i];
    }

    // Gather data to root (process 0)
    MPI_Gatherv(local_y.data(), local_y.size(), MPI_DOUBLE, 
                y.data(), sendcounts.data(), displacements.data(), 
                MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (world_rank == 0)
    {
      double normy2 = PPTP::norm2(y);
      std::cout<<"||MPI - y||="<<normy2<<std::endl;

      std::cout << "Gathered y vector: \n";
      for (int i = 0; i < global_nrows; ++i) {
        std::cout << y[i] << " ";
      }
      std::cout << std::endl;
    }

    //MPI_Type_free(&csr_type);

  }
  timer.printInfo();

  // step 2 : initialize (1) and finalize (2)
  MPI_Finalize(); // (2)
  // --------------------

  return 0 ;
}
