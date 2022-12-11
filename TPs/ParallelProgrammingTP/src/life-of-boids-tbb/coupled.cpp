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

//================================================TBB===================================================
	if (birds.size() > 0) {

		std::vector<size_t> keys1(predators.size()), keys2(birds.size()), to_kill_ids;
		std::transform(predators.begin(), predators.end(), keys1.begin(), [](auto& it) {return it.first;});
		std::transform(birds.begin(), birds.end(), keys2.begin(), [](auto& it) {return it.first;});
		tbb::blocked_range<size_t> range1(0, keys1.size());
		tbb::blocked_range<size_t> range2(0, keys2.size());
		tbb::mutex kill_mutex;
		
		auto func1 = [&](tbb::blocked_range<size_t> r) {

			for (auto i=r.begin(); i<r.end(); ++i) {
					predators[keys1[i]].update(obstacles, predators, birds);
			}
		};

		auto func2 = [&](tbb::blocked_range<size_t> r) {
				
			for (auto i=r.begin(); i<r.end(); ++i) {
				if (birds[keys2[i]].update(obstacles, predators, birds, fruits) == false) {
					// acquire lock before pushing an entry into the kill list
					tbb::mutex::scoped_lock lock(kill_mutex);
					to_kill_ids.push_back(keys2[i]);
				}
			}
		};

		
		tbb::parallel_for(range1, func1);
		tbb::parallel_for(range2, func2);
		for (size_t i : to_kill_ids) {
			if (birds.find(i) != birds.end())
				birds.erase(i);
		}
	}

//================================================TBB===================================================
/*
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
*/
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
