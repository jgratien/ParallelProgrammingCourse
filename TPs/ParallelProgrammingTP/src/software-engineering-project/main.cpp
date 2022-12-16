#include "main.h"
#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>

// agent addition params
bool addBird = false;
bool addPredator = false;
double cursorX = 0;
double cursorY = 0;

int main(int argc, char** argv)
{
  using namespace boost::program_options ;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
      ("np",value<int>()->default_value(0), "matrix size")
      ("mode",value<int>()->default_value(0), "0:seq, 1:tbb, 2:omp");
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);
  
	int nb_thread = vm["np"].as<int>() ;
	int mode = vm["mode"].as<int>();
	
	
    // Agents initialization
    vars::agentWindowVars_t var = initAgentWindow();
    int count(0);

    // Global loop
    int count_FPS = 0, count_LOOP = 0;
    double total_FPS = 0.f;
    auto start = std::chrono::high_resolution_clock::now();
	auto start1 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < 100; i++) {
        // Update all the objects by applying the laws.
		
		if(mode == 1){
			var.fruits = updateObjects_tbb(var.obstacles, var.predators, var.birds, var.trees, var.fruits, nb_thread);
		}
		else if(mode == 2){
			var.fruits = updateObjects_omp(var.obstacles, var.predators, var.birds, var.trees, var.fruits, nb_thread);
		}
		else{
			var.fruits = updateObjects(var.obstacles, var.predators, var.birds, var.trees, var.fruits);
		}
        //display_FPS(start, total_FPS, count_FPS, count_LOOP);
    }
	auto end1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> float_ms2 = end1 - start1;
	//double fps = (1 / (float_ms.count()) * 1000) * 500;
    std::cout << float_ms2.count() << std::endl;
	std::ofstream fs;
	fs.open("time_boids.csv",std::ios_base::app);
	fs << nb_thread << ";" << mode << ";" << float_ms2.count() <<"\n";
	fs.close();
}