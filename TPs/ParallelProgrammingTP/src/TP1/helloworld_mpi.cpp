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
    {
      int my_rank = 0 ;
      int nb_procs = 1 ;
      // get nb procs
      nb_procs = MPI::COMM_WORLD.Get_size(); //Or MPI_Comm_size(MPI_COMM_WORLD, &nb_procs);
      // get process rank
      my_rank = MPI::COMM_WORLD.Get_rank();
      std::cout<<"Hello world, my rank : "<<my_rank<<", number of processors : "<<nb_procs<<")"<<std::endl ;
    }

  }
  timer.printInfo() ;

  // Finalyze MPI
  MPI_Finalize() ;
  return 0 ;
}
