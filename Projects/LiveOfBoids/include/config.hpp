#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <math.h>
#include <string>

// Simulation Configuration
struct SimulationConfig {
    int numAgents = 100;
    int numPredators = 2;
    int numPreys = 10;

    float cellSize = 50.0f;     // Grid cell size
    float minSpeed = 0.1f;      // Minimum speed of agents
    float maxSpeed = 0.5f;      // Maximum speed of agents
    float agentSize = 7.0f;     // Size of agents
    float range = 100.0f;       // Range of agents' influence
    float viewAngle = 3 * M_PI / 2; // View angle (radians)

    float alignmentWeight = 10.0f;  // Alignment rule weight
    float cohesionWeight = 1.0f;    // Cohesion rule weight
    float separationWeight = 2.0f; // Separation rule weight
    float avoidanceWeight = 6.0f;  // Avoidance rule weight
    float huntingWeight = 2.0f;    // Hunting rule weight
    float margin = 150.0f;         // Margin around the grid

    float colorMode = 1.0;
    float shaderType = 0.0;

    std::string mode = "default";
    int nbThreads = 0;
};

SimulationConfig parseArguments(int argc, char* argv[], SimulationConfig defaultConfig);
#endif // CONFIG_HPP
