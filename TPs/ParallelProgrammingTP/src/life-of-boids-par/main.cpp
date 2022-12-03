#include "main.h"

// agent addition params
bool addBird = false;
bool addPredator = false;
double cursorX = 0;
double cursorY = 0;

int main() {
    // Agents initialization
    vars::agentWindowVars_t var = initAgentWindow();
    int count(0);

    // Global loop
    int count_FPS = 0, count_LOOP = 0;
    double total_FPS = 0.f;
    auto start = std::chrono::high_resolution_clock::now();

    while (true) {
        // Update all the objects by applying the laws.
        var.fruits = updateObjects(var.obstacles, var.predators, var.birds, var.trees, var.fruits);
        display_FPS(start, total_FPS, count_FPS, count_LOOP);
    }
}