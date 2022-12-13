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
  using namespace boost::program_options ;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
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
  MPI_Init(&argc,&argv) ;

  int my_rank = 0 ;
  int nb_proc = 1 ;
  MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;

  using namespace PPTP ;

  Timer timer_seq, timer_mpi, timer_mpi_debug;
  MatrixGenerator generator ;
  int nx = vm["nx"].as<int>() ;
  if(vm["eigen"].as<int>()==1)
  {
    typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType ;
    typedef Eigen::Matrix<double, Eigen::Dynamic, 1>              EigenVectorType;


    std::size_t nrows = nx*nx ;
    EigenMatrixType matrix(nrows,nrows) ;

    generator.genLaplacian(nx,matrix) ;

    EigenVectorType x(nrows) ;

    for(std::size_t i=0;i<nrows;++i)
      x(i) = i+1 ;

    EigenVectorType y ;
    {
      // Timer::Sentry sentry(timer,"EigenDenseMV") ;
      y = matrix*x ;
    }

    double normy = PPTP::norm2(y) ;
    std::cout<<"||y||="<<normy<<std::endl;

  }

  if(my_rank==0)
  {
    DenseMatrix matrix ;

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


    std::size_t nrows = matrix.nrows();
    std::vector<double> x;
    x.resize(nrows) ;

    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;

    std::vector<double> y(nrows), z(nrows);
    {
        // SEQUENTIAL MATRIX MULTIPLICATION
        Timer::Sentry sentry(timer_seq, "Sequential SparseMV");
        matrix.mult(x, y);
    }
    double normy = PPTP::norm2(y);
    std::cout << "||y_SEQ||=" << normy << std::endl;

    {
        // MPI DENSE MATRIX MULTIPLICATION
        MPI_Status status; MPI_Request request;

        double const* matrix_ptr = matrix.data();
        std::size_t q = nrows / nb_proc; std::size_t r = nrows % nb_proc;

        std::vector<int> displs(nb_proc); std::vector<int> recvcounts(nb_proc);
        std::vector<int> displs_matrix(nb_proc); std::vector<int> recvcounts_matrix(nb_proc);

        displs[0] = 0; recvcounts[0] = q;
        displs_matrix[0] = 0; recvcounts_matrix[0] = q*nrows;

        for (int destination = 1; destination < nb_proc; ++destination)
        {
            // DISPLACEMENTS & COUNTS FOR EACH PROC
            std::size_t local_nrows = q; if (destination <= r) local_nrows++;
            displs[destination] = displs[destination - 1] + recvcounts[destination - 1]; recvcounts[destination] = local_nrows;
            displs_matrix[destination] = displs_matrix[destination-1] + recvcounts_matrix[destination-1]; recvcounts_matrix[destination] = local_nrows*nrows;
        }

        // BCAST GLOBAL & LOCAL SIZE
        MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
        MPI_Bcast(&q, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
        MPI_Bcast(&r, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

        // BCAST X
        MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        {
            Timer::Sentry sentry(timer_mpi, "MPI DenseMV");

            std::size_t local_nrows(recvcounts[my_rank]);
            std::vector<double> local_values(local_nrows * nrows);

            // SCATTER DENSE MATRIX DATA
            MPI_Scatterv(matrix_ptr, recvcounts_matrix.data(), displs_matrix.data(), MPI_DOUBLE, local_values.data(), local_nrows* nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

            // PROC 0
            std::vector<double> local_y(recvcounts[my_rank]);
            mult(x, local_y, recvcounts[my_rank], nrows, local_values);

            // GATHER
            MPI_Gatherv(local_y.data(), recvcounts[my_rank], MPI_DOUBLE, z.data(), recvcounts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        double normz = PPTP::norm2(z);
        std::cout << "||y_MPI||=" << normz << std::endl;
    }
    
    // TIME OUTPUT
    std::ofstream outfile;
    outfile.open("time.txt", std::ios_base::app);
    if (nb_proc == 1 && nx == 50)
    {
        outfile << "nx	" << "np	" << "Time_MPI	" << "Time_SEQ	" << "Speed_Up" << std::endl;
    }
    outfile << nx << "	" << nb_proc << "	" << timer_mpi.sum_time() << "	" << timer_seq.sum_time() << "	" << timer_seq.sum_time() / timer_mpi.sum_time() << std::endl;

  }
  else
  {
    // SLAVE PROC
    MPI_Status status; MPI_Request request;
    std::size_t nrows, local_nrows, r;
    int source = 0;

    // RECEIVE GLOBAL & LOCAL SIZE
    MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&local_nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&r, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    if (my_rank <= r) local_nrows++;

    // RECEIVE X
    std::vector<double> x(nrows);
    MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // RECEIVE DENSE MATRIX DATA
    std::vector<double> local_values(nrows*local_nrows);
    MPI_Scatterv(NULL, NULL, NULL, MPI_DOUBLE, local_values.data(), nrows*local_nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // COMPUTE LOCAL PRODUCT
    std::vector<double> local_y(local_nrows);
    mult(x, local_y, local_nrows, nrows, local_values);

    // GATHER DATA
    MPI_Gatherv(local_y.data(), local_nrows, MPI_DOUBLE, NULL, NULL, NULL, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  }
  
  MPI_Finalize() ;
  
  return 0 ;
}