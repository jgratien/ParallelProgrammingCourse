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
#include <cmath>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/LU>

#include "MatrixVector/DenseMatrix.h"
#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"

#include "Utils/Timer.h"


// Utiliser make install &> make.log

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

  Timer timer ;
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
      Timer::Sentry sentry(timer,"EigenDenseMV") ;
      y = matrix*x ;
    }

    double normy = PPTP::norm2(y) ;
    std::cout<<"||y||="<<normy<<std::endl ;

  }

  std::vector<size_t> tab_local_sizes(nb_proc);
  std::size_t local_size;

  if(my_rank==0) // MASTER NODE
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


    //matrix.debug();

    std::size_t nrows = matrix.nrows();
    std::vector<double> x;
    x.resize(nrows) ;

    //std::string s("Total matrix at master : " + tostring(matrix.m_values));
    //std::cout << s << std::endl;

    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;

      {
        std::vector<double> y(nrows);
        {
          Timer::Sentry sentry(timer,"DenseMV") ;
          matrix.mult(x,y) ;
        }
        double normy = PPTP::norm2(y) ;
        std::cout<<"Result should be : ||y|| = "<<normy<<std::endl ;
      }

    MPI_Status st;
    std::vector<MPI_Status> vec_st(nb_proc-1);
    std::vector<double> final_y(nrows);

    // SEND GLOBAL SIZE
    MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;


    // BROAD CAST VECTOR X
    //std::string s("Master broadcast x = " + tostring(x));
    //std::cout << s << std::endl;
    MPI_Request req_xbcast;
    MPI_Ibcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD, &req_xbcast) ;

    int size = (int) nrows/nb_proc;
    int reste =  nrows % nb_proc;
    // SEND MATRIX

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

    MPI_Scatter(tab_local_sizes.data(), 1, MPI_UNSIGNED_LONG, &local_size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;

    //std::string s("tab_local_sizes : " + tostring(tab_local_sizes));
    //std::cout << "Local sizes are " << s << std::endl;
    //std::cout << "Master has sent sizes " << std::endl;

    //size_t counter(0);
    //size_t j(0);
    //for (size_t i=0; i < (matrix.size() / nrows); ++i) {
    //  if (i == counter) {
    //    std::cout << "Node " << j << std::endl;
    //    counter += tab_local_sizes[j];
    //    j += 1;
    //  }
    //  matrix.printline(i);
    //}


    double * local_ptr = matrix.data();
    local_ptr += size * nrows;
    int irow = size;
    std::vector<MPI_Request> requests_data(nb_proc-1);
    for (int i=1; i<nb_proc;++i)
    {
      MPI_Isend(local_ptr, nrows * tab_local_sizes[i], MPI_DOUBLE, i, 113, MPI_COMM_WORLD, &requests_data[i-1]) ;
      local_ptr += (nrows * tab_local_sizes[i]);
    }


      //std::cout << "Master has sent all data " << std::endl;




    // COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
    //DenseMatrix local_matrix ;

    // EXTRACT LOCAL DATA FROM MASTER PRO
    local_size = tab_local_sizes[0] ;

    // COMPUTE LOCAL SIZE

    // EXTRACT LOCAL MATRIX DATA






    matrix.resize(local_size, nrows);
    std::vector<double> local_y(nrows);
    {
      Timer::Sentry sentry(timer,"mult Master") ;
      matrix.mult(x, local_y);
      // compute parallel SPMV
      // TIMER HERE
    }


    MPI_Waitall(nb_proc - 1, requests_data.data(), vec_st.data());

    MPI_Wait(&req_xbcast, &st);
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
    //std::string s1("Le vecteur y qu'a récupéré master est : " + tostring(y));
    //std::cout << s1 << std::endl;

    {
      double normy = PPTP::norm2(final_y) ;
      std::cout<<"Result is : ||y||="<<normy<<std::endl ;
    }

  }
  else
  { // ALL NON-MASTER NODES
    // COMPUTE LOCAL MATRICE LOCAL VECTOR

    DenseMatrix local_matrix ;
    std::size_t nrows ;
    //std::size_t local_nrows ;


    // RECV DATA FROM MASTER PROC

    // RECV GLOBAL SIZE
    std::vector<double> x;
    MPI_Request req_xbcast;
    MPI_Status status;
    MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;
    //std::cout << "Node " << my_rank << " received global size from broadcast nrows = " << nrows << std::endl;

    x.resize(nrows) ;

    // BROAD CAST VECTOR X

    double* ptr_matrix_x = x.data();
    MPI_Ibcast(ptr_matrix_x, nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD, &req_xbcast) ;
    //std::string s("Node " + std::to_string(my_rank) + " received x = " + tostring(x));
    //std::cout << s << std::endl;

    // RECV LOCAL SIZE


    MPI_Scatter(tab_local_sizes.data(), 1, MPI_UNSIGNED_LONG, &local_size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;

      //std::cout << "Node " << my_rank << " received local size from send local_size = " << local_size << std::endl;


    local_matrix.init(local_size, nrows);
    double* ptr_matrix_data = local_matrix.data() ;
    MPI_Recv(ptr_matrix_data, local_size * nrows , MPI_DOUBLE, 0, 113, MPI_COMM_WORLD, &status);


    //std::cout << "Reçu par node " << my_rank << std::endl;
    //for (size_t i=0; i < (local_matrix.size() / nrows); ++i) {
    //  local_matrix.printline(i);
    //}

    //std::string s("Total matrix at node " + std::to_string(my_rank) + "(encore) : " + tostring(local_matrix.m_values));
    //std::cout << s << std::endl;


    //std::cout << "Node " << my_rank <<" got all matrix data " << std::endl;




    std::vector<double> local_y(local_size);

    MPI_Wait(&req_xbcast, &status);
    {
      // compute parallel SPMV

      Timer::Sentry sentry(timer,"mult Slave " + std::to_string(my_rank)) ;

      local_matrix.mult(x, local_y);
      // TIMER HERE
      //std::string s2("Vecteur envoyé par node " + std::to_string(my_rank) + " " + tostring(local_y));
      //std::cout << s2 << std::endl;
      //std::string s("Node " + std::to_string(my_rank) + " sending y = " + tostring(local_y));
      //std::cout << s << std::endl;

    }
    MPI_Gatherv(local_y.data(), local_size, MPI_DOUBLE,
                  NULL, NULL, NULL,
                  MPI_DOUBLE, 0, MPI_COMM_WORLD);

  }
  //if (my_rank == 0) {
    timer.printInfo() ;
  //}

  MPI_Finalize();
  return 0 ;
}
