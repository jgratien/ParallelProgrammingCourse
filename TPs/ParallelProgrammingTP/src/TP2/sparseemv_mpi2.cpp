#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <mpi.h>

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

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);
  int rank = 0;
  int size = 1;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  using namespace boost::program_options;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
      ("nrows", value<int>()->default_value(0), "matrix size")
      ("nx", value<int>()->default_value(0), "nx grid size")
      ("file", value<std::string>(), "file input")
      ("eigen", value<int>()->default_value(0), "use eigen package");
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help"))
  {
      std::cout << desc << "\n";
      MPI_Finalize();
      return 1;
  }
  using namespace PPTP;

  Timer timer;
  MatrixGenerator generator;
  if (vm["eigen"].as<int>() == 1)
  {
    typedef Eigen::SparseMatrix<double> MatrixType;
    typedef Eigen::VectorXd VectorType;
    MatrixType matrix;
    if (vm.count("file"))
    {
      std::string file = vm["file"].as<std::string>();
      generator.readFromFile(file, matrix);
    }
    else
    {
      int nx = vm["nx"].as<int>();
      generator.genLaplacian(nx, matrix);
    }

    std::size_t nrows = matrix.rows();
    VectorType x(nrows);

    for (std::size_t i = 0; i < nrows; ++i)
      x(i) = i + 1;

    VectorType y;
    {
      Timer::Sentry sentry(timer, "EigenSpMV");
      y = matrix * x;
    }

    double normy = PPTP::norm2(y);
    if (rank == 0) {
      std::cout << "||y|| = " << normy << std::endl;
    }
  }

  CSRMatrix matrix;
  if (vm.count("file"))
  {
    std::string file = vm["file"].as<std::string>();
    generator.readFromFile(file, matrix);
  }
  else
  {
    int nx = vm["nx"].as<int>();
    generator.genLaplacian(nx, matrix);
  }

  std::size_t nrows = matrix.nrows();
  std::vector<double> x, y, y2;
  x.resize(nrows);
  y.resize(nrows);
  y2.resize(nrows);

  int *sendcounts = new int[size];
  int *displs = new int[size];
  int local_nrows = nrows / size;
  int remainder = nrows % size;

  for (int i = 0; i < size; ++i)
  {
      sendcounts[i] = local_nrows + (i < remainder ? 1 : 0);
      displs[i] = (i > 0) ? displs[i - 1] + sendcounts[i - 1] : 0;
  }

  int local_start_row = displs[rank];
  int local_end_row = local_start_row + sendcounts[rank];

  std::vector<double> local_x(sendcounts[rank]);
  std::vector<double> local_y(sendcounts[rank]);

  for (std::size_t i = 0; i < nrows; ++i)
    x[i] = i + 1;

  {
     Timer::Sentry sentry(timer,"SpMV") ;
     matrix.mult(x,y) ;
  }
  if (rank == 0){
     double normy = PPTP::norm2(y) ;
     std::cout<<"||y||="<<normy<<std::endl ;
  }

  MPI_Scatterv(x.data(), sendcounts, displs, MPI_DOUBLE, local_x.data(), sendcounts[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

  {
    Timer::Sentry sentry(timer, "MPISpMV");
    for (int i = local_start_row; i < local_end_row; ++i)
    {
      double value = 0;
      for (int k = matrix.m_kcol[i]; k < matrix.m_kcol[i + 1]; ++k)
      {
          value += matrix.m_values[k] * x[matrix.m_cols[k]];
      }
      local_y[i - local_start_row] = value;
    }
  }

  MPI_Gatherv(local_y.data(), sendcounts[rank], MPI_DOUBLE, y2.data(), sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (rank == 0)
  {
      double normy2 = PPTP::norm2(y2);
      std::cout << "||y2|| = " << normy2 << "       NP:" << size << "     NX:" << vm["nx"].as<int>() <<std::endl;
  }

  delete[] sendcounts;
  delete[] displs;

  if (rank == 0) {
    timer.printInfo();
  }

  MPI_Finalize();
  return 0;
}
