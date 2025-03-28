/********************************************************************************************
 * 
 * OVERVIEW:
 * ----------------------
 * This program demonstrates how to perform a Sparse Matrix-Vector (SpMV) multiplication 
 * using two different approaches:
 *   1) Eigen-based SpMV (optional, triggered by "--eigen 1").
 *   2) Custom parallel SpMV using a CSR (Compressed Sparse Row) data structure and MPI.
 *
 * We use MPI (Message Passing Interface) to distribute the rows of the matrix among multiple 
 * processes (or "ranks"). Each rank works on its allocated portion of the input vector 'x' 
 * and the matrix, computes partial results, and then gathers everything back on the root rank.
 *
 * The program also uses Boost.Program_options to handle command-line arguments such as 
 * --help, --nx, --file, and --eigen. The user can specify:
 *   - --file <filename>  : Reads the matrix from a file.
 *   - --nx <integer>     : Generates a Laplacian matrix of size nx * nx if no file is provided.
 *   - --eigen <0 or 1>   : If set to 1, performs an additional SpMV using Eigen.
 *
 * The code performs the following steps:
 *   1. Initialize MPI and determine rank/size.
 *   2. Parse command-line arguments via Boost.Program_options.
 *   3. Optionally perform an Eigen-based SpMV if --eigen=1.
 *   4. Create or load a CSR matrix from file or generate a Laplacian.
 *   5. Perform a single-process CSR SpMV (only rank 0 does the actual computation).
 *   6. Distribute the input vector 'x' among all ranks (Scatter).
 *   7. Each rank multiplies its local rows of the matrix by the global 'x'.
 *   8. Gather partial results from ranks back into a full result on rank 0 (Gather).
 *   9. Print norms and timing information.
 *   10. Finalize MPI and exit.
 *
 * We use a custom Timer class to measure execution time for major steps (e.g., "EigenSpMV",
 * "SpMV" for single-process, and "MPISpMV" for the distributed version).
 *
 *
 *
 ********************************************************************************************/

#include <iostream>                              // For std::cout, std::endl
#include <boost/lexical_cast.hpp>                // Boost lexical_cast
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <mpi.h>                                 // MPI header
 
#include <string>
#include <vector>
#include <fstream>
 
#include <Eigen/Dense>                           // Eigen includes for dense operations
#include <Eigen/Sparse>                          // Eigen includes for sparse operations
#include <Eigen/LU>
 
#include "MatrixVector/CSRMatrix.h"              // Custom CSR matrix class
#include "MatrixVector/LinearAlgebra.h"          // norm2() function, etc.
#include "MatrixVector/MatrixGenerator.h"        // To generate or load matrices
#include "Utils/Timer.h"                         // Timer class for performance measurement

int main(int argc, char** argv)
{
    //--------------------------------------------------------------------
    // 1) Initialize MPI environment
    //--------------------------------------------------------------------
    MPI_Init(&argc, &argv);

    // Variables to store current rank and total number of processes
    int world_rank = 0; 
    int num_procs  = 1;

    // Obtain total process count in MPI_COMM_WORLD and the rank of each process
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    //--------------------------------------------------------------------
    // 2) Parse command-line arguments using Boost.Program_options
    //--------------------------------------------------------------------
    using namespace boost::program_options;
    options_description desc("Options");  // Describe what arguments are available

    // Define available command-line options
    desc.add_options()
        ("help",    "Show help message")                           // e.g. --help
        ("nrows",   value<int>()->default_value(0),  "Global matrix size")
        ("nx",      value<int>()->default_value(0),  "Grid dimension for Laplacian")
        ("file",    value<std::string>(),            "Input file for reading matrix")
        ("eigen",   value<int>()->default_value(0),  "Use Eigen (0 or 1)");

    // Variable map to store the parsed option values
    variables_map vm;

    // Parse the command line into the vm
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    // If the user specifies --help, print the options and exit
    if (vm.count("help")) {
        std::cout << desc << "\n";
        MPI_Finalize();
        return 1;
    }

    //--------------------------------------------------------------------
    // 3) Create tools/classes from the PPTP namespace
    //--------------------------------------------------------------------
    using namespace PPTP; // Allows direct usage of Timer, MatrixGenerator, etc.
    Timer timer;          // For measuring performance intervals
    MatrixGenerator gen;  // Helps to read/generate matrices

    //--------------------------------------------------------------------
    // 4) Optional: If eigen=1, perform Eigen-based SpMV
    //--------------------------------------------------------------------
    if (vm["eigen"].as<int>() == 1)
    {
        // Define convenient type aliases for Eigen's sparse matrix and vector
        typedef Eigen::SparseMatrix<double> EigenSparse;
        typedef Eigen::VectorXd            EigenVec;

        // We'll store the matrix in an Eigen sparse matrix
        EigenSparse eigen_matrix;

        // If user provided --file, read from file; otherwise generate Laplacian
        if (vm.count("file")) {
            std::string filepath = vm["file"].as<std::string>();
            gen.readFromFile(filepath, eigen_matrix);  // reads from disk
        } else {
            int ngrid = vm["nx"].as<int>();
            gen.genLaplacian(ngrid, eigen_matrix);     // generate a Laplacian matrix
        }

        // Prepare the input vector x_in with ascending values (1,2,3,...)
        std::size_t total_rows = eigen_matrix.rows();
        EigenVec x_in(total_rows);
        for (std::size_t i = 0; i < total_rows; ++i)
            x_in(i) = static_cast<double>(i + 1);

        // Perform the multiplication using Eigen
        EigenVec y_out;
        {
            // Timer::Sentry starts and stops timing automatically for "EigenSpMV"
            Timer::Sentry sentry(timer, "EigenSpMV");
            y_out = eigen_matrix * x_in;
        }

        // Compute the norm of the result vector
        double norm_eigen = PPTP::norm2(y_out);

        // Only print on the root rank
        if (world_rank == 0) {
            std::cout << "||y|| = " << norm_eigen << " (Eigen-based)\n";
        }
    }

    //--------------------------------------------------------------------
    // 5) Create or load a CSRMatrix (our custom data type) for parallel SpMV
    //--------------------------------------------------------------------
    CSRMatrix sparse_mat;

    // If --file was passed, read from the file; otherwise generate a Laplacian
    if (vm.count("file")) {
        std::string cfile = vm["file"].as<std::string>();
        gen.readFromFile(cfile, sparse_mat);
    } else {
        int ngrid = vm["nx"].as<int>();
        gen.genLaplacian(ngrid, sparse_mat);
    }

    // Get the number of rows in the CSR matrix
    std::size_t nrows = sparse_mat.nrows();

    // Prepare global vectors x, y, and y2
    //  x: the input vector to be multiplied
    //  y: result of a single-process approach
    //  y2: result of the parallel approach (collected back on root)
    std::vector<double> x(nrows), y(nrows), y2(nrows);

    //--------------------------------------------------------------------
    // 6) Prepare data structures for splitting rows among processes
    //--------------------------------------------------------------------
    // sendcounts: how many rows each rank will handle
    // displs    : where each chunk starts in the global array
    int *sendcounts = new int[num_procs];
    int *displs     = new int[num_procs];

    // base_rows = nrows / num_procs, leftover = nrows % num_procs
    // leftover is used to distribute extra rows among the first 'leftover' ranks
    int base_rows = static_cast<int>(nrows / num_procs);
    int leftover  = static_cast<int>(nrows % num_procs);

    // Fill sendcounts/displs so ranks know which rows they own
    for (int i = 0; i < num_procs; ++i)
    {
        // Each rank i gets base_rows plus 1 more if i < leftover
        sendcounts[i] = base_rows + ((i < leftover) ? 1 : 0);

        // For displacements, accumulate from previous ranks
        if (i > 0) {
            displs[i] = displs[i - 1] + sendcounts[i - 1];
        } else {
            displs[i] = 0;
        }
    }

    // local_start is the first row index for this rank
    // local_end   is the first row index that this rank does NOT own
    int local_start = displs[world_rank];
    int local_end   = local_start + sendcounts[world_rank];

    // Create local_x, local_y to store just the portion of the vector relevant to this rank
    std::vector<double> local_x(sendcounts[world_rank]);
    std::vector<double> local_y(sendcounts[world_rank]);

    //--------------------------------------------------------------------
    // 7) Populate the input vector x with ascending values
    //--------------------------------------------------------------------
    for (std::size_t i = 0; i < nrows; ++i) {
        x[i] = static_cast<double>(i + 1);
    }

    //--------------------------------------------------------------------
    // 8) Single-process SpMV (on the entire CSR matrix) for reference
    //    - This is performed as if we only have 1 process. Times the full local multiply.
    //--------------------------------------------------------------------
    {
        // Time the single-process multiplication
        Timer::Sentry sentry(timer, "SpMV");
        sparse_mat.mult(x, y);  // multiply entire matrix by x, store in y
    }

    // Rank 0 prints the norm of the result from single-process approach
    if (world_rank == 0) {
        double norm_res = PPTP::norm2(y);
        std::cout << "||y|| = " << norm_res << " (Single-process)\n";
    }

    //--------------------------------------------------------------------
    // 9) Scatter the global input vector x to the local_x of each rank
    //--------------------------------------------------------------------
    MPI_Scatterv(x.data(),               // data on the root
                 sendcounts,             // how many elements each rank gets
                 displs,                 // displacements in the global array
                 MPI_DOUBLE,             // MPI data type
                 local_x.data(),         // local buffer for this rank
                 sendcounts[world_rank], // # of elements for this rank
                 MPI_DOUBLE,
                 0,                      // root rank
                 MPI_COMM_WORLD);

    //--------------------------------------------------------------------
    // 10) Each rank performs local SpMV on its subset of rows
    //--------------------------------------------------------------------
    {
        // Time the distributed multiplication
        Timer::Sentry distributed_time(timer, "MPISpMV");

        // Loop over rows from local_start to local_end-1
        for (int row = local_start; row < local_end; ++row)
        {
            double temp_val = 0.0;
            // For each non-zero entry in this row, multiply and accumulate
            for (int k = sparse_mat.m_kcol[row]; k < sparse_mat.m_kcol[row + 1]; ++k)
            {
                temp_val += sparse_mat.m_values[k] * x[sparse_mat.m_cols[k]];
            }
            // Store the result in local_y with a shift in index
            local_y[row - local_start] = temp_val;
        }
    }

    //--------------------------------------------------------------------
    // 11) Gather partial results from all ranks into y2 on the root
    //--------------------------------------------------------------------
    MPI_Gatherv(local_y.data(),             // local data for this rank
                sendcounts[world_rank],     // how many elements to send
                MPI_DOUBLE,                 // MPI data type
                y2.data(),                  // global array on root
                sendcounts,                 // # of elements from each rank
                displs,                     // displacements in the global array
                MPI_DOUBLE,
                0,                          // root rank
                MPI_COMM_WORLD);

    // Root rank prints the norm of the fully assembled result (y2)
    if (world_rank == 0)
    {
        double final_norm = PPTP::norm2(y2);
        std::cout << "||y2|| = " << final_norm
                  << "   Processes:" << num_procs
                  << "   NX:"        << vm["nx"].as<int>()
                  << std::endl;
    }

    //--------------------------------------------------------------------
    // 12) Clean up and finalize
    //--------------------------------------------------------------------
    // Deallocate arrays created with new[]
    delete[] sendcounts;
    delete[] displs;

    // Only rank 0 prints full timer info
    if (world_rank == 0) {
        timer.printInfo();
    }

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}
