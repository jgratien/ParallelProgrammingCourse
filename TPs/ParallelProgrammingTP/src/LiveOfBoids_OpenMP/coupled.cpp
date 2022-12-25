#include "main.h"

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
				 std::vector<Fruit>& fruits,
				 int nb_threads) {

    std::vector<Fruit> newFruits;

    std::size_t N_predators = predators.size();
    std::size_t N_birds = birds.size();
    std::size_t N_trees = trees.size();
    std::size_t N_fruits = fruits.size();

    std::vector<bool> birds_alive(N_birds, true);

    if(nb_threads>0)
      omp_set_num_threads(nb_threads) ;

    #pragma omp parallel
    {
      
      #pragma omp for
      
      for (std::size_t i=0; i<N_predators; ++i) {
	predators[i].update(obstacles, predators, birds);
      }

      #pragma omp for
      for (std::size_t i=0; i<N_birds; i++) {
        
	bool is_alive = birds[i].update(obstacles, predators, birds, fruits);
	if (!is_alive){
	  birds_alive[i]=false;
	}
      }
      
      //the erase part is done in sequential because in parallel there would be problems with the index of birds

      int i_erased=0;
      for (std::size_t i=0; i<N_birds; i++){
	if(!birds_alive[i]){
	  birds.erase(i-i_erased);
	  i_erased+=1;
	}
      }

      #pragma omp for
      for (std::size_t i=0; i<N_trees; i++) {
        trees[i].DropFruitAndAppend(fruits);
      }
     
      #pragma omp for
      for (std::size_t i=0; i<N_fruits; i++)   {
	if (fruits[i].get_alive())
	  newFruits.push_back(fruits[i]);
	else {
	  size_t n = birds.size();
	  birds[n] = Bird(fruits[i].get_x(), fruits[i].get_y(), 0, n);
	}
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
        std::cout << fps << std::endl;
        total_fps += fps;
        start = std::chrono::high_resolution_clock::now();
        i = 0;
        j++;
    }
}
