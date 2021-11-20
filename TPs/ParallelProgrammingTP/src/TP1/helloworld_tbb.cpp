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

#include "tbb/tbb.h"

#include "Utils/Timer.h"

void print_hello(std::size_t id,std::size_t nb_threads)
{
  sleep(id) ;
  std::cout<<"Hello Word ("<<id<<","<<nb_threads<<")"<<std::endl ;
}



using namespace tbb;
int main(int argc, char** argv)
{

  using namespace tbb;
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

  std::size_t nb_threads = vm["nb-threads"].as<int>() ;

  {
    PPTP::Timer::Sentry sentry(timer,"HelloWord") ;
    for(std::size_t i=0;i<nb_threads;++i)
      print_hello(i,nb_threads);

    // USE TBB to call print info in parallel
  }

  timer.printInfo() ;
  return 0 ;
}
