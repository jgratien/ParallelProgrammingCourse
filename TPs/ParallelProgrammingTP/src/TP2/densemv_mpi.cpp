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

int main(int argc, char** argv) {
    using namespace boost::program_options;
    options_description desc;
    desc.add_options()("help", "produce help")(
        "nrows", value<int>()->default_value(0), "matrix size")(
        "nx", value<int>()->default_value(0), "nx grid size")(
        "file", value<std::string>(), "file input")(
        "eigen", value<int>()->default_value(0), "use eigen package");
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
        std::size_t local_size_;

        int nx = vm["nx"].as<int>();
        generator.genLaplacian(nx, matrix);

        int nrows = matrix.nrows();
        std::vector<double> x;
        x.resize(nrows);

        for (std::size_t i = 0; i < nrows; ++i) x[i] = i + 1;

        {
            std::vector<double> y(nrows);
            {
                Timer::Sentry sentry(timer, "DenseMV");
                matrix.mult(x, y);
            }
            double normy = PPTP::norm2(y);
            std::cout << "||y||=" << normy << std::endl;
        }

        // std::cout << "nrows : " << matrix.nrows() << '\n';
        // std::cout << "ncols : " << matrix.nrows() << '\n';
        // for(int i = 0; i < matrix.nrows(); i++){
        //   for(int j = 0; j < matrix.nrows(); j++){
        //     std::cout << "Matrix " << i << "; " << j << " : " << matrix(i, j)
        //     << '\n';
        //   }
        // }

        Timer::Sentry sentry(timer,"Rank 0 Begin") ;

        {
            MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

            unsigned long q = nrows / nb_proc;
            unsigned long r = nrows % nb_proc;
            double* matrix_ptr = matrix.data();
            int epsilon = 0;
            if (r > 0) epsilon = 1;
            local_size_ = (q + epsilon);
            // std::cout << "local_size_ : " << local_size_ << '\n';
            double* local_ptr = matrix_ptr + nrows * local_size_;
            // std::cout << "nrows : " << nrows << '\n';
            for (int i = 1; i < nb_proc; ++i) {
                // std::cout<<" SEND MATRIX DATA to proc "<<i<<std::endl ;
                int local_size_test = q;
                if (i < r) local_size_test++;
                MPI_Send(&local_size_test, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(local_ptr, local_size_test * nrows, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
                local_ptr += local_size_test * nrows;
            }
        }

        {
            MPI_Bcast(x.data(), x.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
            // for(double e : x){
            //   std::cout << "Sent : x values : " << e << '\n';
            // }
        }

        DenseMatrix local_matrix;
        std::vector<double> y_local(local_size_);

        {
            local_matrix.init(local_size_, nrows);
            double* ptr = local_matrix.data();
            double const* matrix_ptr = matrix.data();
            for (int i = 0; i < (local_size_ * nrows); i++) {
                ptr[i] = matrix_ptr[i];
            }

            matrix_ptr = local_matrix.data();
            for (std::size_t irow = 0; irow < local_size_; ++irow) {
                double value = 0;
                for (std::size_t jcol = 0; jcol < nrows; ++jcol) {
                    value += matrix_ptr[jcol] * x[jcol];
                }
                y_local[irow] = value;
                matrix_ptr += nrows;
            }
            // std::cout << "Rank : " << my_rank << ", Done." << '\n';
            result = 0;
            for (auto const& x : y_local) result += x * x;
            // std::cout << "Rank : " << my_rank << " and " << result << '\n';
        }
    } else {
        DenseMatrix local_matrix;
        int nrows;
        int local_size;

        {
            MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

            MPI_Recv(&local_size, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // std::cout << "Received [rank: " << my_rank << ", local_size: " << local_size << "]" << '\n';
            local_matrix.init(local_size, nrows);
            double* ptr = local_matrix.data();
            MPI_Recv(ptr, local_size * nrows, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // for(int e = 0; e < local_size * nrows; e++){
            //   std::cout << "[Received] Element [e : " << e << ", value : " <<
            //   ptr[e] << "]" << '\n';
            // }
        }

        std::vector<double> x;
        x.resize(nrows);
        {
            MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            // for(double e : x){
            //   std::cout << "Received : x values : " << e << '\n';
            // }
        }

        std::vector<double> y_local(local_size);
        y_local.resize(local_size);
        {
            double const* matrix_ptr = local_matrix.data();
            for (std::size_t irow = 0; irow < local_size; ++irow) {
                double value = 0;
                for (std::size_t jcol = 0; jcol < nrows; ++jcol) {
                    value += matrix_ptr[jcol] * x[jcol];
                }
                y_local[irow] = value;
                matrix_ptr += nrows;
            }

            // for(double e : y_local){
            //   std::cout << "[Checked] Element [e : " << e << "]" << '\n';
            // }

            // std::cout << "Rank : " << my_rank << ", Done." << '\n';
            result_ = 0;
            for (auto const& x : y_local) result_ += x * x;
            // std::cout << "Rank : " << my_rank << " and " << result_ << '\n';
        }
    }
    MPI_Gather(&result_, 1, MPI_DOUBLE, data, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        Timer::Sentry sentry(timer,"Rank 0 End") ;
        int i;
        for (i = 1; i < nb_proc; i++) {
            result += data[i];
        }
        result = std::sqrt(result);
        std::cout << "||y||=" << result << '\n';
    }

    timer.printInfo();
    MPI_Finalize();
    return 0;
}
