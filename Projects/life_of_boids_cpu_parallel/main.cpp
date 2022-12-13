#include "main.h"
#include "update.h"

int main(int argc, char *argv[]) {
    
    // Nb threads initialization
    int np(1);
    if(argv[1]!=NULL) np = std::atoi(argv[1]);

    int num_birds(1);
    if (argv[2] != NULL) num_birds = std::atoi(argv[2]);

    std::string mode;
    if (argv[2] != NULL) mode = argv[3];
	
    // Agents initialization
    vars::agentWindowVars_t var = initAgentWindow(num_birds);
    int count(0);

    // Global loop
    int iter(0), max_iter(100);
    auto start = std::chrono::high_resolution_clock::now();

    while (iter < max_iter) {
        if(mode == "tbb")
            tbbUpdateObjects(var.obstacles, var.predators, var.birds, var.trees, var.fruits, np);
        else if (mode == "omp")
            ompUpdateObjects(var.obstacles, var.predators, var.birds, var.trees, var.fruits, np);
        else
            updateObjects(var.obstacles, var.predators, var.birds, var.trees, var.fruits);
	    iter ++;
    }

    write_results(start, mode, np, num_birds);
}
