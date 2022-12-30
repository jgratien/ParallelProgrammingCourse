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
        // least frequently changing members must appear at the top for better access to the memory.
        // and to avoid the structure padding.
        std::vector<Obstacle> obstacles;
        std::vector<Tree> trees;
        predators_t predators;
        birds_t birds;
        std::vector<Fruit> fruits;
    };
    using agentWindowVars_t = struct agentVars_t;
};


std::vector<Fruit> updateObjects(std::vector<Obstacle>& obstacles,
                                    predators_t& predators,
                                    birds_t& birds,
                                    std::vector<Tree>& trees,
                                    std::vector<Fruit>& fruits);


std::vector<Fruit> updateObjectsOMP(std::vector<Obstacle>& obstacles,
                                    predators_t& predators,
                                    birds_t& birds,
                                    std::vector<Tree>& trees,
                                    std::vector<Fruit>& fruits,
									int& np);

std::vector<Fruit> updateObjectsTBB(std::vector<Obstacle>& obstacles,
                                    predators_t& predators,
                                    birds_t& birds,
                                    std::vector<Tree>& trees,
                                    std::vector<Fruit>& fruits,
									int& np);
									
vars::agentWindowVars_t initAgentWindow();

void display_FPS(std::chrono::time_point<std::chrono::high_resolution_clock>& start,
    double& total_fps,
    int& i, int& j);
	
