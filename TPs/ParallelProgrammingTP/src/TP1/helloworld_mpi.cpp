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
      int my_rank;
      int nb_procs;
      MPI_Comm_size(MPI_COMM_WORLD, &nb_procs);
      MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


      if(my_rank == 0){
          int x = 23;
          MPI_Ssend(
          &x,
          1,
          MPI_INT,
          1,
          40,
          MPI_COMM_WORLD);
      }
      else if(my_rank == 1){
          int y;
          MPI_Status status;
          MPI_Recv(
          &y,             //Buffer itself
          1,              //Size of buffer
          MPI_INT,        //Type
          0,              //Source
          40,             //Tag - NEED to be the same as sender
          MPI_COMM_WORLD, //Communicator
          &status         //MPI Status
          );

          std::cout << "I RECEIVED Y : " << y << '\n';
          // std::cout << "STATUS : " << status << '\n';
      }

      // get nb procs
      // get process rank
      std::cout<<"Hello world ("<<my_rank<<","<<nb_procs<<")"<<std::endl ;
    }
  }
  timer.printInfo() ;

  // Finalyze MPI
  MPI_Finalize() ;
  return 0 ;
}
