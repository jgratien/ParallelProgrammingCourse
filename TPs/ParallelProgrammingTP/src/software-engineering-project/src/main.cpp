#include "main.h"
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
	using namespace boost::program_options;
	options_description desc;
	desc.add_options()
		("n_th", value<int>()->default_value(1), "nb threads")
		("mode", value<int>()->default_value(0), "computation mode");
	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);
    // Agents initialization
    vars::agentWindowVars_t var = initAgentWindow();
    int count(0);

    // Global loop
    int count_FPS = 0, count_LOOP = 0;
    double total_FPS = 0.f;
	double fps;
	int mode = vm["mode"].as<int>();
	int n_th = vm["n_th"].as<int>();
	auto start = std::chrono::high_resolution_clock::now();
	auto begin = std::chrono::high_resolution_clock::now();
	auto end = std::chrono::high_resolution_clock::now();
	{
		if (mode == 0)
		{
			while ((end - begin).count() < 1000000000) {
				// Update all the objects by applying the laws.
				var.fruits = updateObjects(var.obstacles, var.predators, var.birds, var.trees, var.fruits);
				fps = display_FPS(start, total_FPS, count_FPS, count_LOOP);
				end = std::chrono::high_resolution_clock::now();

			}
			std::cout << "mean fps : " << total_FPS / count_LOOP << " mode :" << mode << '\n';
		}
		else if (mode == 1)
		{

			while ((end - begin).count() < 1000000000) {
				// Update all the objects by applying the laws.
				var.fruits = omp_updateObjects(var.obstacles, var.predators, var.birds, var.trees, var.fruits, n_th);
				fps = display_FPS(start, total_FPS, count_FPS, count_LOOP);
				end = std::chrono::high_resolution_clock::now();

			}
			std::cout << "mean fps : " << total_FPS / count_LOOP << " mode :" << mode << " nb_threads :" << n_th << '\n';
		}
		else if (mode == 2)
		{
			while ((end - begin).count() < 1000000000) {
				// Update all the objects by applying the laws.
				var.fruits = tbb_updateObjects(var.obstacles, var.predators, var.birds, var.trees, var.fruits, n_th);
				fps = display_FPS(start, total_FPS, count_FPS, count_LOOP);
				end = std::chrono::high_resolution_clock::now();
			}
			std::cout << "mean fps : " << total_FPS / count_LOOP <<" mode :"<<mode<< " nb_threads :" << n_th << '\n';
		}
		
	}

}