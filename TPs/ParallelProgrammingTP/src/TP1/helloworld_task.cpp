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
#include "Utils/Timer.h"

int main(int argc, char** argv)
{
  using namespace boost::program_options ;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
      ("nb-threads",value<int>()->default_value(0), "nb threads")
      ("nb-tasks",value<int>()->default_value(1), "nb tasks") ;
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help"))
  {
      std::cout << desc << "\n";
      return 1;
  }

  PPTP::Timer timer ;
  int nb_threads = vm["nb-threads"].as<int>() ;
  if(nb_threads>0)
    omp_set_num_threads(nb_threads) ;

  int nb_tasks = vm["nb-tasks"].as<int>() ;

  int nb_available_threads = omp_get_max_threads() ;
  std::cout<<"NB AVAILABLE_THREADS :"<<nb_available_threads<<std::endl ;
  {
    PPTP::Timer::Sentry sentry(timer,"HelloWord") ;
    //#pragma CREATE PARALLEL SECTION
    {
      //#pragma CREATE SINGLE SECTION
      {
        int id = 0;
        int nb_threads = 1;
        std::cout<<"Thread ( th_id="<<id<<", nb_th="<<nb_threads<<") create "<<nb_tasks<<" tasks"<<std::endl ;
        for(int itask=0;itask<nb_tasks;++itask)
        {
          //#pragma CREATE TASK
          {
            int id =0;  //GET THREAD ID
            sleep(itask) ;
            std::cout<<"Hello World (task_id="<<itask<<" th_id="<<id<<")"<<std::endl ;
          }
        }
        //#pragma WAIT THAT ALL TASK ARE FINISHED
        std::cout<<"Thread (th_id="<<id<<",nb_th="<<nb_threads<<") all tasks are executed"<<std::endl ;
      }
    }
  }
  timer.printInfo() ;
  return 0 ;
}
