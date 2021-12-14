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
#include "lib/myMath/Vec2.hpp"
#include "lib/myMath/utils.hpp"
#include "resources/model/Flock.hpp"
#include "resources/graphics/glx.hpp"
#include "resources/graphics/graphics.hpp"
#include "resources/graphics/GraphicalManager.hpp"
#include "resources/graphics/oglTypes.hpp"
#include "resources/controller/flock_generator.hpp"
#include "lib/myTimer/Timer.hpp"
#include "tbb/tbb.h"
#include <string>

#ifndef __GNUC__
#pragma endregion
#endif

Flock* MAIN_pFLOCK = nullptr;
std::vector<Agent*> mainFlock;

#define LOOP_SIZE 1
#define FLOCK_SIZE 1000
// #define NB_THREADS 16

int main() {
    Timer timer;

    //std::vector<int> sizes = { 1,10,100,250,500,1000,2000,3000,5000,8000,10000 };
     std::vector<int> sizes = {1,10,100,250,500};

    std::vector<int> nb_threads = {1,16,24};
    //std::vector<int> nb_threads = { 1,2,4,8,12,16,24 };
    int i = 0;
    int end = sizes.size();
    for (int sz : sizes) {
        std::cout << i << " / " << end << std::endl;
        std::string str(std::to_string(sz) + "_");

        Flock flock = generate_parrot_flock(sz);

        // std::cout << "FLOCK SIZE=" << FLOCK_SIZE << "\nNB_THREADS=" << NB_THREADS << std::endl;


        MAIN_pFLOCK = &flock;

        {
            Sentry sentry(timer, str + "Seq");
            long int t = 0;
            do {
                // std::cout << "Tour " << t << '\n';
                for (auto& bird : *MAIN_pFLOCK) {
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
            } while (t <= LOOP_SIZE);
        }

        for (int nbth : nb_threads) {

            {
                Sentry sentry(timer, str + std::to_string(nbth) + "_""OMP");
                long int t = 0;
                do {
                    #pragma omp parallel for shared(MAIN_pFLOCK) num_threads(nbth)
                    for (int i = 0; i < (*MAIN_pFLOCK).getPopSize(); ++i) {
                        // std::cout << omp_get_num_threads() << std::endl;

                        Agent* bird = (*MAIN_pFLOCK)[i];
                        std::tuple<std::vector<Agent*>, std::vector<Agent*>> allNeighbors =
                            (*MAIN_pFLOCK).computeNeighbors(*bird);
                        std::vector<Agent*> bVec = std::get<0>(allNeighbors);
                        std::vector<Agent*> eVec = std::get<1>(allNeighbors);

                        (*bird).computeLaws(bVec, eVec);
                        (*bird).prepareMove();
                        (*bird).setNextPosition(keepPositionInScreen((*bird).getNextPosition(), 800, 800));
                        (*bird).move();
                    }
                    ++t;
                } while (t <= LOOP_SIZE);
            }

            {
                Sentry sentry(timer, str + std::to_string(nbth) + "_""OMP_Task");
                long int t = 0;
                do {
                    #pragma omp parallel shared(MAIN_pFLOCK) num_threads(nbth)
                    {
                        #pragma omp single
                        {
                            for (int i = 0; i < (*MAIN_pFLOCK).getPopSize(); ++i) {
                                #pragma omp task
                                {
                                    //std::cout << omp_get_num_threads() << std::endl;

                                    Agent* bird = (*MAIN_pFLOCK)[i];
                                    std::tuple<std::vector<Agent*>, std::vector<Agent*>> allNeighbors =
                                        (*MAIN_pFLOCK).computeNeighbors(*bird);
                                    std::vector<Agent*> bVec = std::get<0>(allNeighbors);
                                    std::vector<Agent*> eVec = std::get<1>(allNeighbors);

                                    (*bird).computeLaws(bVec, eVec);
                                    (*bird).prepareMove();
                                    (*bird).setNextPosition(keepPositionInScreen((*bird).getNextPosition(), 800, 800));
                                    (*bird).move();
                                }
                            }
                        #pragma omp taskwait
                        }
                    }
                    ++t;
                } while (t <= LOOP_SIZE);
            }

            {
                Sentry sentry(timer, str + std::to_string(nbth) + "_" + "TBB");
                long int t = 0;
                do {
                    // tbb::task_scheduler_init(NB_THREADS);
                    tbb::task_arena arena(nbth);

                    arena.execute([] {
                        tbb::parallel_for(size_t(0), (size_t)(*MAIN_pFLOCK).getPopSize(),
                            [&](size_t i) {
                                Agent* bird = (*MAIN_pFLOCK)[i];
                                std::tuple<std::vector<Agent*>, std::vector<Agent*>> allNeighbors =
                                    (*MAIN_pFLOCK).computeNeighbors(*bird); //this costs performance
                                std::vector<Agent*> bVec = std::get<0>(allNeighbors);
                                std::vector<Agent*> eVec = std::get<1>(allNeighbors);

                                (*bird).computeLaws(bVec, eVec);
                                (*bird).prepareMove();
                                (*bird).setNextPosition(keepPositionInScreen((*bird).getNextPosition(), 800, 800));
                                (*bird).move();
                            });
                        });


                    ++t;
                } while (t <= LOOP_SIZE);
            }

            {
                Sentry sentry(timer, str + std::to_string(nbth) + "_" + "TBB_Range");
                long int t = 0;
                do {
                    // tbb::task_scheduler_init(NB_THREADS);
                    tbb::task_arena arena(nbth);

                    arena.execute([] {
                        //tbb::parallel_for(size_t(0), (size_t)(*MAIN_pFLOCK).getPopSize(),
                        //    [&](size_t i) {
                        tbb::parallel_for(tbb::blocked_range<int>(0, (size_t)(*MAIN_pFLOCK).getPopSize()),
                            [&](tbb::blocked_range<int> const r){
                                for (auto irow = r.begin(); irow < r.end(); ++irow) {
                                    Agent* bird = (*MAIN_pFLOCK)[irow];
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
                        });


                    ++t;
                } while (t <= LOOP_SIZE);
            }
        }
    }


    timer.printInfo();
    return 0;
}

