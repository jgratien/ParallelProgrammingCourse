/****************************************************************************
 * 
 *   1) Command-line parsing via Boost.Program_options
 *   2) An Eigen-based dense matrix-vector multiplication
 *   3) A dense matrix-vector multiplication on the master process
 *   4) A distribution of matrix blocks among all processes for parallel MV
 *   5) Broadcast/Gather of relevant vectors/matrix data via MPI
 *
 * 
 ****************************************************************************/

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
    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    // Setup for handling command-line options
    using namespace boost::program_options;
    options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help")
        ("nrows", value<int>()->default_value(0), "matrix size")
        ("nx", value<int>()->default_value(0), "nx grid size")
        ("file", value<std::string>(), "file input")
        ("eigen", value<int>()->default_value(0), "use Eigen package");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    // If help is requested, print options and exit
    if (vm.count("help")) {
        std::cout << desc << "\n";
        MPI_Finalize();
        return 1;
    }

    // Get the rank of this process and the number of total processes
    int my_rank = 0;
    int nb_proc = 1;
    MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Use the PPTP namespace for Timer, MatrixGenerator, etc.
    using namespace PPTP;

    // Create a timer object for performance measurements
    Timer timer;
    // Create a matrix generator to either read or generate matrix data
    MatrixGenerator generator;

    // Get the grid size for Laplacian generation from the command line
    int nx = vm["nx"].as<int>();

    /*************************************************************************
     * Optional: Perform an Eigen-based dense matrix-vector multiplication
     *           if --eigen=1 is passed
     ************************************************************************/
    if (vm["eigen"].as<int>() == 1)
    {
        // Using Eigen dynamic matrix/vector for demonstration
        typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType;
        typedef Eigen::Matrix<double, Eigen::Dynamic, 1>              EigenVectorType;

        // For a Laplacian, we consider an nrows = nx*nx matrix
        std::size_t nrows = static_cast<std::size_t>(nx) * static_cast<std::size_t>(nx);
        EigenMatrixType matrix(nrows, nrows);

        // Generate a Laplacian matrix of size (nx x nx)
        generator.genLaplacian(nx, matrix);

        EigenVectorType x(nrows);
        // Initialize the input vector with ascending values
        for(std::size_t i = 0; i < nrows; ++i) {
            x(i) = static_cast<double>(i + 1);
        }

        EigenVectorType y;
        {
            // Time the Eigen-based multiplication
            Timer::Sentry sentry(timer, "EigenDenseMV");
            y = matrix * x;
        }

        // Compute and print the norm of the result
        double normy = PPTP::norm2(y);
        std::cout << "||y|| (Eigen-based) = " << normy << std::endl;
    }

    /*************************************************************************
     * MASTER (rank 0) code to:
     *   - Possibly read matrix from file or generate Laplacian
     *   - Broadcast matrix size
     *   - Distribute matrix blocks to other processes
     *   - Broadcast input vector x
     *   - Perform single-process MV to compare, etc.
     ************************************************************************/
    if (my_rank == 0)
    {
        // We will use a DenseMatrix to store our matrix data
        DenseMatrix matrix;

        // If a file is given, read matrix from file; otherwise generate Laplacian
        if (vm.count("file"))
        {
            std::string file = vm["file"].as<std::string>();
            generator.readFromFile(file, matrix);
        }
        else
        {
            int nx_local = vm["nx"].as<int>();
            generator.genLaplacian(nx_local, matrix);
        }

        // Get total number of rows from the matrix
        std::size_t nrows = matrix.nrows();

        // Prepare an input vector x and initialize with ascending values
        std::vector<double> x(nrows);
        for (std::size_t i = 0; i < nrows; ++i) {
            x[i] = static_cast<double>(i + 1);
        }

        // Broadcast the total number of rows to every process
        MPI_Bcast(&nrows, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

        // Distribute chunks of the matrix to other processes
        for (int i = 1; i < nb_proc; ++i)
        {
            std::size_t start_row = (i * nrows) / nb_proc;
            std::size_t end_row   = ((i + 1) * nrows) / nb_proc;
            std::size_t local_nrows = end_row - start_row;

            // Send local_nrows to rank i
            MPI_Send(&local_nrows, 1, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);

            // Let the matrix handle the process of sending
            matrix.sendToProcess(start_row, end_row, i);
        }

        // Broadcast the input vector x to all processes
        MPI_Bcast(x.data(), static_cast<int>(nrows), MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // Single-process multiply (for comparison/performance reference)
        {
            std::vector<double> y(nrows);
            Timer::Sentry sentry(timer, "DenseMV");
            matrix.mult(x, y);  // Full matrix-vector multiplication on rank 0

            // Compute norm of the result
            double normy = PPTP::norm2(y);
            std::cout << "||y|| (single-process) = " << normy << std::endl;
        }

        // Now do the local portion on rank 0 as well
        // We will effectively treat rank 0's chunk as local_data
        DenseMatrix local_matrix;
        std::size_t local_nrows = (nrows / nb_proc);

        // The broadcast below is to keep code consistent with the structure
        // rank 0 also "receives" local_nrows from itself
        MPI_Bcast(&local_nrows, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

        // Initialize local_matrix and copy the relevant portion
        local_matrix.init(local_nrows);
        local_matrix.copy(matrix);  // Here we might do partial copy if needed

        // Now compute local spMV for rank 0 portion
        std::vector<double> local_y(local_nrows);
        local_matrix.mult(x, local_y);  // partial result

        // We would typically gather partial results from all ranks at this point.
        // (Not fully shown in the original code.)
    }
    /*************************************************************************
     * OTHER RANKS code to:
     *   - Receive the global row count
     *   - Receive their local matrix chunk
     *   - Broadcast the input vector x
     *   - Perform local spMV
     ************************************************************************/
    else
    {
        // We'll hold local matrix data here
        DenseMatrix local_matrix;
        std::size_t nrows    = 0;
        std::size_t local_nrows = 0;

        // Receive total number of rows from the master
        MPI_Bcast(&nrows, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

        // Receive local_nrows for this rank
        MPI_Recv(&local_nrows, 1, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Initialize local_matrix and receive chunk from the master
        local_matrix.init(local_nrows);
        local_matrix.recvFromProcess(local_nrows, 0);

        // Prepare the input vector x (same size as nrows)
        std::vector<double> x(nrows);

        // Broadcast x from master
        MPI_Bcast(x.data(), static_cast<int>(nrows), MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // Allocate local_y for partial results
        std::vector<double> local_y(local_nrows);

        // Perform local spMV
        local_matrix.mult(x, local_y);

        // Typically, partial results would be sent back to master or gathered
        // (Not fully shown in the original code.)
    }

    // Print timing info from each rank
    timer.printInfo();

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}
