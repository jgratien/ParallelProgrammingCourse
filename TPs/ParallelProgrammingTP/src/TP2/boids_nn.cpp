/*
 * boids_nn.cpp
 *
 *  Created on: Dec 7, 2024
 *      Author: ossaradj
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

#include "Utils/Timer.h"

#include "LiveOfBoids/Boid.h"
#include "LiveOfBoids/BoidGenerator.h"

int main(int argc, char** argv)
{
    using namespace boost::program_options ;
    options_description desc;
    desc.add_options()
        ("help", "produce help")
        ("nb-threads",value<int>()->default_value(0), "nb threads")
        ("nrows",value<int>()->default_value(0), "matrix size")
        ("nx",value<int>()->default_value(0), "nx grid size")
        ("nb-boids",value<int>()->default_value(0), "number of boids")
        ("chunk-size",value<int>()->default_value(1), "chunk size");
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
    BoidGenerator generator ;
    std::vector<Boid> boids ;
    float radius = 10.f ;
    
    int nb = vm["nb-boids"].as<int>() ;
    generator.generate(nb, boids) ;
    
    std::vector<int> y(nb);
    {
        {
            Timer::Sentry sentry(timer,"Boids") ;
            generator.findNeighbors(boids,y,radius) ;
        }
        std::cout << "finish Boids\n" ;
        // double normy = PPTP::norm2(y) ;
        // std::cout<<"||y||="<<normy<<std::endl ;
        // for(int i=0; i<nb; ++i)
        //     if(y[i]!=0)
        //         std::cout << y[i] << "\n" ;
    }

    generator.setChunkSize(vm["chunk-size"].as<int>()) ;
    {
        std::vector<int> y2(nb);
        {
            Timer::Sentry sentry(timer,"OMPTaskBoids") ;
            generator.omptaskfindNeighbors(boids,y2,radius) ;
        }
        std::cout << "finish 2\n" ; 
        // double normy = PPTP::norm2(y) ;
        // std::cout<<"OMPTask ||y||="<<normy<<std::endl ;
        for(int i=0; i<nb; ++i)
            if(y2[i]!=0)
                if(y[i] != y2[i]) 
                {
                    std::cout << "False OMPTaskBoids" << "\n" ;
                    break ;
                }
    }

    {
      std::vector<int> y2(nb);
        {
            Timer::Sentry sentry(timer,"OMPTileBoids") ;
            generator.omptilefindNeighbors(boids,y2,radius) ;
        }
        std::cout << "finish 3\n" ; 
        // double normy = PPTP::norm2(y) ;
        // std::cout<<"OMPTask ||y||="<<normy<<std::endl ;
        for(int i=0; i<nb; ++i)
            if(y2[i]!=0)
                if(y[i] != y2[i]) 
                {
                    std::cout << "False OMPTileBoids" << "\n" ;
                    break ;
                }
    }

    {
      std::vector<int> y2(nb);
      {
        Timer::Sentry sentry(timer,"TBBRangeBoids") ;
        generator.tbbrangefindNeighbors(boids,y2,radius) ;
      }
      std::cout << "finish 4\n" ;
    //   double normy = PPTP::norm2(y) ;
    //   std::cout<<"TBBRange ||y||="<<normy<<std::endl ;
        for(int i=0; i<nb; ++i)
            if(y2[i]!=0)
                if(y[i] != y2[i]) 
                {
                    std::cout << "False TBBRangeBoids" << "\n" ;
                    break ;
                }
    }

    {
      std::vector<int> y2(nb);
      {
        Timer::Sentry sentry(timer,"TBBRange2DBoids") ;
        generator.tbbrange2dfindNeighbors(boids,y2,radius) ;
      }
      std::cout << "finish 5\n" ;
    //   double normy = PPTP::norm2(y) ;
    //   std::cout<<"TBBRange2DTile ||y||="<<normy<<std::endl ;
        for(int i=0; i<nb; ++i)
            if(y2[i]!=0)
                if(y[i] != y2[i]) 
                {
                    std::cout << "False TBBRange2DBoids" << "\n" ;
                    break ;
                }
    }
  
  timer.printInfo() ;
  return 0 ;
}