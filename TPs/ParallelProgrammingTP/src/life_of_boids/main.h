# pragma once

#include "config/config.h"
#include "movement/agents/agent.h"
#include "movement/agents/predator.h"
#include "movement/agents/bird.h"
#include "movement/obstacles/obstacle.h"
#include "movement/fruits/fruit.h"
#include "movement/fruits/tree.h"

namespace vars {

    struct agentVars_t {
		
        std::vector<Obstacle> obstacles;
        std::vector<Tree> trees;
        predators_t predators;
        birds_t birds;
        std::vector<Fruit> fruits;
    };
    using agentWindowVars_t = struct agentVars_t;
};

 std::vector<Fruit> updateObjects_seq(std::vector<Obstacle>& obstacles,
                                predators_t& predators,
                                birds_t& birds,
                                std::vector<Tree>& trees,
                                std::vector<Fruit>& fruits);
								
 std::vector<Fruit> updateObjects_omp(std::vector<Obstacle>& obstacles,
                                predators_t& predators,
                                birds_t& birds,
                                std::vector<Tree>& trees,
                                std::vector<Fruit>& fruits,
								int nb_thread);

 std::vector<Fruit> updateObjects_tbb(std::vector<Obstacle>& obstacles,
                                predators_t& predators,
                                birds_t& birds,
                                std::vector<Tree>& trees,
                                std::vector<Fruit>& fruits,
								int nb_thread);								

vars::agentWindowVars_t initAgentWindow();

void display_FPS(std::chrono::time_point<std::chrono::high_resolution_clock>& start,
    double& total_fps,
    int& i, int& j);