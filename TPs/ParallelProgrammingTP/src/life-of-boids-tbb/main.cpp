#include "main.h"
#include <sstream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <tbb/tbb.h>
#include "Utils/Timer.h"

int main(int argc, char** argv) {
	using namespace boost::program_options ;
	options_description desc;
	desc.add_options()
		("help", "produce help")
		("nb-threads",value<int>()->default_value(0), "nb threads") 
		("nb-birds",value<int>()->default_value(1000), "nb birds") 
		//("csv",value<std::string>(), "csv path")
		;

	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);
	
	if (vm.count("help")) {
      		std::cout << desc << "\n";
      		return 1;
  	}
	
	int nb_threads = vm["nb-threads"].as<int>();
	tbb::task_scheduler_init init(nb_threads);
	DEFAULT_NUM_BIRDS = vm["nb-birds"].as<int>();
	// std::string csv_path = vm["csv"].as<std::string>();
  // Agents initialization
    vars::agentWindowVars_t var = initAgentWindow();
    int count(0);
	std::stringstream ss;
	auto start = std::chrono::high_resolution_clock::now();
	{

			while (count < 100) {
				// Update all the objects by applying the laws.
				updateObjects(var.obstacles, var.predators, var.birds, var.trees, var.fruits);
				count++;
			}
	}
	auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	ss << "[INFO]!results={\"method\": \"tbb\", \"nb_threads\":" << nb_threads
	<< ", \"nb_birds\": " << DEFAULT_NUM_BIRDS << ", \"time\": " << time_span.count() << "}";
		
	std::cout << ss.str() << std::endl;
/*
	std::ofstream f;

	f.open(csv_path, std::ios::app);
	f << nb_threads << "," << DEFAULT_NUM_BIRDS << "," << time_span.count() << "\n";
    f.close();	
*/
}
