#include <mpi.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "MatrixVector/DenseMatrix.h"
#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"
#include "Utils/Timer.h"

#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int my_rank, nb_proc;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

    using namespace boost::program_options;
    options_description desc;
    desc.add_options()
      ("help", "produce help")
      ("nx", value<int>()->default_value(0), "nx grid size");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
      if (my_rank == 0) std::cout << desc << "\n";
      MPI_Finalize();
      return 1;
    }

    PPTP::CSRMatrix matrix;
    int nx = vm["nx"].as<int>();

    // Rank 0 generates the matrix
    std::size_t nrows=0, nnz=0;
    if (my_rank == 0) {
        PPTP::MatrixGenerator generator;
        generator.genLaplacian(nx, matrix);
        nrows = matrix.nrows();
        nnz = matrix.nnz();
    }

    // Broadcast dimensions
    MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nnz, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    // Rank 0 times the sequential multiply
    PPTP::Timer timer_seq, timer_par;
    double time_as_double;
    if (my_rank == 0) {
        std::vector<double> x_seq(nrows), y_seq(nrows, 0.0);
        for (std::size_t i = 0; i < nrows; ++i)
            x_seq[i] = i + 1.0;

        {
            PPTP::Timer::Sentry sentry_seq(timer_seq, "Sequential SparseMV");
            matrix.mult(x_seq, y_seq);
        }
        double normy = PPTP::norm2(y_seq);
        std::cout << "||y_SEQ||=" << normy << std::endl;
        timer_seq.printInfo();

        const auto& counters = timer_seq.getCounters();
        auto iter = counters.find("Sequential SparseMV");
        if (iter != counters.end()) {
            time_as_double = iter->second;
        }
    }

    // Broadcast matrix data
    std::vector<int> kcol, cols;
    std::vector<double> values;
    if (my_rank == 0) {
        kcol = matrix.kcol();
        cols = matrix.cols();
        values = matrix.values();
    } else {
        kcol.resize((int)nrows + 1);
        cols.resize((int)nnz);
        values.resize((int)nnz);
    }

    MPI_Bcast(kcol.data(), (int)kcol.size(), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(cols.data(), (int)cols.size(), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(values.data(), (int)values.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Distribute rows
    int rows_per_proc = (int)(nrows / nb_proc);
    int remaining_rows = (int)(nrows % nb_proc);

    // Compute local start/end for each process
    int my_start = my_rank * rows_per_proc + std::min(my_rank, remaining_rows);
    int my_end   = my_start + rows_per_proc - 1;
    if (my_rank < remaining_rows) my_end++;

    int my_local_size = my_end - my_start + 1;

    // Broadcast vector x
    std::vector<double> x(nrows);
    if (my_rank == 0) {
        for (std::size_t i = 0; i < nrows; ++i) x[i] = i + 1.0;
    }
    MPI_Bcast(x.data(), (int)nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Barrier before timing parallel multiplication
    MPI_Barrier(MPI_COMM_WORLD);

    double t0 = MPI_Wtime();
    // Compute local part
    std::vector<double> local_y(my_local_size, 0.0);
    for (int i = 0; i < my_local_size; ++i) {
        std::size_t irow = (std::size_t)(my_start + i);
        double value = 0.0;
        for (int k = kcol[irow]; k < kcol[irow + 1]; ++k) {
            value += values[k] * x[cols[k]];
        }
        local_y[i] = value;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    double local_time = t1 - t0;
    double max_time;
    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // Gather results
    std::vector<int> recvcounts(nb_proc), displs(nb_proc);
    if (my_rank == 0) {
        for (int i = 0; i < nb_proc; ++i) {
            int s = i * rows_per_proc + std::min(i, remaining_rows);
            int e = s + rows_per_proc - 1;
            if (i < remaining_rows) e++;
            int size = e - s + 1;
            recvcounts[i] = size;
        }
        displs[0] = 0;
        for (int i = 1; i < nb_proc; ++i)
            displs[i] = displs[i - 1] + recvcounts[i - 1];
    }

    std::vector<double> global_y;
    if (my_rank == 0) global_y.resize(nrows);

    MPI_Gatherv(local_y.data(), my_local_size, MPI_DOUBLE,
                global_y.data(), recvcounts.data(), displs.data(),
    		MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        double normy_par = PPTP::norm2(global_y);
        std::cout << "||y_PAR||=" << normy_par << std::endl;
        std::cout << "Parallel SparseMV max time: " << max_time << std::endl;
    }
    if (my_rank == 0) {
	
    std::ofstream outfile;
    outfile.open("/gext/mahammed.el_sharkawy/parallel-programming-project/TPs/ParallelProgrammingTP/Bench.log",std::ios_base::app);
    outfile << "SPMV  np: " << nb_proc << "  nx: " << nx << "  Time SPMV: "
            << time_as_double << " Time MPI: " << max_time
            << " Acceleration: " << time_as_double/max_time << std::endl;

    
    }


    MPI_Finalize();
    return 0;
}
