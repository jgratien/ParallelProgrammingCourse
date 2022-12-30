#include "main.h"
#include <omp.h>
#include "tbb/tbb.h"

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
//_____________________________________________Seq _______________________________________

std::vector<Fruit> updateObjects_seq(std::vector<Obstacle>& obstacles,
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

    return newFruits;
}

//_____________________________________________ Omp _______________________________________

std::vector<Fruit> updateObjects_omp(std::vector<Obstacle>& obstacles, predators_t& predators, birds_t& birds,std::vector<Tree>& trees,
                                     std::vector<Fruit>& fruits, int nb_thread) 
{

    std::vector<Fruit> newFruits;
	
	if(nb_thread>0)
			omp_set_num_threads(nb_thread) ;

    #pragma omp parallel
    {
      
      #pragma omp for
      
      for (std::size_t i=0; i<predators.size(); ++i) 
	  {
	     predators[i].update(obstacles, predators, birds);
      }

      #pragma omp for
      for (std::size_t i=0; i<birds.size(); i++) 
	  {
	    bool is_alive = birds[i].update(obstacles, predators, birds, fruits);
	    
      }

      for (Tree& tree : trees) 
	  {
        tree.DropFruitAndAppend(fruits);
      }

      for (Fruit& fruit : fruits) 
	  {
        if (fruit.get_alive())
            newFruits.push_back(fruit);
        else 
		{
            size_t n = birds.size();
            birds[n] = Bird(fruit.get_x(), fruit.get_y(), 0, n);
        }
	  }
    }

    return newFruits;
}

//_____________________________________________ Tbb __________________________________________

std::vector<Fruit> updateObjects_tbb(std::vector<Obstacle>& obstacles,
                                    predators_t& predators,
                                    birds_t& birds,
                                    std::vector<Tree>& trees,
                                    std::vector<Fruit>& fruits,
									int nb_thread) {

    std::vector<Fruit> newFruits;
	
    tbb::task_scheduler_init init(nb_thread);
	
	tbb::parallel_for( size_t(0), 
	                   birds.size(), 
					   [&](size_t i) 
					   {
						   bool is_alive = birds[i].update
						 (
						   obstacles, 
						   predators, 
						   birds, 
						   fruits);
						}
	);


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

//_____________________________________________ Display _____________________________________

void display_FPS(std::chrono::time_point<std::chrono::high_resolution_clock>& start,
    double& total_fps,
    int& i, int& j) {
    i++;
    if (i == NUMBER_LOOP_FPS) 
	{
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> float_ms = end - start;
        double fps = (1 / (float_ms.count()) * 1000) * NUMBER_LOOP_FPS;
        std::cout << fps << std::endl;
        total_fps += fps;
        start = std::chrono::high_resolution_clock::now();
        i = 0;
        j++;
    }
}