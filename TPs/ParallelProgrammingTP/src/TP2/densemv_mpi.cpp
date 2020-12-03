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

#include "Utils/Timer.h"

int main(int argc, char **argv)
{
    using namespace boost::program_options;
    options_description desc;
    desc.add_options()("help", "produce help")("nrows", value<int>()->default_value(0), "matrix size")("nx", value<int>()->default_value(0), "nx grid size")("file", value<std::string>(), "file input")("eigen", value<int>()->default_value(0), "use eigen package");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help"))
    {
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
            Timer::Sentry sentry(timer, "EigenDenseMV");
            y = matrix * x;
        }

        double normy = PPTP::norm2(y);
        std::cout << "||y||=" << normy << std::endl;
    }

    //Parallel implementation

    double norm = 0;
    double local_norm = 0;

    if (my_rank == 0)
    {
        DenseMatrix matrix;

        if (vm.count("file"))
        {
            std::string file = vm["file"].as<std::string>();
            generator.readFromFile(file, matrix);
        }
        else
        {
            int nx = vm["nx"].as<int>();
            generator.genLaplacian<DenseMatrix>(nx, matrix);
        }

        int nrows = nx;
        std::vector<double> x(nrows);


        for (std::size_t i = 0; i < nrows; i++)
            x[i] =  i+ 1;

	//Timing for denseMV_Parallel
	{
        Timer::Sentry sentry(timer, "DenseMV_Parallel");
	int rootLocalSize;
        int slaveLocalSize;

        int rest = nx % nb_proc;

        slaveLocalSize = nx / nb_proc;
        rootLocalSize = slaveLocalSize + rest;

        std::vector<double> const &dataVector = matrix.getValues();


	timer.printInfo() ;

        {
            // SEND GLOBAL SIZE
            MPI_Bcast(&nrows, 1, MPI_INT, 0, MPI_COMM_WORLD);

            // SEND MATRIX
            for (int i = 1; i < nb_proc; ++i)
            {
		double const *localDataPtr = dataVector.data() + rootLocalSize * nx + slaveLocalSize * nx * (i-1);
                
		// SEND LOCAL SIZE to PROC I
                MPI_Send(&slaveLocalSize, 1, MPI_INT, i, i * 2, MPI_COMM_WORLD);

                // SEND MATRIX DATA
                MPI_Send(localDataPtr, slaveLocalSize * nrows, MPI_DOUBLE, i, i * 3 + 1, MPI_COMM_WORLD);
            }
        }

        {
            // BROAD CAST VECTOR X
            MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        // COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
        // DenseMatrix local_matrix;
        int local_nrows = rootLocalSize;
        std::vector<double> local_y(local_nrows);
        
	{
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
        }

	} //Ending timing scope for DenseMV_Parallel

	//Baseline sequential implementation       
	{
            std::vector<double> y(nrows);
            {
                Timer::Sentry sentry(timer, "DenseMV");
                matrix.mult(x, y);
            }
            double normy = PPTP::norm2(y);
            std::cout << "||y||=" << normy << std::endl;
        }
	timer.printInfo() ;
    }

    else
    {
        // COMPUTE LOCAL MATRICE LOCAL VECTOR

        // DenseMatrix local_matrix;
        int nrows;
        int local_nrows;

        MPI_Status status;
        std::vector<double> local_vals;

        {
            // RECV GLOBAL SIZE
            MPI_Bcast(&nrows, 1, MPI_INT, 0, MPI_COMM_WORLD);

            // RECV LOCAL SIZE
            MPI_Recv(&local_nrows, 1, MPI_INT, 0, my_rank * 2, MPI_COMM_WORLD, &status);

            // RECV MATRIX DATA
	    local_vals.resize(local_nrows * nrows);
            MPI_Recv(local_vals.data(), local_nrows * nrows, MPI_DOUBLE, 0, my_rank * 3 + 1, MPI_COMM_WORLD, &status);
        }

        std::vector<double> x;
	x.resize(nrows);
        {
            // BROAD CAST VECTOR X
            MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        std::vector<double> local_y(local_nrows);

        {
            // compute parallel SPMV
            double tempSum = 0;

            for (int row = 0; row < local_nrows; row++)
            {
                tempSum = 0;
                for (int col = 0; col < nrows; col++)
                {
                    tempSum += local_vals.at(row * nrows + col) * x.at(col);
                }
                local_y[row] = tempSum;
                local_norm += tempSum * tempSum;
            }

            MPI_Send(local_y.data(), local_nrows, MPI_DOUBLE, 0, my_rank * 4 + 1, MPI_COMM_WORLD);
        }
    }

    MPI_Reduce(&local_norm, &norm, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        std::cout << "Parallel ||y||=" << sqrt(norm) << std::endl;
    }
	
    MPI_Finalize();
    return 0;
}

