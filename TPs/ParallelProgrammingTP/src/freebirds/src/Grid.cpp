#include "Grid.hpp"
#include <cmath>
#include <algorithm>

#define SEQUENTIAL 0
#define OPENMP 1
#define TBB 2

// Include tbb and omp
#include "tbb/parallel_for.h"
#include <tbb/blocked_range.h>
#include <omp.h>

Grid::Grid(int width, int height, float cellSize, float margin) 
    : gridWidth(static_cast<int>(width / cellSize)), gridHeight(static_cast<int>(height / cellSize)), cellSize(cellSize), margin(margin) {
        cells.resize(gridWidth, std::vector<std::vector<Agent*>>(gridHeight));
}

// Clear all points from cells
void Grid::clear() {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            cells[x][y].clear();
        }
    }
}

// Add agent to appropriate cell
void Grid::addBoid(Agent* agent) {
    int cellX = static_cast<int>(agent->getPosition().x / cellSize);
    int cellY = static_cast<int>(agent->getPosition().y / cellSize);

    if (cellX >= 0 && cellX < gridWidth && cellY >= 0 && cellY < gridHeight) {
        cells[cellX][cellY].push_back(agent);
    }
}

// Find neighbors for an agent
std::vector<Agent*> Grid::findNeighbors(Agent* agent) {
    int cellX = static_cast<int>(agent->getPosition().x / cellSize) % gridWidth;
    int cellY = static_cast<int>(agent->getPosition().y / cellSize) % gridHeight;

    if (cellX < 0) cellX += gridWidth;
    if (cellY < 0) cellY += gridHeight;

    std::vector<Agent*> neighbors;
    int searchRadius = static_cast<int>(std::ceil(agent->getRange() / cellSize));

    // Loop over cells within search range
    for (int xOffset = -searchRadius; xOffset <= searchRadius; ++xOffset) {
        for (int yOffset = -searchRadius; yOffset <= searchRadius; ++yOffset) {
            int neighborX = (cellX + xOffset + gridWidth) % gridWidth;
            int neighborY = (cellY + yOffset + gridHeight) % gridHeight;

            for (Agent* other : cells[neighborX][neighborY]) {
                if (agent != other && agent->getPosition().wrappedDistanceTo(other->getPosition(), gridWidth, gridHeight) <= agent->getRange()) {
                    Vec2 relativePosition = other->getPosition() - agent->getPosition();
                    float angle = std::acos(agent->getVelocity().normalized().dot(relativePosition.normalized()));

                    if (angle <= agent->getViewAngle() / 2) {
                        neighbors.push_back(other);
                    }
                }
            }
        }
    }
    return neighbors;
}

std::tuple<std::vector<Agent*>, std::vector<Agent*>, std::vector<Agent*>> 
Grid::findNeighborsPredatorsAndPrey(Agent* agent) {
    int cellX = static_cast<int>(agent->getPosition().x / cellSize) % gridWidth;
    int cellY = static_cast<int>(agent->getPosition().y / cellSize) % gridHeight;

    if (cellX < 0) cellX += gridWidth;
    if (cellY < 0) cellY += gridHeight;

    std::vector<Agent*> neighbors;
    std::vector<Agent*> predators;
    std::vector<Agent*> prey;
    int searchRadius = static_cast<int>(std::ceil(agent->getRange() / cellSize));

    // Loop over cells within the search range
    for (int xOffset = -searchRadius; xOffset <= searchRadius; ++xOffset) {
        for (int yOffset = -searchRadius; yOffset <= searchRadius; ++yOffset) {
            // Wrap cell indices
            int neighborX = (cellX + xOffset + gridWidth) % gridWidth;
            int neighborY = (cellY + yOffset + gridHeight) % gridHeight;

            // Check points in the neighboring cell
            for (Agent* other : cells[neighborX][neighborY]) {
                if (agent != other && not (agent->getDestruction() && other->getDestruction())) {
                    float distance = agent->getPosition().wrappedDistanceTo(
                        other->getPosition(), gridWidth, gridHeight);

                    if (distance <= agent->getRange()) {
                        Vec2 relativePosition = other->getPosition() - agent->getPosition();
                        float angle = std::acos(agent->getVelocity().normalized()
                                                    .dot(relativePosition.normalized()));

                        if (angle <= agent->getViewAngle() / 2) {
                            // Classify the agent as neighbor, predator, or prey
                            if (typeid(*other) == typeid(*agent)) {
                                neighbors.push_back(other);
                            } else if (agent->getType() == AgentType::Neutral && other->getType() == AgentType::Predator) {
                                predators.push_back(other);
                            } else if ((agent->getType() == AgentType::Neutral && other->getType() == AgentType::Prey) |
                             (agent->getType() == AgentType::Predator && other->getType() == AgentType::Neutral)) {
                                prey.push_back(other);
                            }
                        }
                    }
                }
            }
        }
    }

    return std::make_tuple(neighbors, predators, prey);
}


// Update all agents based on the rules
void Grid::updateBoids(int mode) {
    if (mode != SEQUENTIAL && mode != OPENMP && mode != TBB) {
        throw std::invalid_argument("Invalid mode");
    }

    auto updateCell = [this](auto& cell) {
        for (auto it = cell.begin(); it != cell.end();) {
            Agent* agent = *it;
            auto [neighbors, predators, prey] = findNeighborsPredatorsAndPrey(agent);

            agent->computeLaws(neighbors, predators, prey);
            if (agent->getDestruction()) {
                it = cell.erase(it);
            } else {
                agent->prepareMove();
                ++it;
            }
        }
    };

    // Sequential Update
    if (mode == SEQUENTIAL) {
        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                updateCell(cells[x][y]);
            }
        }
    }
    // OpenMP Parallel Update
    else if (mode == OPENMP) {
        #pragma omp parallel for collapse(2) schedule(static)
        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                updateCell(cells[x][y]);
            }
        }
    }
    // TBB Parallel Update
    else if (mode == TBB) {
        tbb::parallel_for(tbb::blocked_range<int>(0, gridWidth),
                          [&](const tbb::blocked_range<int>& r) {
                              for (int x = r.begin(); x != r.end(); ++x) {
                                  for (int y = 0; y < gridHeight; ++y) {
                                      updateCell(cells[x][y]);
                                  }
                              }
                          });
    }

    // Movement update (same for all modes)
    auto moveAgents = [this](auto& cell) {
        for (Agent* agent : cell) {
            agent->move();
            wrapPosition(agent);
        }
    };

    if (mode == SEQUENTIAL) {
        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                moveAgents(cells[x][y]);
            }
        }
    } else if (mode == OPENMP) {
        #pragma omp parallel for collapse(2) schedule(static)
        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                moveAgents(cells[x][y]);
            }
        }
    } else if (mode == TBB) {
        tbb::parallel_for(tbb::blocked_range<int>(0, gridWidth),
                          [&](const tbb::blocked_range<int>& r) {
                              for (int x = r.begin(); x != r.end(); ++x) {
                                  for (int y = 0; y < gridHeight; ++y) {
                                      moveAgents(cells[x][y]);
                                  }
                              }
                          });
    }
}

void Grid::wrapPosition(Agent* agent) {
    float screenWidth = gridWidth * cellSize;
    float screenHeight = gridHeight * cellSize;

    // Wrap positions
    if (agent->getPosition().x < 0) {
        agent->setPosition(Vec2{agent->getPosition().x + screenWidth, agent->getPosition().y}); 
    } else if (agent->getPosition().x >= screenWidth) {
        agent->setPosition(Vec2{agent->getPosition().x - screenWidth, agent->getPosition().y});
    }

    if (agent->getPosition().y < 0) {
        agent->setPosition(Vec2{agent->getPosition().x, agent->getPosition().y + screenHeight});
    } else if (agent->getPosition().y >= screenHeight) {
        agent->setPosition(Vec2{agent->getPosition().x, agent->getPosition().y - screenHeight});
    }
}
