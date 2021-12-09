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
      ("nb-threads",value<int>()->default_value(0), "nb threads") ;
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

  int nb_procs     = omp_get_num_procs() ;
  std::cout<<"NB PROCS     :"<<nb_procs<<std::endl ;
  int nb_available_threads = omp_get_max_threads() ;
  std::cout<<"NB AVAILABLE_THREADS :"<<nb_available_threads<<std::endl ;
  {
    PPTP::Timer::Sentry sentry(timer,"HelloWord") ;


    if(nb_threads == 0){
    	nb_threads = nb_available_threads;
    }

    #pragma omp parallel
    {
//    	int id = 0 ;
//    	int nb_threads = nb_available_threads;//1 ;


		#pragma omp for
    	for(int i = 0; i < nb_threads; i++){
//    		sleep(i);
    		float x = 25;
    		for(int j = 0; j< 100000000*i; ++j){
    			x /= std::sqrt(x);
    		}
    		std::cout<<"Hello world ("<<i+1<<","<<nb_threads<<")"<<std::endl;
    	}
//      sleep(id) ;

    }
  }
  timer.printInfo() ;
  return 0 ;
}
