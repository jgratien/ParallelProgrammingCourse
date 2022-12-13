# pragma once

#include <fstream>

#include "tbb/tbb.h"
#include <mutex>
static std::mutex kill_mtx;

#include <omp.h>

#include "config/config.h"
#include "objects/agents/agent.h"
#include "objects/agents/predator.h"
#include "objects/agents/bird.h"
#include "objects/obstacles/obstacle.h"
#include "objects/obstacles/fruit.h"
#include "objects/obstacles/tree.h"

void updateObjects(std::vector<Obstacle>& obstacles,
    predators_t& predators,
    birds_t& birds,
    std::vector<Tree>& trees,
    std::vector<Fruit>& fruits) {

    std::vector<Fruit> newFruits;

    for (auto& it : predators) {
        it.second.update(obstacles, predators, birds);
    }

    for (auto it = birds.begin(); it != birds.end();) {

        bool is_alive = it->second.update(obstacles, predators, birds, fruits);
        if (!is_alive)
            it = birds.erase(it);
        else
            it++;
    }

    for (Tree& tree : trees) {
        tree.DropFruitAndAppend(fruits);
    }

    for (Fruit& fruit : fruits) {
        if (fruit.get_alive())
            newFruits.push_back(fruit);
        else {
            size_t n = birds.size();
            birds[n] = Bird(fruit.get_x(), fruit.get_y(), 0, n);
        }
    }
}


void tbbUpdateObjects(std::vector<Obstacle>& obstacles,
    predators_t& predators,
    birds_t& birds,
    std::vector<Tree>& trees,
    std::vector<Fruit>& fruits,
	int& np) {

    std::vector<Fruit> newFruits;
    std::vector<size_t> kill;
	
	tbb::task_scheduler_init init(np);

    tbb::parallel_for(size_t(0),predators.size(),
		    [&](size_t i)
		    {
			predators[i].update(obstacles, predators, birds);	
		    }
		    );

    tbb::parallel_for(size_t(0),birds.size(),
		    [&](size_t i)
		    {
		    		bool is_alive = birds[i].update(obstacles, predators, birds, fruits);

                		if (!is_alive) {
                    			kill_mtx.lock();
                    			kill.push_back(i);
                    			kill_mtx.unlock();
                		}

		    }
		    );

    // Remove the dead birds
    for (size_t& k : kill)
        birds.erase(k);

    // Sequential cause not enough workload
    for (Tree& tree : trees) {
        tree.DropFruitAndAppend(fruits);
    }

    // Sequential cause not enough workload
    for (Fruit& fruit : fruits) {
        if (fruit.get_alive())
            newFruits.push_back(fruit);
        else {
            size_t n = birds.size();
            birds[n] = Bird(fruit.get_x(), fruit.get_y(), 0, n);
        }
    }
}

void ompUpdateObjects(std::vector<Obstacle>& obstacles,
    predators_t& predators,
    birds_t& birds,
    std::vector<Tree>& trees,
    std::vector<Fruit>& fruits,
    int& np) {


    	std::vector<Fruit> newFruits;
    	std::vector<size_t> kill;

	omp_set_num_threads(np);

	#pragma omp parallel
	{
		#pragma omp for
		for(size_t i=0; i<predators.size(); ++i) {
			predators[i].update(obstacles, predators, birds);
		}

		std::vector<size_t> kill_private;
		#pragma omp for
		for(size_t i=0; i<birds.size(); ++i) {
			bool is_alive = birds[i].update(obstacles, predators, birds, fruits);
			if(!is_alive) {
				kill_private.push_back(i);
			}
		}
		#pragma omp critical
		kill.insert(kill.end(), kill_private.begin(), kill_private.end());
	}


    	// Remove the dead birds
    	for (size_t& k : kill)
        	birds.erase(k);

    	// Sequential cause not enough workload
    	for (Tree& tree : trees) {
        	tree.DropFruitAndAppend(fruits);
   	 }

    	// Sequential cause not enough workload
    	for (Fruit& fruit : fruits) {
        	if (fruit.get_alive())
            		newFruits.push_back(fruit);
        	else {
            		size_t n = birds.size();
            		birds[n] = Bird(fruit.get_x(), fruit.get_y(), 0, n);
        	}
    	}
}
