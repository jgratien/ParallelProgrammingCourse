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
      ("nb-threads",value<int>()->default_value(0), "nb threads")
      ("nrows",value<int>()->default_value(0), "matrix size")
      ("nx",value<int>()->default_value(0), "nx grid size")
      ("chunk-size",value<int>()->default_value(1), "chunk size")
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
  {
    omp_set_num_threads(nb_threads) ;
    tbb::task_scheduler_init init(nb_threads);
  }
  int nb_procs     = omp_get_num_procs() ;
  std::cout<<"NB PROCS     :"<<nb_procs<<std::endl ;
  int nb_available_threads = omp_get_max_threads() ;
  std::cout<<"NB AVAILABLE_THREADS :"<<nb_available_threads<<std::endl ;

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
      int nx = vm["nx"].as<int>() ;
      generator.genLaplacian<DenseMatrix>(nx,matrix) ;
    }


    std::size_t nrows = matrix.nrows();
    std::vector<double> x;
    x.resize(nrows) ;

    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;

    {
      std::vector<double> y(nrows);
      {
        Timer::Sentry sentry(timer,"DenseMV") ;
        matrix.mult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"||y||="<<normy<<std::endl ;
    }

    matrix.setChunkSize(vm["chunk-size"].as<int>()) ;
    {
      std::vector<double> y(nrows);
      {
        Timer::Sentry sentry(timer,"OMPDenseMV") ;
        matrix.ompmult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"OMP ||y||="<<normy<<std::endl ;
    }

    {
      std::vector<double> y(nrows);
      {
        Timer::Sentry sentry(timer,"OMPTaskDenseMV") ;
        matrix.omptaskmult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"OMPTask ||y||="<<normy<<std::endl ;
    }

    {
      std::vector<double> y(nrows);
      {
        Timer::Sentry sentry(timer,"OMPTileDenseMV") ;
        matrix.omptilemult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"OMPTile ||y||="<<normy<<std::endl ;
    }

    {
      std::vector<double> y(nrows);
      {
        Timer::Sentry sentry(timer,"TBBDenseMV") ;
        matrix.tbbmult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"TBB ||y||="<<normy<<std::endl ;
    }


    {
      std::vector<double> y(nrows);
      {
        Timer::Sentry sentry(timer,"TBBRangeDenseMV") ;
        matrix.tbbrangemult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"TBBRange ||y||="<<normy<<std::endl ;
    }

    {
      std::vector<double> y(nrows);
      {
        Timer::Sentry sentry(timer,"TBBRange2DDenseMV") ;
        matrix.tbbrange2dmult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"TBBRange2DTile ||y||="<<normy<<std::endl ;
    }
  }
  timer.printInfo() ;
  return 0 ;
}
