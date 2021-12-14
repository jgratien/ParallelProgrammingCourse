#ifndef __GNUC__
#pragma region declarations
#endif
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <omp.h>
#include <string>
#include <fstream>
#include "lib/myMath/Vec2.hpp"
#include "lib/myMath/utils.hpp"
#include "resources/model/Flock.hpp"
#include "resources/graphics/glx.hpp"
#include "resources/graphics/graphics.hpp"
#include "resources/graphics/GraphicalManager.hpp"
#include "resources/graphics/oglTypes.hpp"
#include "resources/controller/flock_generator.hpp"
#include "tbb/tbb.h"
#include "lib/myTimer/Timer.hpp"
#ifndef __GNUC__
#pragma endregion
#endif

Flock* MAIN_pFLOCK = nullptr;
std::vector<Agent*> mainFlock;
int nb_iters = 200;

void stadandardLoop(Flock * flock_ptr){
  long int t = 0;
  do {
      for (auto& bird : *flock_ptr) {
          std::tuple<std::vector<Agent*>, std::vector<Agent*>> allNeighbors =
              (*MAIN_pFLOCK).computeNeighbors(*bird); //this costs performance
          std::vector<Agent*> bVec = std::get<0>(allNeighbors);
          std::vector<Agent*> eVec = std::get<1>(allNeighbors);

          (*bird).computeLaws(bVec, eVec);
          (*bird).prepareMove();
          (*bird).setNextPosition(keepPositionInScreen((*bird).getNextPosition(), 800, 800));
          (*bird).move();
      }
      ++t;
  } while (t <= nb_iters);

}

int ompLoop(Flock * flock_ptr, int nb_threads) {

    long int t = 0;
    do {
        #pragma omp parallel for shared(flock_ptr) num_threads(nb_threads)
        for (int i=0; i< (*flock_ptr).getPopSize(); i++) {
            Agent * bird = (*flock_ptr)[i];
            std::tuple<std::vector<Agent*>, std::vector<Agent*>> allNeighbors =
                (*MAIN_pFLOCK).computeNeighbors(*bird); //this costs performance
            std::vector<Agent*> bVec = std::get<0>(allNeighbors);
            std::vector<Agent*> eVec = std::get<1>(allNeighbors);

            (*bird).computeLaws(bVec, eVec);
            (*bird).prepareMove();
            (*bird).setNextPosition(keepPositionInScreen((*bird).getNextPosition(), 800, 800));
            (*bird).move();
        }
        ++t;
    } while (t <= nb_iters);

    return 0;
}

int tbbLoop(Flock * flock_ptr, int nb_threads) {

    long int t = 0;
    tbb::task_scheduler_init init(nb_threads);
    do {

      tbb::parallel_for(
        tbb::blocked_range<int>(0,(*flock_ptr).getPopSize()), [&](tbb::blocked_range<int> r){
        for (int i=r.begin(); i < r.end(); i++) {
            Agent * bird = (*flock_ptr)[i];
            std::tuple<std::vector<Agent*>, std::vector<Agent*>> allNeighbors =
                (*MAIN_pFLOCK).computeNeighbors(*bird); //this costs performance
            std::vector<Agent*> bVec = std::get<0>(allNeighbors);
            std::vector<Agent*> eVec = std::get<1>(allNeighbors);

            (*bird).computeLaws(bVec, eVec);
            (*bird).prepareMove();
            (*bird).setNextPosition(keepPositionInScreen((*bird).getNextPosition(), 800, 800));
            (*bird).move();
          }
        });
        ++t;
    } while (t <= nb_iters);

    return 0;
}

int main() {
    std::ofstream outfile;
    int size = 600;
    mainFlock.reserve(size);
    std::vector<int> nb_threadsVector = {1,2,4,8,12,16,24};

    Flock flock = generate_parrot_flock(size);

    MAIN_pFLOCK = &flock;

    Timer timer0;
    {
      Sentry sentry(timer0, "Standard");
      stadandardLoop(MAIN_pFLOCK);
    }
    outfile.open("Benchmark_LOB.log", std::ios_base::app);
    outfile << "Standard Time: " << timer0.getTime(0) << std::endl;
    outfile.close();

    for (int& nb_threads : nb_threadsVector) {
      Timer timer;
      std::cout << nb_threads << std::endl;
    {
      Sentry sentry(timer, "OMP");
      ompLoop(MAIN_pFLOCK, nb_threads);
    }
    {
      Sentry sentry(timer, "Tbb");
      tbbLoop(MAIN_pFLOCK, nb_threads);
    }

    outfile.open("Benchmark_LOB.log", std::ios_base::app);
    outfile << "nb_threads: " << nb_threads << " OMP Time: " << timer.getTime(0)
            << " Tbb Time: " << timer.getTime(1)
            << std::endl;
    outfile.close();
  }

    return 0;
}
