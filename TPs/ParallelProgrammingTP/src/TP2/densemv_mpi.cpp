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

int main(int argc, char ** argv) {
    using namespace boost::program_options;
    options_description desc;
    desc.add_options()
        ("help", "produce help")
        ("nrows", value < int > () -> default_value(0), "matrix size")
        ("nx", value < int > () -> default_value(0), "nx grid size")
        ("file", value < std::string > (), "file input")
        ("eigen", value < int > () -> default_value(0), "use eigen package");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    MPI_Init( & argc, & argv);

    int my_rank = 0;
    int nb_proc = 1;
    MPI_Comm_size(MPI_COMM_WORLD, & nb_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, & my_rank);

    using namespace PPTP;

    Timer timer;
    MatrixGenerator generator;
    int nx = vm["nx"].as < int > ();
    if (vm["eigen"].as < int > () == 1) {
        typedef Eigen::Matrix < double, Eigen::Dynamic, Eigen::Dynamic > EigenMatrixType;
        typedef Eigen::Matrix < double, Eigen::Dynamic, 1 > EigenVectorType;

        std::size_t nrows = nx * nx;
        EigenMatrixType matrix(nrows, nrows);

        generator.genLaplacian(nx, matrix);

        EigenVectorType x(nrows);

        for (std::size_t i = 0; i < nrows; ++i)
            x(i) = i + 1;

        EigenVectorType y; {
            Timer::Sentry sentry(timer, "EigenDenseMV");
            y = matrix * x;
        }

        double normy = PPTP::norm2(y);
        std::cout << "||y||=" << normy << std::endl;

    }

    if (my_rank == 0) {
		
		Timer::Sentry sentry(timer, "DenseMVMPI");
        DenseMatrix matrix;
		std::size_t local_nrows;
		
        if (vm.count("file")) {
            std::string file = vm["file"].as < std::string > ();
            generator.readFromFile(file, matrix);
        } else {
            int nx = vm["nx"].as < int > ();
            generator.genLaplacian(nx, matrix);
        }

        std::size_t nrows = matrix.nrows();
        std::vector < double > x;
        x.resize(nrows);
        for (std::size_t i = 0; i < nrows; ++i) {
            x[i] = i + 1;
        }

        /*for (size_t i = 0; i < nrows; ++i) {
			std::cout << (i+1) << "]";
            for (size_t j = 0; j < nrows; ++j) {
				std::cout << " " << matrix(i, j);
			}
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
		*/
		double* toSend;
        {

            // SEND GLOBAL SIZE
            MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
            // SEND LOCAL SIZE to PROC I
            toSend = matrix.data();
			/*std::cout << "RANK:" << my_rank << "\tLOCAL ROWS: " << local_nrows << "\tSIZE: " << (local_nrows * nrows) << std::endl;
			for (size_t i = 0; i < local_nrows; ++i) {
				std::cout << (i+1) << "]";
				for (size_t j = 0; j < nrows; ++j) {
					std::cout << " " << matrix(i, j);
				}
				std::cout << std::endl;
			}
			std::cout << std::endl << std::endl;
			*/
            for (int i = 1; i < nb_proc; ++i) {
                std::cout << " SEND MATRIX DATA to proc " << i << std::endl;

                local_nrows = nrows / nb_proc;
                MPI_Send(&local_nrows, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);

                // SEND MATRIX DATA
                int count = (local_nrows * nrows);
                MPI_Send((toSend + i * count), count, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);

            }
        }
	

        {
            MPI_Bcast( & x[0], nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        {
            std::vector < double > y(nrows);
			{
                Timer::Sentry sentry(timer, "DenseMV");
                matrix.mult(x, y);
            }
            double normy = PPTP::norm2(y);
            std::cout << "||y||=" << normy << std::endl;
        }

        // COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
		int modulo = nrows % nb_proc;
		local_nrows = modulo + (nrows / nb_proc);
        DenseMatrix local_matrix;
		std::vector < double > local_y(local_nrows);
		
		{
            // EXTRACT LOCAL DATA FROM MASTER PROC
			local_matrix.init(local_nrows, nrows);
			for(size_t i=0; i<local_nrows; ++i) {
				for (size_t j=0; j<nrows; ++j) {
					local_matrix(i, j) = toSend[i * nrows + j];
				}
			}
			
			local_matrix.mult(x, local_y);

            // EXTRACT LOCAL MATRIX DATA
			std::vector<double> toRecv(nrows);
			// std::copy(local_y.begin(), local_y.end(), std::back_inserter(toRecv));
			for (int i=0; i<local_y.size(); i++) {
				toRecv[i] = local_y[i];
			}
			
			for (int i=1; i<nb_proc; ++i) {
				MPI_Recv(toRecv.data() + (i * local_nrows), local_nrows, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}

			double normy = PPTP::norm2(toRecv);
            std::cout << "||y2||=" << normy << std::endl;
        }

        {
	
        /*for (size_t i = 0; i < local_nrows; ++i) {
		std::cout << (i+1) << "]";
                std::cout << " " << local_y(i);
            	std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;*/
		// compute parallel SPMV
        }
    } else {
        // COMPUTE LOCAL MATRICE LOCAL VECTOR
        DenseMatrix local_matrix;
        std::size_t nrows;
        std::size_t local_nrows;

        {
            // RECV DATA FROM MASTER PROC
            // RECV GLOBAL SIZE
            MPI_Bcast( & nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
            // RECV LOCAL SIZE
            MPI_Recv( & local_nrows, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // RECV MATRIX DATA
            local_matrix.init(local_nrows, nrows);
            int count = local_nrows * nrows;
            double * toRecv = new double[count];
            MPI_Recv(toRecv, count, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            size_t c = 0;
            for (size_t i = 0; i < local_nrows; ++i)
                for (size_t j = 0; j < nrows; ++j)
                    local_matrix(i, j) = toRecv[c++];
            delete toRecv;
            //std::cout << "RANK:" << my_rank << "\tLOCAL ROWS: " << local_nrows << "\tSIZE: " << c << std::endl;

        }
		/*
        for (size_t i = 0; i < local_nrows; ++i) {
			std::cout << (i+1) << "]";
            for (size_t j = 0; j < nrows; ++j)
                std::cout << " " << local_matrix(i, j);
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
		*/
        std::vector < double > x; {
            x.resize(nrows);
            MPI_Bcast( & x[0], nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        std::vector < double > local_y(local_nrows);
		{
            // compute parallel SPMV
			local_matrix.mult(x, local_y);
			
			MPI_Send(local_y.data(), local_nrows, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

			/*for (size_t i = 0; i < local_nrows; ++i) {
		std::cout << (i+1) << "]";
                std::cout << " " << local_y(i);
            	std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;*/
        }

    }
    timer.printInfo();

    MPI_Finalize();
    return 0;
}
