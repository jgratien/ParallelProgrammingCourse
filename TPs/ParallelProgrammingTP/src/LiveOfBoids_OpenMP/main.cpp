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

  using namespace boost::program_options ;
  options_description desc;
  desc.add_options()
    ("help", "produce help")
    ("nb-threads",value<int>()->default_value(4), "nb threads")
    ;
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
    }
  int nb_procs     = omp_get_num_procs() ;
  std::cout<<"NB PROCS     :"<<nb_procs<<std::endl ;
  int nb_available_threads = omp_get_max_threads() ;
  std::cout<<"NB AVAILABLE_THREADS :"<<nb_available_threads<<std::endl ;

    // Agents initialization
    vars::agentWindowVars_t var = initAgentWindow();
    int count(0);

    // Global loop
    int count_FPS = 0, count_LOOP = 0;
    double total_FPS = 0.f;
    auto start = std::chrono::high_resolution_clock::now();

    while (count_LOOP<10) { //Limited to 10 loops of fps calcul
        // Update all the objects by applying the laws.
        var.fruits = updateObjects(var.obstacles, var.predators, var.birds, var.trees, var.fruits, nb_threads);
        display_FPS(start, total_FPS, count_FPS, count_LOOP);
    }

    return 1;
}
