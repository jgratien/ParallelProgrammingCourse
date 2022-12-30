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

int main(int argc, char** argv) {

	int np(1);
	if(argv[1]!=NULL) np = std::atoi(argv[1]);   

	std::string mode;	 
	if (argv[2] != NULL) mode = argv[2];
	
	// Agents initialization
   vars::agentWindowVars_t var = initAgentWindow();
    int count(0);
	
	
    // Global loop
    int count_FPS = 0, count_LOOP = 0;
    double total_FPS = 0.f;
    auto start = std::chrono::high_resolution_clock::now();
	auto start1 = std::chrono::high_resolution_clock::now();
	int i=0;
    while (i<100) {
		i++;
        // Update all the objects by applying the laws.
		if (mode=="OMP")
			var.fruits = updateObjectsOMP(var.obstacles, var.predators, var.birds, var.trees, var.fruits, np);		
        else if (mode=="TBB")
			var.fruits = updateObjectsTBB(var.obstacles, var.predators, var.birds, var.trees, var.fruits, np);
        else 
			var.fruits = updateObjects(var.obstacles, var.predators, var.birds, var.trees, var.fruits);
    }
	
	auto end1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end1 - start1;

    std::cout << duration.count() << std::endl;
	std::ofstream fs;
	fs.open("time_boids.csv",std::ios_base::app);
	fs << np << ";" << mode << ";" << duration.count() <<"\n";
	fs.close();
}