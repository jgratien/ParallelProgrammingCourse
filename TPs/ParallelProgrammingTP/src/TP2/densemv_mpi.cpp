/*
 * helloworld.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: gratienj
 */
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

#include "Utils/TimerNow.h"

int main(int argc, char **argv)
{
    using namespace boost::program_options;
    using namespace PPTP;

    options_description desc;
    variables_map vm;

    MPI_Init(&argc, &argv);

    int my_rank = 0;
    int nb_proc = 1;

    MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    double norm = 0;
    double local_norm = 0;
    double start, end;

    MPI_Status status;
    MPI_Request req,req2;

    if (my_rank == 0)
    {

        desc.add_options()("help", "produce help")("nrows", value<int>()->default_value(0), "matrix size")("nx", value<int>()->default_value(0), "nx grid size")("file", value<std::string>(), "file input")("eigen", value<int>()->default_value(0), "use eigen package");
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return 1;
        }

        MatrixGenerator generator;

        int nx = vm["nx"].as<int>();

        if (vm["eigen"].as<int>() == 1)
        {
            typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType;
            typedef Eigen::Matrix<double, Eigen::Dynamic, 1> EigenVectorType;

            std::size_t nrows = nx;
            EigenMatrixType matrix(nrows, nrows);

            generator.genLaplacian(nx, matrix);

            EigenVectorType x(nrows);

            for (std::size_t i = 0; i < nrows; ++i)
                x(i) = i + 1;

            EigenVectorType y;
            {
                y = matrix * x;
            }

            double normy = PPTP::norm2(y);
            std::cout << "||y|| = " << normy << std::endl;
        }
        else
        {
            //Baseline sequential implementation
            DenseMatrix matrix;

            if (vm.count("file"))
            {
                std::string file = vm["file"].as<std::string>();
                generator.readFromFile(file, matrix);
            }
            else
            {
                generator.genLaplacian<DenseMatrix>(nx, matrix);
            }

            std::vector<double> x(nx);
            for (std::size_t i = 0; i < nx; i++)
                x[i] = i + 1;
            std::vector<double> y(nx);
            {
                matrix.mult(x, y);
            }
            double normy = PPTP::norm2(y);
            std::cout << "||y|| = " << normy << std::endl;
        }

        //------------------------------Parallel implementation---------------------------

        DenseMatrix matrix;

        //Preparing matrix data
        if (vm.count("file"))
        {
            std::string file = vm["file"].as<std::string>();
            generator.readFromFile(file, matrix);
        }
        else
        {
            generator.genLaplacian<DenseMatrix>(nx, matrix);
        }

        int nrows = nx;

        //preparing X vector
        std::vector<double> x(nrows);
        for (std::size_t i = 0; i < nrows; i++)
            x[i] = i + 1;

        start = now();
        std::size_t local_nrows;
        std::size_t slaveLocalSize;

        std::size_t rest = nrows % nb_proc;

        slaveLocalSize = nx / nb_proc;
        local_nrows = slaveLocalSize + rest;

        std::vector<double> const &dataVector = matrix.getValues();

        {
            // SEND GLOBAL & LOCAL SIZES
            MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
            MPI_Bcast(&slaveLocalSize, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

            // SEND X
            MPI_Ibcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD, &req);

            // SEND LOCAL MATRICES
            for (int i = 1; i < nb_proc; ++i)
            {
		double const *localDataPtr = dataVector.data() + local_nrows * nx + slaveLocalSize * nx * (i-1);
                
                MPI_Isend(localDataPtr, slaveLocalSize * nrows, MPI_DOUBLE, i, i * 3 + 1, MPI_COMM_WORLD, &req2);
            }
        }

        // COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
        // DenseMatrix local_matrix;
        std::vector<double> local_y(local_nrows);
            // compute parallel SPMV
            double tempSum = 0;
            for (int row = 0; row < local_nrows; row++)
            {
                tempSum = 0;
                for (int col = 0; col < nrows; col++)
                {
                    tempSum += dataVector.at(row * nrows + col) * x.at(col);
                }
                local_y[row] = tempSum;
                local_norm += tempSum * tempSum;
            }
        MPI_Wait(&req, MPI_STATUS_IGNORE);
        MPI_Wait(&req2, MPI_STATUS_IGNORE);
    }

    else
    {
        // DenseMatrix local_matrix;
        std::size_t nrows;
        std::size_t slaveLocalSize;

        std::vector<double> local_vals; //= new std::vector<double>();
        std::vector<double> x;

        {
            // RECV GLOBAL SIZE
            MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

            // RECV LOCAL SIZE
            MPI_Bcast(&slaveLocalSize, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

            // BROAD CAST VECTOR X
            x.resize(nrows);
            MPI_Ibcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD, &req);

            //RECV LOCAL MATRIX DATA
            local_vals.resize(slaveLocalSize * nrows);
            MPI_Irecv(local_vals.data(), slaveLocalSize * nrows, MPI_DOUBLE, 0, my_rank * 3 + 1, MPI_COMM_WORLD, &req2);
        }

        std::vector<double> local_y;
        double tempSum = 0;
        MPI_Wait(&req, MPI_STATUS_IGNORE);
        MPI_Wait(&req2, MPI_STATUS_IGNORE);
        local_y.resize(slaveLocalSize);
        {
            // compute parallel SPMV

            for (int row = 0; row < slaveLocalSize; row++)
            {
                tempSum = 0;
                for (int col = 0; col < nrows; col++)
                {
                    tempSum += local_vals.at(row * nrows + col) * x.at(col);
                }
                local_y[row] = tempSum;
                local_norm += tempSum * tempSum;
            }

            //MPI_Send(local_y.data(), local_nrows, MPI_DOUBLE, 0, my_rank * 4 + 1, MPI_COMM_WORLD);
        }
    }

    MPI_Reduce(&local_norm, &norm, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        end = now();
        std::cout << "Time =" << end - start << std::endl;
        std::cout << "Parallel ||y||=" << sqrt(norm) << std::endl;
    }

    MPI_Finalize();
    return 0;
}
