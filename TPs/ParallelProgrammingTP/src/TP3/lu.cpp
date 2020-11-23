/*
 * helloworld.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: gratienj
 */

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include "omp.h"
#include "tbb/tbb.h"

#include <string>
#include <vector>
#include <fstream>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/LU>

using namespace tbb;
#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/DenseMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"
#include "Solvers/LUSolver.h"

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

  int nb_procs     = omp_get_num_procs() ;
  std::cout<<"NB PROCS     :"<<nb_procs<<std::endl ;
  int nb_available_threads = omp_get_max_threads() ;
  std::cout<<"NB AVAILABLE_THREADS :"<<nb_available_threads<<std::endl ;

  using namespace PPTP ;

  MatrixGenerator generator ;
  int nx = vm["nx"].as<int>() ;

  if(vm["eigen"].as<int>()==1)
  {
    typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType ;
    typedef Eigen::Matrix<double, Eigen::Dynamic, 1>              EigenVectorType;
    typedef Eigen::PartialPivLU<EigenMatrixType>    EigenSolverType ;


    std::size_t nrows = nx*nx ;
    EigenMatrixType matrix(nrows,nrows) ;

    generator.genLaplacian(nx,matrix) ;

    EigenVectorType x(nrows) ;

    for(std::size_t i=0;i<nrows;++i)
      x(i) = i+1 ;

    EigenVectorType y = matrix*x ;

    double normy = PPTP::norm2(y) ;
    std::cout<<"||y||="<<normy<<std::endl ;

    EigenVectorType x2(nrows) ;
    EigenSolverType lu_solver(matrix) ;
    x2 = lu_solver.solve(y) ;

    EigenVectorType err(nrows) ;
    err = x-x2 ;
    double norm_err = PPTP::norm2(err) ;
    std::cout<<"||x-x2||="<<norm_err<<std::endl ;
  }
  else
  {

    DenseMatrix matrix ;
    if(vm.count("file"))
    {
      std::string file = vm["file"].as<std::string>() ;
      generator.readFromFile(file,matrix) ;
    }
    else
    {
      generator.genLaplacian(nx,matrix) ;
    }


    std::size_t nrows = matrix.nrows();
    std::vector<double> x,y ;
    x.resize(nrows) ;
    y.resize(nrows) ;

    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;

    matrix.mult(x,y) ;

    double normy = PPTP::norm2(y) ;
    std::cout<<"||y||="<<normy<<std::endl ;

    std::vector<double> x2(nrows) ;
    LUSolver<DenseMatrix> lu_solver ;
    lu_solver.init(matrix) ;
    lu_solver.solve(y,x2) ;

    std::vector<double> err(nrows) ;
    std::copy(x.begin(),x.end(),err.begin()) ;
    axpy(-1.,x2,err) ;
    double norm_err = norm2(err) ;
    std::cout<<"||x-x2||="<<norm_err<<std::endl ;
  }

  return 0 ;
}
