#include "config.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

// Function to parse command-line arguments
SimulationConfig parseArguments(int argc, char* argv[], SimulationConfig defaultConfig) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--numAgents" && i + 1 < argc) {
            defaultConfig.numAgents = std::stoi(argv[++i]);
        } else if (arg == "--numPredators" && i + 1 < argc) {
            defaultConfig.numPredators = std::stoi(argv[++i]);
        } else if (arg == "--numPreys" && i + 1 < argc) {
            defaultConfig.numPreys = std::stoi(argv[++i]);
        }
        
        else if (arg == "--cellSize" && i + 1 < argc) {
            defaultConfig.cellSize = std::stof(argv[++i]);
        } else if (arg == "--minSpeed" && i + 1 < argc) {
            defaultConfig.minSpeed = std::stof(argv[++i]);
        } else if (arg == "--maxSpeed" && i + 1 < argc) {
            defaultConfig.maxSpeed = std::stof(argv[++i]);
        } else if (arg == "--agentSize" && i + 1 < argc) {
            defaultConfig.agentSize = std::stof(argv[++i]);
        } else if (arg == "--range" && i + 1 < argc) {
            defaultConfig.range = std::stof(argv[++i]);
        } else if (arg == "--viewAngle" && i + 1 < argc) {
            defaultConfig.viewAngle = std::stof(argv[++i]);
        }  
        
        else if (arg == "--alignmentWeight" && i + 1 < argc) {
            defaultConfig.alignmentWeight = std::stof(argv[++i]);
        } else if (arg == "--separationWeight" && i + 1 < argc) {
            defaultConfig.separationWeight = std::stof(argv[++i]);
        } else if (arg == "--cohesionWeight" && i + 1 < argc) {
            defaultConfig.cohesionWeight = std::stof(argv[++i]);
        } else if (arg == "--avoidanceWeight" && i + 1 < argc) {
            defaultConfig.avoidanceWeight = std::stof(argv[++i]);
        } else if (arg == "--huntingWeight" && i + 1 < argc) {
            defaultConfig.huntingWeight = std::stof(argv[++i]);
        } else if (arg == "--margin" && i + 1 < argc) {
            defaultConfig.margin = std::stof(argv[++i]);
        }  
        
        else if (arg == "--colorMode" && i + 1 < argc) {
            defaultConfig.colorMode = std::stof(argv[++i]);
        } else if (arg == "--shaderType" && i + 1 < argc) {
            defaultConfig.shaderType= std::stof(argv[++i]);
        }

        // Add more parameters as needed
    }
    return defaultConfig;
}
