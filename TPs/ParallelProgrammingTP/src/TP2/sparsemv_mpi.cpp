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
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"
#include "Utils/Timer.h"
#include "omp.h"
#include "tbb/tbb.h"

using vi = std::vector<int>;
using vd = std::vector<double>;

int count(const int* vector_ptr, const int& n){
  int nb_elems = 0;
  for(int i = 0; i < n; i++){
    nb_elems += *(vector_ptr + i);
  }
  return nb_elems;
}

vd mult(const vd& x, const vi& rows, const vi& cols, const vd& values){
  int nb_elems = 0;
  vd y;
  for(int row : rows){
    double value = 0;
    for(int i = 0; i < row; i++){
      value += x[cols[nb_elems + i]] * values[nb_elems + i];
    }
    y.push_back(value);
    nb_elems += row;
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

  std::vector<double> y_final;
  std::vector<double> y_local;

  if(my_rank==0){
    CSRMatrix matrix;
    int nx = vm["nx"].as<int>();
    generator.genLaplacian(nx, matrix);
    // std::cout << "m_kcol : " << matrix.rows().size() << '\n';
    // for(auto e : matrix.rows()){
    //   std::cout << "e : " << e << '\n';
    // }
    // std::cout << "m_cols : " << matrix.cols().size() << '\n';
    // std::cout << "m_values : " << matrix.values().size() << '\n';
    // std::cout << "m_nrows : " << matrix.nrows() << '\n';
    // ====================================================


    int nrows = matrix.nrows();
    std::vector<double> x;
    std::vector<double> y;
    std::vector<int> positions;
    std::vector<int> sizes;
    x.resize(nrows);
    y.resize(nrows);
    positions.resize(nb_proc);
    sizes.resize(nb_proc);

    for(int i = 0; i < nrows; ++i) x[i] = i + 1;

    {
      Timer::Sentry sentry(timer,"SpMV");
      matrix.mult(x,y) ;
    }
    double normy = PPTP::norm2(y) ;
    std::cout << "||y||=" << normy << std::endl;

    {
      Timer::Sentry sentry(timer,"SparseMPI");
      MPI_Bcast(&nrows, 1, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Bcast(x.data(), x.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

      std::vector<int> rows = matrix.rows();
      std::vector<int> cols = matrix.cols();
      std::vector<double> values = matrix.values();
      for (int i = rows.size() - 1; i > 0; i--) rows[i] -= rows[i - 1];

      int q = nrows / nb_proc;
      int r = nrows % nb_proc;

      // std::cout << "q : " << q << '\n';
      // std::cout << "r : " << r << '\n';

      int* prows = rows.data() + 1; // the first element is not interesting
      int* pcols = cols.data();
      double* pvalues = values.data();

      // vi trows;
      // trows.insert(trows.end(), rows.begin() + 1, rows.end());
      // vd temp = mult(x, trows, cols, values);
      // std::cout << "temp size : " << temp.size() << '\n';
      // std::cout << "nrows : " << nrows << '\n';
      // double rrr = 0;
      // for (auto e : temp) rrr += e * e;
      // std::cout << "rrr||y||=" << std::sqrt(rrr) << '\n';

      int epsilon = 0;
      if (r > 0) epsilon = 1;
      int local_size = (q + epsilon);
      int local_nb_elems = count(prows, local_size);
      // std::cout << "my_rank : " << my_rank << ", local_size : " << local_size << '\n';
      int* local_cptr = pcols + local_nb_elems;
      double* local_vptr = pvalues + local_nb_elems;
      // std::cout << "my_rank : " << my_rank << ", nb_elems : " << local_nb_elems << '\n';
      int* local_rptr = prows + local_size;

      int position = 0;
      positions[0] = position;
      sizes[0] = local_size;

      for (int i = 1; i < nb_proc; i++){
        int proc_local_size = q;
        if (i < r) proc_local_size++;
        MPI_Send(&proc_local_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(local_rptr, proc_local_size, MPI_INT, i, 1, MPI_COMM_WORLD);

        int nb_elems = count(local_rptr, proc_local_size);
        MPI_Send(&nb_elems, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
        MPI_Send(local_cptr, nb_elems, MPI_INT, i, 3, MPI_COMM_WORLD);
        MPI_Send(local_vptr, nb_elems, MPI_DOUBLE, i, 4, MPI_COMM_WORLD);
        local_cptr += nb_elems;
        local_vptr += nb_elems;
        local_rptr += proc_local_size;
        position += proc_local_size;
        sizes[i] = proc_local_size;
        positions[i] = position;
      }

      std::vector<int> local_rows;
      std::vector<int> local_cols;
      std::vector<double> local_values;
      local_rows.insert(local_rows.end(), prows, prows + local_size);
      local_cols.insert(local_cols.end(), pcols, pcols + local_nb_elems);
      local_values.insert(local_values.end(), pvalues, pvalues + local_nb_elems);


      y_local = mult(x, local_rows, local_cols, local_values);

      y_final.resize(nrows);
      MPI_Gatherv(y_local.data(), local_size, MPI_DOUBLE, y_final.data(), sizes.data(), positions.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    normy = PPTP::norm2(y_final);
    std::cout << "||y||=" << normy << std::endl;

  } else {
    int nrows;
    int local_size;
    int nb_elems;
    std::vector<double> x;
    std::vector<int> local_rows;
    std::vector<int> local_cols;
    std::vector<double> local_values;

    MPI_Bcast(&nrows, 1, MPI_INT, 0, MPI_COMM_WORLD);

    x.resize(nrows);
    MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Recv(&local_size, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // std::cout << "my_rank : " << my_rank << ", local_size : " << local_size << '\n';

    local_rows.resize(local_size);
    int* prows = local_rows.data();
    MPI_Recv(prows, local_size, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // for(auto e : local_rows){
    //   std::cout << "my_rank : " << my_rank << ", e : " << e << '\n';
    // }

    MPI_Recv(&nb_elems, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // std::cout << "my_rank : " << my_rank << ", nb_elems : " << nb_elems << '\n';

    local_cols.resize(nb_elems);
    int* pcols = local_cols.data();
    MPI_Recv(pcols, nb_elems, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // for(auto e1 : local_cols){
    //   std::cout << "my_rank : " << my_rank << ", e1 : " << e1 << '\n';
    // }

    local_values.resize(nb_elems);
    double* pvalues = local_values.data();
    MPI_Recv(pvalues, nb_elems, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // for(auto e2 : local_values){
    //   std::cout << "my_rank : " << my_rank << ", e2 : " << e2 << '\n';
    // }

    y_local = mult(x, local_rows, local_cols, local_values);

    MPI_Gatherv(y_local.data(), local_size, MPI_DOUBLE, NULL, NULL, NULL, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  }

  timer.printInfo();
  MPI_Finalize();
  return 0;
}
