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
std::string tostring(std::vector<T> &x, int delTrailingZeros)
{
  std::string s;
  s += "y(";
  for (unsigned int i=0; i < x.size(); ++i) {
    s += std::to_string(x[i]);

    if (delTrailingZeros == 1) {
      s.erase(s.find_last_not_of('0') + 1, std::string::npos);
    }
    if (i < x.size() - 1) {
      s += "|";
    }
  }
  s += ")";
  return s;
}

using namespace PPTP;


std::vector<int> FirstEltRowDiff(std::vector<int> base) {
  int first_elem = base[0];
  std::vector<int> vecDiff(base.size());
  for (int i=0; i < vecDiff.size(); ++i) {
    vecDiff[i] = base[i] - first_elem;
  }
  return vecDiff;
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
        matrix.mult(x, y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"baseline y ||y||="<<normy<<std::endl ;

      /*{
        Timer::Sentry sentry(timer,"OMPSpMV") ;
        matrix.ompmult(x, y2) ;
      }*/
      double normy2 = PPTP::norm2(y2) ;
      std::cout<<"y with OMP ||y||="<<normy2<<std::endl ;

      std::vector<size_t> tab_local_sizes(nb_proc);

      std::vector<double> final_y(nrows);
      /***************************************************************************
      SPARSE MATRIX VECTOR MPI */
      {
        Timer::Sentry sentry(timer,"SpMV MPI") ;
        // SEND GLOBAL SIZE
        MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;

        int size = (int) nrows / nb_proc;
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
        //MPI_Bcast(tab_local_sizes.data(), nb_proc, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;

        int * row_off = matrix.m_kcol_ptr();

        std::vector<int> tosend_sizes(2);
        // SEND SIZES OF DATA
        int from(row_off[0]); // From which index
        int to(row_off[tab_local_sizes[0]] - 1); // To which index
        std::vector<int> tab_nb_vals(nb_proc); // We have to store the nb_vals
        tab_nb_vals[0] = (to - from + 1); // nb_vals of master
        row_off += tab_local_sizes[0]; // Shift the pointer for node 1
        std::vector<MPI_Request> requests_nbvals(nb_proc-1); // Requests for asynchronous send
        for (int i=1; i<nb_proc;++i)
        {
          from = row_off[0];
          to = row_off[tab_local_sizes[i]] - 1;
          tab_nb_vals[i] = to - from + 1; // Number of values (V and colidx) to send
          tosend_sizes[0] = (int) tab_local_sizes[i];
          tosend_sizes[1] = to - from + 1;
          MPI_Isend(tosend_sizes.data(), 2, MPI_INT, i /* TO NODE i */, i /* TAG OF COMM */, MPI_COMM_WORLD, &requests_nbvals[i-1]);
          row_off += tab_local_sizes[i]; // Shift for next node
        }



        row_off = matrix.m_kcol_ptr();

        std::vector<int> local_rowoff(tab_local_sizes[0] + 1);
        for (int i = 0; i < local_rowoff.size(); ++i) {
          local_rowoff[i] = row_off[i];
        }

        row_off += tab_local_sizes[0] ;
        std::vector<MPI_Request> requests_rowoff(nb_proc-1);

        // SEND ROW OFFSETS
        for (int i=1; i<nb_proc;++i)
        {
          MPI_Isend(row_off, tab_local_sizes[i] + 1, MPI_INT, i, 113, MPI_COMM_WORLD, &requests_rowoff[i-1]) ;

          row_off += tab_local_sizes[i];
        }




        MPI_Status st;
        for(auto r : requests_nbvals) { // Wait for all sizes to be received
            MPI_Wait(&r, &st);
        }

        std::vector<MPI_Request> requests_colidx(nb_proc-1);
        std::vector<MPI_Request> requests_vptr(nb_proc-1);

        int * col_idx = matrix.m_cols_ptr() + tab_nb_vals[0];
        double * V_ptr = matrix.m_values_ptr() + tab_nb_vals[0];
        for (int i = 1; i < nb_proc; ++i) {
          MPI_Isend(col_idx, tab_nb_vals[i], MPI_INT, i /* TO NODE i */, 1000 + i /* TAG OF COMM */, MPI_COMM_WORLD, &requests_colidx[i-1]);
          MPI_Isend(V_ptr, tab_nb_vals[i], MPI_DOUBLE, i /* TO NODE i */, 666 + i /* TAG OF COMM */, MPI_COMM_WORLD, &requests_vptr[i-1]);
          col_idx += tab_nb_vals[i]; // Shift for next node
          V_ptr += tab_nb_vals[i]; // Shift for next node
        }

        for(auto r : requests_rowoff) { // Wait row_idx to be received
            MPI_Wait(&r, &st);
        }
        for(auto r : requests_colidx) { // Wait column indexes to be received
            MPI_Wait(&r, &st);
        }
        for(auto r : requests_vptr) { // Wait values to be received
            MPI_Wait(&r, &st);
        }

        {
          // BROAD CAST VECTOR X
          MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
        }

        matrix.keepFirstNRows(tab_local_sizes[0]);
        std::vector<double> local_y(nrows);
        {
          matrix.mult(x, local_y);
          // compute parallel SPMV
          // TIMER HERE
        }


        {
          // RECONSTRUCT Y
          std::vector<int> displacements(nb_proc);
          std::vector<int> int_tab_local_sizes(nb_proc);
          displacements[0] = 0;
          int_tab_local_sizes[0] = (int) tab_local_sizes[0];
          for (int i = 1; i < nb_proc; ++i) {
            displacements[i] = (int) (displacements[i-1] + tab_local_sizes[i-1]);
            int_tab_local_sizes[i] = (int) tab_local_sizes[i];
          }
          MPI_Gatherv(local_y.data(), tab_local_sizes[0], MPI_DOUBLE,
                    final_y.data(), int_tab_local_sizes.data(), displacements.data(),
                    MPI_DOUBLE, 0, MPI_COMM_WORLD);

          //std::string s("Master gathered y = " + tostring(y));
          //std::cout << s << std::endl;
        }


        }


        //matrix.mult(x, y3);
        double normy3 = PPTP::norm2(final_y) ;
        std::cout << "y with MPI : ||y||=" << normy3 << std::endl ;

      /***************************************************************************
      FIN SPARSE MATRIX VECTOR MPI */
    }
    else { // ALL NON-MASTER NODES //
      MPI_Status status;
      std::size_t nrows;
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;

      // RECV LOCAL SIZE
      //std::vector<size_t> tab_local_sizes(nb_proc);
      //MPI_Bcast(tab_local_sizes.data(), nb_proc, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;


      std::vector<int> local_sizes(2);
      int nb_vals{0};
      size_t local_size;
      MPI_Recv(local_sizes.data(), 2, MPI_INT, 0, my_rank, MPI_COMM_WORLD, &status);

      local_size = local_sizes[0];
      nb_vals = (int) local_sizes[1];

      // RECV ROW_OFF
      std::vector<int> vec_m_kcol(local_size + 1);
      int* row_off = vec_m_kcol.data();
      MPI_Recv(row_off, local_size + 1, MPI_INT, 0, 113, MPI_COMM_WORLD, &status);

      std::vector<int> diffVect = FirstEltRowDiff(vec_m_kcol);



      std::vector<int> vec_m_cols(nb_vals);
      MPI_Recv(vec_m_cols.data(), nb_vals, MPI_INT, 0, 1000 + my_rank, MPI_COMM_WORLD, &status);

      std::vector<double> vec_m_values(nb_vals);
      MPI_Recv(vec_m_values.data(), nb_vals, MPI_DOUBLE, 0, 666 + my_rank, MPI_COMM_WORLD, &status);

      std::vector<double> x;
      x.resize(nrows) ;
      {
        // BROAD CAST VECTOR X
        double* ptr_matrix_x = x.data();
        MPI_Bcast(ptr_matrix_x, nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
      }

      CSRMatrix local_matrix(diffVect, vec_m_cols, vec_m_values);

      std::vector<double> local_y(local_size);
      {
        // compute parallel SPMV
        local_matrix.mult(x, local_y);
        // TIMER HERE
        //std::string s2("Vecteur envoyé par node " + std::to_string(my_rank) + " " + tostring(local_y));
        //std::cout << s2 << std::endl;

        MPI_Gatherv(local_y.data(), local_size, MPI_DOUBLE,
                  NULL, NULL, NULL,
                  MPI_DOUBLE, 0, MPI_COMM_WORLD);
      }
    }
  }
  if (my_rank == 0) {
    timer.printInfo();
  }
  MPI_Finalize();
  return 0 ;

}
