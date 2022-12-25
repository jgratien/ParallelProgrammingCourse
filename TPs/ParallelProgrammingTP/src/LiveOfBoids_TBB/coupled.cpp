#include "main.h"
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

    tbb::parallel_for( int(0), nb_threads, [&]( int i )
	{

	  for (std::size_t j = i*N_predators/nb_threads; j<(i+1)*N_predators/nb_threads; ++j) {
	    predators[j].update(obstacles, predators, birds);
	  }

	}

		       );


    tbb::parallel_for( int(0), nb_threads, [&]( int i )
		       {

			 for (std::size_t j = i*N_birds/nb_threads; j<(i+1)*N_birds/nb_threads; ++j) {
        
			   bool is_alive = birds[j].update(obstacles, predators, birds, fruits);
			   if (!is_alive){
			     birds_alive[j]=false;
			   }

			 }

		       }
		       
		       );

    int i_erased=0;
    for(std::size_t i=0; i<N_birds; ++i){
      if(!birds_alive[i]){
	birds.erase(i);
	i_erased+=1;
      }
    }

    tbb::parallel_for( int(0), nb_threads, [&]( int i )
		       {
			 for (std::size_t j = i*N_trees/nb_threads; j<(i+1)*N_trees/nb_threads; ++j) {
			   trees[j].DropFruitAndAppend(fruits);
			 }
		       }
		       );


    tbb::parallel_for( int(0), nb_threads, [&]( int i )
		       {
			 for (std::size_t j = i*N_fruits/nb_threads; j<(i+1)*N_fruits/nb_threads; ++j) {
			   if (fruits[j].get_alive())
			     newFruits.push_back(fruits[j]);
			   else {
			     size_t n = birds.size();
			     birds[n] = Bird(fruits[j].get_x(), fruits[j].get_y(), 0, n);
			   }
			 }

		       }
		       );

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
