#include "main.h"
#include <sstream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include "omp.h"
#include <tbb/tbb.h>


int main(int argc, char** argv) {
	using namespace boost::program_options ;
	options_description desc;
	desc.add_options()
		("help", "produce help")
		("nb-threads",value<int>()->default_value(0), "nb threads") 
		("method",value<int>()->default_value(1), "method (1-omp/2-omptask/3-tbb)") 
		("nb-birds",value<int>()->default_value(1000), "nb birds")
		;

	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);
	
	if (vm.count("help")) {
      		std::cout << desc << "\n";
      		return 1;
  	}
	
	int nb_threads = vm["nb-threads"].as<int>();
	int method = vm["method"].as<int>();
	if(nb_threads > 0) {
        	omp_set_num_threads(nb_threads) ;
		tbb::task_scheduler_init init(nb_threads);
	}
    


	DEFAULT_NUM_BIRDS = vm["nb-birds"].as<int>();
  // Agents initialization
    vars::agentWindowVars_t var = initAgentWindow();
    int count(0);
    std::function<std::vector<Fruit>(std::vector<Obstacle>& obstacles,
                                predators_t& predators,
                                birds_t& birds,
                                std::vector<Tree>& trees,
                                std::vector<Fruit>& fruits)> update;
    std::string method_name = "openmp";
	switch(method) {
		case 1:
			method_name = "openmp";
			update = updateObjects_omp;
			break;
		case 2:
			method_name = "openmp_task";
			update = updateObjects_task;
			break;
		case 3:
			method_name = "tbb";
			update = updateObjects_tbb;
			break;
	}

	std::stringstream ss;
	auto start = std::chrono::high_resolution_clock::now();
	{

		while (count < 100) {
			// Update all the objects by applying the laws.
			update(var.obstacles, var.predators, var.birds, var.trees, var.fruits);
			count++;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	ss << "[INFO]!results={\"method\": \"" << method_name << "\", \"nb_threads\":" << nb_threads
	<< ", \"nb_birds\": " << DEFAULT_NUM_BIRDS << ", \"time\": " << time_span.count() << "}";
		
	std::cout << ss.str() << std::endl;
/*
    // Global loop
	std::stringstream ss;
	auto start = std::chrono::high_resolution_clock::now();
	{

		while (count < 100) {
			// Update all the objects by applying the laws.
			updateObjects_omp(var.obstacles, var.predators, var.birds, var.trees, var.fruits);
			count++;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	ss << "[INFO]!results={\"method\": \"openmp\", \"nb_threads\":" << nb_threads
	<< ", \"nb_birds\": " << DEFAULT_NUM_BIRDS << ", \"time\": " << time_span.count() << "}";
		
	std::cout << ss.str() << std::endl;
	
	std::stringstream ss2;
	var = initAgentWindow();
	count = 0;
	start = std::chrono::high_resolution_clock::now();
	{

		while (count < 100) {
			// Update all the objects by applying the laws.
			updateObjects_task(var.obstacles, var.predators, var.birds, var.trees, var.fruits);
			count++;
		}
	}
	end = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	ss2 << "[INFO]!results={\"method\": \"openmp_task\", \"nb_threads\":" << nb_threads
	<< ", \"nb_birds\": " << DEFAULT_NUM_BIRDS << ", \"time\": " << time_span.count() << "}";
		
	std::cout << ss2.str() << std::endl;
	
	std::stringstream ss3;
	var = initAgentWindow();
	count = 0;
	start = std::chrono::high_resolution_clock::now();
	{

		while (count < 100) {
			// Update all the objects by applying the laws.
			updateObjects_tbb(var.obstacles, var.predators, var.birds, var.trees, var.fruits);
			count++;
		}
	}
	end = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	ss3 << "[INFO]!results={\"method\": \"tbb\", \"nb_threads\":" << nb_threads
	<< ", \"nb_birds\": " << DEFAULT_NUM_BIRDS << ", \"time\": " << time_span.count() << "}";
		
	std::cout << ss3.str() << std::endl;
*/
}
