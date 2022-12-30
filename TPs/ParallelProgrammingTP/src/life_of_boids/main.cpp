#include "main.h"

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>

#include <fstream>

#include "omp.h"
#include <tbb/tbb.h>

// agent addition params
bool addBird = false;
bool addPredator = false;
double cursorX = 0;
double cursorY = 0;
  
  int main(int argc, char** argv)
{
  using namespace tbb;
  using namespace boost::program_options ;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
      ("mode",value<int>()->default_value(0), "mode")
	  ("nb-threads",value<int>()->default_value(0), "nb threads");
	  
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help"))
  {
      std::cout << desc << "\n";
      return 1;
  }

  int nb_threads = vm["nb-threads"].as<int>() ;
  int mode = vm["mode"].as<int>() ;

//_______________________________________________________________________
  
   if(nb_threads>0)
   {
    omp_set_num_threads(nb_threads) ;
   }
   
   
  
  
  if(nb_threads>0)
  {
    omp_set_num_threads(nb_threads) ;
    tbb::task_scheduler_init init(nb_threads);
  }

  int nb_procs  = omp_get_num_procs() ;
  
  std::cout<<"Number of Procs : "<<nb_procs<<std::endl ;
  
  int nb_available_threads = omp_get_max_threads() ;
  
  std::cout<<"N° of Available Threads : "<< nb_available_threads<<std::endl ;
 
  
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    // Agents initialization 
    
	vars::agentWindowVars_t var = initAgentWindow();
    
	int count(0);

    // Global loop
    
	int count_FPS = 0, count_LOOP = 0;
    
	double total_FPS = 0.f;
  
	auto start = std::chrono::high_resolution_clock::now();
	    auto start1 = std::chrono::high_resolution_clock::now();


   for (size_t i = 0; i < 100; i++) {
		
		if(mode == 1){
			var.fruits = updateObjects_tbb(var.obstacles, var.predators, var.birds, var.trees, var.fruits, nb_threads);
		}
		else if(mode == 2){
			var.fruits = updateObjects_omp(var.obstacles, var.predators, var.birds, var.trees, var.fruits, nb_threads);
		}
		else{
			var.fruits = updateObjects_seq(var.obstacles, var.predators, var.birds, var.trees, var.fruits);
		}
        //display_FPS(start, total_FPS, count_FPS, count_LOOP);
    }
	
		  
    auto end1 = std::chrono::high_resolution_clock::now();
	
	std::chrono::duration<double, std::milli> float_ms2 = end1 - start1;
	
    std::cout << float_ms2.count() << std::endl;
	
  std::ofstream outfile;
  outfile.open("life_of_boids.log", std::ios_base::app);
  outfile << "nb_threads: " << nb_threads << " Task methode: " << mode <<" Count: " << float_ms2.count()<< std::endl;
  outfile.close();
}


    