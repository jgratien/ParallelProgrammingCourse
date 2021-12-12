#include <mpi.h>

#include <Eigen/Dense>
#include <Eigen/LU>
#include <Eigen/Sparse>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/DenseMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"
#include "Utils/Timer.h"

using vd = std::vector<double>;

vd mult(const vd& x, const vd& values, const int& rows, const int& cols){
  vd y;
  for (int row = 0; row < rows; row++) {
      double value = 0;
      for (int col = 0; col < cols; col++) {
          value += values[row * cols + col] * x[col];
      }
      y.push_back(value);
  }
  return y;
}

int main(int argc, char** argv) {
    using namespace boost::program_options;
    options_description desc;
    desc.add_options()("help", "produce help");
    desc.add_options()("nrows", value<int>()->default_value(0), "matrix size");
    desc.add_options()("nx", value<int>()->default_value(0), "nx grid size");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    MPI_Init(&argc, &argv);

    int my_rank = 0;
    int nb_proc = 1;
    MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    using namespace PPTP;

    Timer timer;
    MatrixGenerator generator;

    double data[nb_proc - 1];
    double result;
    double result_;

    if (my_rank == 0) {
        DenseMatrix matrix;
        int nx = vm["nx"].as<int>();
        generator.genLaplacian(nx, matrix);

        int nrows = matrix.nrows();
        std::vector<double> x;
        std::vector<double> y;
        x.resize(nrows);
        y.resize(nrows);

        for (int i = 0; i < nrows; ++i) x[i] = i + 1;

        {
            Timer::Sentry sentry(timer, "DenseMV");
            matrix.mult(x, y);
        }
        double normy = PPTP::norm2(y);
        std::cout << "||y||=" << normy << std::endl;

        // std::cout << "nrows : " << matrix.nrows() << '\n';
        // std::cout << "ncols : " << matrix.nrows() << '\n';
        // for(int i = 0; i < matrix.nrows(); i++){
        //   for(int j = 0; j < matrix.nrows(); j++){
        //     std::cout << "Matrix " << i << "; " << j << " : " << matrix(i, j)
        //     << '\n';
        //   }
        // }
        // ==========================================================

        Timer::Sentry sentry(timer,"DenseMPI");

        MPI_Bcast(&nrows, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(x.data(), x.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

        unsigned long q = nrows / nb_proc;
        unsigned long r = nrows % nb_proc;

        double* pvalues = matrix.data();

        int epsilon = 0;
        if (r > 0) epsilon = 1;

        int local_size = (q + epsilon);
        // std::cout << "my_rank : " << my_rank << ", local_size : " << local_size << '\n';
        double* local_vptr = pvalues + nrows * local_size;

        for (int i = 1; i < nb_proc; ++i) {
            int proc_local_size = q;
            if (i < r) proc_local_size++;
            MPI_Send(&proc_local_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(local_vptr, proc_local_size * nrows, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
            local_vptr += proc_local_size * nrows;
        }

        std::vector<double> local_values;
        local_values.insert(local_values.end(), pvalues, pvalues + local_size * nrows);

        y = mult(x, local_values, local_size, nrows);
        result = 0;
        for (auto const& e : y) result += e * e;

    } else {
        int nrows;
        int local_size;
        std::vector<double> x;
        std::vector<double> local_values;

        MPI_Bcast(&nrows, 1, MPI_INT, 0, MPI_COMM_WORLD);

        x.resize(nrows);
        MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        MPI_Recv(&local_size, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // std::cout << "my_rank : " << my_rank << ", local_size : " << local_size << '\n';

        local_values.resize(local_size * nrows);
        double* pvalues = local_values.data();
        MPI_Recv(pvalues, local_size * nrows, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // for(auto e : local_values){
        //   std::cout << "my_rank : " << my_rank << ", e : " << e << '\n';
        // }

        std::vector<double> y = mult(x, local_values, local_size, nrows);

        result_ = 0;
        for (auto const& e : y) result_ += e * e;
    }
    MPI_Gather(&result_, 1, MPI_DOUBLE, data, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        Timer::Sentry sentry(timer,"DenseMPI");
        for (int i = 1; i < nb_proc; i++) result += data[i];
        result = std::sqrt(result);
        std::cout << "||y||=" << result << '\n';
    }

    timer.printInfo();
    MPI_Finalize();
    return 0;
}
