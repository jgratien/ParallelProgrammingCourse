#include "main.h"
#include <tbb/tbb.h>
#include <tbb/parallel_for.h>
#include <algorithm>

vars::agentWindowVars_t initAgentWindow() {

    vars::agentWindowVars_t var;
    var.obstacles = Obstacle::init();
    var.predators = Predator::init(var.obstacles);
    var.birds = Bird::init(var.obstacles, var.predators);

    var.trees = Tree::init(var.obstacles);
    var.fruits = {};
    for (Tree tree : var.trees) {
        tree.DropFruitAndAppend(var.fruits);
    }

    return var;
}

std::vector<Fruit> updateObjects(std::vector<Obstacle>& obstacles,
                                    predators_t& predators,
                                    birds_t& birds,
                                    std::vector<Tree>& trees,
                                    std::vector<Fruit>& fruits) {

    std::vector<Fruit> newFruits;

//================================================OPENMP===================================================
	
	if (birds.size() > 0) {
		
		std::vector<size_t> keys1(predators.size()), keys2(birds.size()), to_kill_ids;
		std::transform(predators.begin(), predators.end(), keys1.begin(), [](auto& it) {return it.first;});
		std::transform(birds.begin(), birds.end(), keys2.begin(), [](auto& it) {return it.first;});

		#pragma omp parallel for
		for (size_t i=0; i<keys1.size(); ++i)
			predators[keys1[i]].update(obstacles, predators, birds);

		#pragma omp parallel for
		for (size_t i=0; i<keys2.size(); ++i) {
			bool is_alive = birds[keys2[i]].update(obstacles, predators, birds, fruits);
			if (is_alive == false) {
				// critical section
				#pragma omp critical
				to_kill_ids.push_back(keys2[i]);
			}
		}

		for (size_t i : to_kill_ids) {
			if (birds.find(i) != birds.end())
				birds.erase(i);
		}
	}

//================================================OPENMP===================================================

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

	return newFruits;

}

std::vector<Fruit> updateObjects_task(std::vector<Obstacle>& obstacles,
                                    predators_t& predators,
                                    birds_t& birds,
                                    std::vector<Tree>& trees,
                                    std::vector<Fruit>& fruits) {

    std::vector<Fruit> newFruits;
	
//================================================OPENMP TASK===================================================
	std::vector<size_t> to_kill_ids;
	
	#pragma omp parallel
	{
		#pragma omp single
		{
    		for (auto& it : predators) {
				#pragma omp task
				{
        			it.second.update(obstacles, predators, birds);
				}
    		}
		}
		#pragma omp taskwait
	}
	
	#pragma omp parallel
	{
		#pragma omp single
		{
    		for (auto it = birds.begin(); it != birds.end(); it++) {

				#pragma omp task
				{
        
        			bool is_alive = it->second.update(obstacles, predators, birds, fruits);
					
					if (!is_alive) {
						#pragma omp critical
						{
							to_kill_ids.push_back(it->first);
						}
					}
				}
			}
		}
		#pragma omp taskwait
	}

	for (size_t i : to_kill_ids)
			birds.erase(i);
//================================================OPENMP TASK===================================================

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

    return newFruits;
}


void display_FPS(std::chrono::time_point<std::chrono::high_resolution_clock>& start,
    double& total_fps,
    int& i, int& j) {
    i++;
    if (i == NUMBER_LOOP_FPS) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> float_ms = end - start;
        double fps = (1 / (float_ms.count()) * 1000) * NUMBER_LOOP_FPS;
        // std::cout << fps << std::endl;
        total_fps += fps;
        start = std::chrono::high_resolution_clock::now();
        i = 0;
        j++;
    }
}
