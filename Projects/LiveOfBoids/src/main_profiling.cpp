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
#include "lib/myMath/Vec2.hpp"
#include "lib/myMath/utils.hpp"
#include "resources/model/Flock.hpp"
#include "resources/graphics/glx.hpp"
#include "resources/graphics/graphics.hpp"
#include "resources/graphics/GraphicalManager.hpp"
#include "resources/graphics/oglTypes.hpp"
#include "resources/controller/flock_generator.hpp"
#ifndef __GNUC__
#pragma endregion
#endif

Flock* MAIN_pFLOCK = nullptr;
std::vector<Agent*> mainFlock;

int main() {

    int size = 600;
    mainFlock.reserve(size);



    Flock flock = generate_parrot_flock(size);


    MAIN_pFLOCK = &flock;

    long int t = 0;
    do {
        std::cout << "Tour " << t << '\n';
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

    return 0;
}

