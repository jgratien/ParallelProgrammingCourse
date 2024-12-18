#include "../include/utils/MathUtils.hpp"
#include "../include/utils/Vec2.hpp"
#include "../include/agents/Bird.hpp"
#include "../include/agents/Eagle.hpp"
#include "../include/agents/Prey.hpp"
#include "../include/config.hpp"
#include <chrono>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "Grid.hpp"

#define SEQUENTIAL 0
#define OPENMP 1
#define TBB 2

int main(int argc, char* argv[]) {
    SimulationConfig config = parseArguments(argc, argv, config);

    // Default parameters
    int mode = SEQUENTIAL;       
    int numAgents = 1000;        
    int numThreads = 4;          

    // Parse arguments
    if (argc > 1) mode = std::atoi(argv[1]);
    if (argc > 2) numAgents = std::atoi(argv[2]);
    if (argc > 3) numThreads = std::atoi(argv[3]);

    if (mode < 0 || mode > 2 || numAgents <= 0 || numThreads <= 0) {
        std::cerr << "Invalid arguments!" << std::endl;
        return EXIT_FAILURE;
    }

    // Configure threads
    if (mode == OPENMP) {
        omp_set_num_threads(numThreads);
    }

    if (mode == TBB) {
        std::cout << "TBB mode selected with " << numThreads << " threads." << std::endl;
    }

    int width = 800, height = 600; // Default screen size
    Grid grid(width, height, 10.0f);  // Cell size 10.0f

    std::vector<Agent*> agents;

    // Create agents
    for (int i = 0; i < numAgents; ++i) {
        agents.push_back(new Bird());
    }

    // Start timing the execution
    auto start_time = std::chrono::high_resolution_clock::now();

    // Simulate for a number of steps
    int steps = 1000;
    for (int step = 0; step < steps; ++step) {
        grid.clear();
        for (auto agent : agents) {
            grid.addBoid(agent);
        }
        grid.updateBoids(mode);
    }

    // End timing after the simulation
    auto end_time = std::chrono::high_resolution_clock::now();

    // Calculate the time difference
    std::chrono::duration<double> duration = end_time - start_time;

    // Output the time taken for the simulation
    std::cout << "Simulation completed in " << duration.count() << " seconds." << std::endl;

    // Clean up agents
    for (auto agent : agents) {
        delete agent;
    }

    std::ofstream outfile;
    outfile.open("/home/nsnidi/Parallel_Programming/Parallel_Programming/src/freebirds/src/Bench_birds.log",std::ios_base::app);
    outfile << "Simulation completed in mode " << mode
            << " with " << numThreads << " threads, "
            << numAgents << " birds, "
            << "in " << duration.count() << " ms." << std::endl;

    return 0;
}
