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


 int my_rank = 0 ;
 int nb_procs = 1 ;
 
 PPTP::Timer timer ;
  {
    PPTP::Timer::Sentry sentry(timer,"HelloWord") ;


    //#pragma omp ....CREATE PARALLEL SECTION
    {
      // get nb procs
      int error = MPI_Comm_size(MPI_COMM_WORLD,&nb_procs);
      // get process rank
      error += MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
      std::cout<<"Hello world ("<<my_rank<<","<<nb_procs<<")"<<std::endl ;
    }
  }
  {

	
	if(my_rank==0)
	{	
		MPI_Status status;
		int value = 0;
		
		value =10;
		//send to rank = 1 value
		 MPI_Send(&value, 1, MPI_INT, 1, 1000, MPI_COMM_WORLD);
		//recv du rank = nb_procs -1 last value
		MPI_Recv(&value, 1, MPI_INT, nb_procs -1, 1000, MPI_COMM_WORLD, &status);
		std::cout<<"PING_PONG"<<value<<std::endl;
	}
	else if(my_rank==nb_procs -1)
	{
		
		MPI_Status status;
		int value = 0;
		
		//recoit la valeur du precedent	
		MPI_Recv(&value, 1, MPI_INT, my_rank-1, 1000, MPI_COMM_WORLD, &status);
		std::cout<<"RECV["<<my_rank<<"]"<<value<<std::endl;
		// envoie au rank 0
		value += my_rank;

		std::cout<<"SEND"<<my_rank<<"]"<<value<<std::endl;
	       	MPI_Send(&value, 1, MPI_INT, 0, 1000, MPI_COMM_WORLD);

		
	}
	else
	{

		int value =0;
		MPI_Status status;
		//recoit la valeur du precedent	
		MPI_Recv(&value, 1, MPI_INT, my_rank-1, 1000, MPI_COMM_WORLD, &status);
		
		value += my_rank ; 
		//envoir au suivant

	       	MPI_Send(&value, 1, MPI_INT, my_rank+1, 1000, MPI_COMM_WORLD);

	}


  }
  timer.printInfo() ;

  // Finalyze MPI
   MPI_Finalize() ;
  return 0 ;
}
