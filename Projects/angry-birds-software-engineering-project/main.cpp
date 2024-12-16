#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <cstdlib>
#include <string>
#include <fstream>
#include <omp.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

#include "src/agent/agent.hpp"

#include <chrono>

static inline Vector2D get_pos(float t, int w, int h) {
    return Vector2D((float)(w * (0.5 + 0.4 * std::cos(t))), 
                    (float)(h * (0.5 + 0.4 * std::sin(t))));
}

int main(int argc, char* argv[]) {
    // Default values
    int np = 1;
    int birds = 1000;
    std::string mode = "seq";

    if (argc > 1) {
        np = std::atoi(argv[1]);
    }
    if (argc > 2) {
        birds = std::atoi(argv[2]);
    }
    if (argc > 3) {
        mode = argv[3];
    }

    if (mode == "omp") {
    #ifdef _OPENMP
        omp_set_num_threads(np);
    #else
        std::cerr << "OpenMP mode requested but not available!" << std::endl;
    #endif
    }

    int width = 1400;
    int height = 900;

    // Initialize agents
    std::vector<Agent> agents;
    agents.reserve(birds);
    for (int i = 0; i < birds; ++i) {
        Vector2D pos = get_pos(float(i), width, height);
        agents.emplace_back(pos, Vector2D());
    }

    // Set random initial velocities
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_real_distribution<float> angle_space(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> velocity_space(0.1f, 0.5f);

    for (auto &agent : agents) {
        auto angle = angle_space(eng);
        auto speed = velocity_space(eng);
        agent.setVelocity(Vector2D(speed * std::cos(angle), speed * std::sin(angle)));
    }

    // Number of simulation steps for the benchmark
    int steps = 1000;
    std::vector<Agent::UpdateState> newStates;
    newStates.resize(agents.size());

    
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int step = 0; step < steps; step++) {
        auto angle = angle_space(eng);
        auto speed = velocity_space(eng);
        Vector2D randomVelocity(speed * std::cos(angle), speed * std::sin(angle));

        // Compute new states
        if (mode == "seq") {
            for (size_t i = 0; i < agents.size(); i++) {
                newStates[i] = agents[i].calculateNewState(agents, &width, &height, randomVelocity);
            }
        } else if (mode == "omp") {
            #pragma omp parallel for schedule(dynamic)
            for (size_t i = 0; i < agents.size(); i++) {
                newStates[i] = agents[i].calculateNewState(agents, &width, &height, randomVelocity);
            }
        } else if (mode == "tbb") {
            tbb::parallel_for(tbb::blocked_range<size_t>(0, agents.size()),
                              [&](tbb::blocked_range<size_t> r) {
                                  for (size_t i = r.begin(); i < r.end(); i++) {
                                      newStates[i] = agents[i].calculateNewState(agents, &width, &height, randomVelocity);
                                  }
                              });

        }

        // Apply new states
        if (mode == "seq") {
            for (size_t i = 0; i < agents.size(); i++) {
                agents[i].applyState(newStates[i]);
                agents[i].cross_boundary(&width, &height);
            }
        } else if (mode == "omp") {
            #pragma omp parallel for schedule(dynamic)
            for (size_t i = 0; i < agents.size(); i++) {
                agents[i].applyState(newStates[i]);
                agents[i].cross_boundary(&width, &height);
            }
        } else if (mode == "tbb") {
            tbb::parallel_for(tbb::blocked_range<size_t>(0, agents.size()),
                              [&](tbb::blocked_range<size_t> r) {
                                  for (size_t i = r.begin(); i < r.end(); i++) {
                                      agents[i].applyState(newStates[i]);
                                      agents[i].cross_boundary(&width, &height);
                                  }
                              });
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::ofstream outfile;
    outfile.open("/gext/mahammed.el_sharkawy/parallel-programming-project/Projects/angry-birds-software-engineering-project/Bench_birds.log",std::ios_base::app);
    outfile << "Simulation completed in mode " << mode
            << " with " << np << " threads, "
            << birds << " birds, "
            << "in " << duration.count() << " ms." << std::endl;

    return 0;
}
