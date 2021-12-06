/*
 * spmv_mpi.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: arthurviens
 */

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include "omp.h"
#include "tbb/tbb.h"
#include "mpi.h"

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

template <typename T>
std::string tostring(std::vector<T> &x)
{
  std::string s;
  s += "y(";
  for (unsigned int i=0; i < x.size(); ++i) {
    s += std::to_string(x[i]);
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);
    if (i < x.size() - 1) {
      s += "|";
    }
  }
  s += ")";
  return s;
}

int main(int argc, char** argv)
{
  using namespace boost::program_options ;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
      ("nb-threads",value<int>()->default_value(0), "nb threads")
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

  int nb_threads = vm["nb-threads"].as<int>() ;
  if(nb_threads>0)
    omp_set_num_threads(nb_threads) ;

  MPI_Init(&argc,&argv) ;

  int my_rank = 0 ;
  int nb_proc = 1 ;
  MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;

  if (my_rank == 0) {
    int nb_procs     = omp_get_num_procs() ;
    std::cout<<"NB PROCS     :"<<nb_procs<<std::endl ;
    int nb_available_threads = omp_get_max_threads() ;
    std::cout<<"NB AVAILABLE_THREADS :"<<nb_available_threads<<std::endl ;
  }

  using namespace PPTP ;

  Timer timer ;
  MatrixGenerator generator ;
  if(vm["eigen"].as<int>()==1)
  { /* unused */
    typedef Eigen::SparseMatrix<double> MatrixType ;
    typedef Eigen::VectorXd             VectorType ;
    MatrixType matrix ;
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


    std::size_t nrows = matrix.rows();
    VectorType x(nrows);

    for(std::size_t i=0;i<nrows;++i)
      x(i) = i+1 ;

    VectorType y ;
    {
      Timer::Sentry sentry(timer,"EigenSpMV") ;
       y = matrix*x ;
    }

    double normy = PPTP::norm2(y) ;
    std::cout<<"||y||="<<normy<<std::endl ;
  } /* ununsed */
  else
  {
    std::vector<size_t> tab_local_sizes(nb_proc);

    if (my_rank == 0) { // MASTER NODE
      CSRMatrix matrix ;
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
      std::vector<double> x, y, y2, y3 ;
      x.resize(nrows) ;
      y.resize(nrows) ;
      y2.resize(nrows) ;
      y3.resize(nrows) ;

      for(std::size_t i=0;i<nrows;++i)
        x[i] = i+1 ;
      {
        Timer::Sentry sentry(timer,"SpMV") ;
        matrix.mult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"||y||="<<normy<<std::endl ;

      {
        Timer::Sentry sentry(timer,"OMPSpMV") ;
        matrix.mult(x,y2) ;
      }
      double normy2 = PPTP::norm2(y2) ;
      std::cout<<"||y2||="<<normy2<<std::endl ;

      /***************************************************************************
      SPARSE MATRIX VECTOR MPI */
      std::cout << "Master has a matrix with nnz = " << matrix.nnz() << std::endl;

      // SEND GLOBAL SIZE
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;

      int size = (int) nrows/nb_proc;
      int reste =  nrows % nb_proc;
      size_t local_size;

      tab_local_sizes[0] = (size_t) size;
      for (int i=1; i<nb_proc;++i)
      {
        local_size = size;
        if (reste > 0) {
          reste -= 1;
          local_size += 1;
        }
        tab_local_sizes[i] = local_size;
      }

      // SEND LOCAL SIZE
      MPI_Bcast(tab_local_sizes.data(), nb_proc, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;
      std::string s("tab_local_sizes sent by master : " + tostring(tab_local_sizes));
      std::cout << s << std::endl;

      double * V_ptr = matrix.m_values_ptr();
      int * col_idx = matrix.m_cols_ptr();
      int * row_off = matrix.m_kcol_ptr();

      row_off += tab_local_sizes[0] ;
      std::vector<MPI_Request> requests_rowoff(nb_proc-1);
      std::vector<MPI_Request> requests_colidx(nb_proc-1);
      std::vector<MPI_Request> requests_vptr(nb_proc-1);

      // SEND ROW OFFSETS
      for (int i=1; i<nb_proc;++i)
      {
        if (i < nb_proc - 1) {
          MPI_Isend(row_off, tab_local_sizes[i] + 1, MPI_INT, i, 113, MPI_COMM_WORLD, &requests_rowoff[i-1]) ;
        } else {
          MPI_Isend(row_off, tab_local_sizes[i], MPI_INT, i, 113, MPI_COMM_WORLD, &requests_rowoff[i-1]) ;
        }

        row_off += tab_local_sizes[i];
      }

      // SEND DATA AND COLUMNS
      row_off = matrix.m_kcol_ptr();
      row_off += tab_local_sizes[0] ;
      int from{row_off[0]};
      int to{row_off[0]};
      int nb_vals{0};
      for (int i=1; i<nb_proc;++i)
      {
        from = row_off[0];
        to = row_off[tab_local_sizes[i] - 1];
        nb_vals = to - from;
        MPI_Send(&nb_vals, 1, MPI_INT, i /* TO NODE i */, i /* TAG OF COMM */, MPI_COMM_WORLD);
        std::cout << "Master sent nb_vals = " << nb_vals << " to node " << i << std::endl;
        row_off += tab_local_sizes[i];
      }


      {
        Timer::Sentry sentry(timer,"SpMV MPI") ;
        matrix.mult(x, y3) ;
        double normy3 = PPTP::norm2(y3) ;
        std::cout<<"||y3||="<<normy3<<std::endl ;
      }

      /***************************************************************************
      FIN SPARSE MATRIX VECTOR MPI */
    }
    else { // ALL NON-MASTER NODES //
      std::size_t nrows;
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;
      std::cout << "Node " << my_rank << " received global size from broadcast nrows = " << nrows << std::endl;

      // RECV LOCAL SIZE
      MPI_Bcast(tab_local_sizes.data(), nb_proc, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;
      std::string s("tab_local_sizes recovered by " + std::to_string(my_rank) + " : "+ tostring(tab_local_sizes));
      std::cout << s << std::endl;


      MPI_Status status;
      // RECV ROW_OFF
      std::vector<int> vec_m_kcol(tab_local_sizes[my_rank] + 1);
      int* row_off = vec_m_kcol.data();
      if (my_rank == nb_proc - 1) {
        MPI_Recv(row_off, tab_local_sizes[my_rank], MPI_INT, 0, 113, MPI_COMM_WORLD, &status);
      } else {
        MPI_Recv(row_off, tab_local_sizes[my_rank] + 1, MPI_INT, 0, 113, MPI_COMM_WORLD, &status);
      }
      std::string s2("row_off recovered by " + std::to_string(my_rank) + " : "+ tostring(vec_m_kcol));
      std::cout << s2 << std::endl;

      int nb_vals{0};
      MPI_Recv(&nb_vals, 1, MPI_INT, 0, my_rank, MPI_COMM_WORLD, &status);
      std::cout << my_rank << " received nb vals = " << nb_vals << std::endl;
    }
  }
  timer.printInfo();
  MPI_Finalize();
  return 0 ;

}
