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
#include "omp.h"

#include "Utils/Timer.h"

int main(int argc, char** argv)
{
  using namespace boost::program_options ;
  options_description desc;
  desc.add_options()
      ("help", "produce help") ;
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help"))
  {
      std::cout << desc << "\n";
      return 1;
  }

  // Initialize MPI
  MPI_Init(&argc,&argv) ;


  PPTP::Timer timer ;
  {
    PPTP::Timer::Sentry sentry(timer,"HelloWord") ;


    //#pragma omp ....CREATE PARALLEL SECTION
    {
      int nbTask;
      int myRank;
      // get nb proc
      MPI_Comm_size(MPI_COMM_WORLD, &nbTask); // total nb process
      // get process rank
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank); // get current process with MPI_Comm_rank and write it in variable myRank
      std::cout<<"Hello world ("<<myRank<<","<<nbTask<<")"<<std::endl ;
    }
  }
  timer.printInfo() ;

  // Finalyze MPI
  MPI_Finalize() ;
  return 0 ;
}
