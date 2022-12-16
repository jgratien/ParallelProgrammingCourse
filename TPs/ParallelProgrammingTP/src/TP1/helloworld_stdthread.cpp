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

#include <thread>

#include "Utils/Timer.h"

void print_hello(int nb_threads,int id)
{
  auto uid = std::this_thread::get_id();
  sleep(id) ;
  std::cout<<"Hello Word ("<<id<<","<<nb_threads<<")"<<std::endl ;
}

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

  std::vector<std::thread> thread_pool ;
  {
    PPTP::Timer::Sentry sentry(timer,"HelloWord") ;
    for(int i=0;i<nb_threads;++i)
    {
      //print_hello(nb_threads,i) ;
      //create thread to call print_hello
	  thread_pool.push_back(std::thread(print_hello, nb_threads,i ));
    }
  
    for(auto& th : thread_pool)
      th.join() ;
  }


  timer.printInfo() ;
  return 0 ;
}
