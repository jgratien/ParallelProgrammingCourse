# pragma once

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>

#include "config/config.h"
#include "objects/agents/agent.h"
#include "objects/agents/predator.h"
#include "objects/agents/bird.h"
#include "objects/obstacles/obstacle.h"
#include "objects/obstacles/fruit.h"
#include "objects/obstacles/tree.h"

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

vars::agentWindowVars_t initAgentWindow(int& num_birds) {

    vars::agentWindowVars_t var;
    var.obstacles = Obstacle::init();
    var.predators = Predator::init(var.obstacles);
    var.birds = Bird::init(var.obstacles, var.predators, num_birds);

    var.trees = Tree::init(var.obstacles);
    var.fruits = {};
    for (Tree tree : var.trees) {
        tree.DropFruitAndAppend(var.fruits);
    }

    return var;
}
void write_results(std::chrono::time_point<std::chrono::high_resolution_clock>& start, std::string mode, int& np, int& num_birds) {

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
	
	std::ifstream infile;
	infile.open("time.log");
	if(!infile) {
		std::ofstream outfile;
		outfile.open("time.log", std::ios_base::app);
		outfile << "Mode    " << "Size	" << "Cores	" << "Time" << std::endl;
	}

	std::ofstream outfile;
	outfile.open("time.log", std::ios_base::app);
	outfile << mode << "    " << num_birds << "	" << np << "	" << duration.count() << std::endl;
}
