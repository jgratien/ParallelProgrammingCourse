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
      int my_rank = 4;
      int nb_procs = 4 ;
      int token = 0;
      MPI_Status status;
      MPI_Comm_size(MPI_COMM_WORLD, &nb_procs) ;
      MPI_Comm_rank(MPI_COMM_WORLD, &my_rank) ;
      // get nb procs
      // get process rank

      if (my_rank == 0 ){
	      token += my_rank;
	      MPI_Send(&token,1,MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
	      MPI_Recv(&token,1,MPI_INT, nb_procs - 1, 0, MPI_COMM_WORLD, &status);
	} else if (my_rank == nb_procs - 1){
		MPI_Recv(&token, 1, MPI_INT, my_rank -1 ,0,MPI_COMM_WORLD,&status);
		token += my_rank;
              MPI_Send(&token,1,MPI_INT, 0, 0, MPI_COMM_WORLD);	      
	}
      else {
	      MPI_Recv(&token,1,MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD,&status);
	      token += my_rank;
	      MPI_Send(&token,1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
	}   


      std::cout<<"Hello world ("<<my_rank<<","<<nb_procs<<") token ="<<token<<std::endl ;
    }
  }
  timer.printInfo() ;

  // Finalyze MPI
  MPI_Finalize() ;
  return 0 ;
}
