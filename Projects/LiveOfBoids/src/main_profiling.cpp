#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <omp.h>
#include <vector>

#include "lib/myMath/Vec2.hpp"
#include "lib/myMath/utils.hpp"
#include "resources/model/Flock.hpp"
#include "resources/graphics/glx.hpp"
#include "resources/graphics/graphics.hpp"
#include "resources/graphics/GraphicalManager.hpp"
#include "resources/graphics/oglTypes.hpp"
#include "resources/controller/flock_generator.hpp"
#include "lib/Timers/timers.hpp"

#define NB_THREADS 4


Flock* MAIN_pFLOCK = nullptr;
std::vector<Agent*> mainFlock;

int main() {

    int size = 1000;
    mainFlock.reserve(size);

    PPTP::Timer timer;



    Flock flock = generate_parrot_flock(size);


    MAIN_pFLOCK = &flock;
    {
        PPTP::Timer::Sentry sentry(timer, "Sequentiel");
        long int t = 0;
        do {

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
        } while (t <= 100);
    }

    {
        PPTP::Timer::Sentry sentry(timer, "OMP");
        long int t = 0;
        do {
            std::cout << "Tour " << t << '\n';

            #pragma omp parallel for shared(MAIN_pFLOCK) num_threads(4)
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
        } while (t <= 100);
    }

    timer.printInfo();

    return 0;
}
