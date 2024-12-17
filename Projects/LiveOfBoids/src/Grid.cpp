#include "Grid.hpp"
#include <cmath>
#include <algorithm>
#include <omp.h>
#include <tbb/tbb.h>
#include <unordered_map>

std::string modeToString(Mode mode) {
    switch (mode) {
        case Mode::Seq: return "seq";
        case Mode::Omp: return "omp";
        case Mode::Tbb: return "tbb";
        case Mode::Default: return "default";
        default: return "unknown";
    }
}

Mode stringToMode(const std::string& mode) {
    static const std::unordered_map<std::string, Mode> modeMap = {
        {"seq", Mode::Seq},
        {"omp", Mode::Omp},
        {"tbb", Mode::Tbb},
        {"default", Mode::Default},
    };

    auto it = modeMap.find(mode);
    if (it != modeMap.end()) {
        return it->second;
    }
    throw std::invalid_argument("Invalid mode. Allowed values: seq, omp, tbb.");
}

Grid::Grid(int width, int height, float cellSize, float margin, std::string mode, int nb_threads) 
    : gridWidth(static_cast<int>(width / cellSize)), gridHeight(static_cast<int>(height / cellSize)), cellSize(cellSize), margin(margin), mode(stringToMode(mode)), nbThreads(nb_threads){
        cells.resize(gridWidth, std::vector<std::vector<Agent*>>(gridHeight));
}

// Clear all points from cells (call at each frame start)
void Grid::clear() {
    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            cells[x][y].clear();
        }
    }
}

// Add a agent to the appropriate cell
void Grid::addBoid(Agent* agent) {
    int cellX = static_cast<int>(agent->getPosition().x / cellSize);
    int cellY = static_cast<int>(agent->getPosition().y / cellSize);

    if (cellX >= 0 && cellX < gridWidth && cellY >= 0 && cellY < gridHeight) {
        cells[cellX][cellY].push_back(agent);

    }
}

std::vector<Agent*> Grid::findNeighbors(Agent* agent) {
        int cellX = static_cast<int>(agent->getPosition().x / cellSize) % gridWidth;
        int cellY = static_cast<int>(agent->getPosition().y / cellSize) % gridHeight;

        if (cellX < 0) cellX += gridWidth;
        if (cellY < 0) cellY += gridHeight;

        std::vector<Agent*> neighbors;
        int searchRadius = static_cast<int>(std::ceil(agent -> getRange() / cellSize));

        // Loop over cells within the search range
        for (int xOffset = -searchRadius; xOffset <= searchRadius; ++xOffset) {
            for (int yOffset = -searchRadius; yOffset <= searchRadius; ++yOffset) {
                // Wrap cell indices
                int neighborX = (cellX + xOffset + gridWidth) % gridWidth;
                int neighborY = (cellY + yOffset + gridHeight) % gridHeight;

                // Check points in the neighboring cell
                for (Agent* other : cells[neighborX][neighborY]) {
                    std::vector<Agent*> obstacles;
                    if (agent != other && agent->getPosition().wrappedDistanceTo(other->getPosition(), gridWidth, gridHeight) <= agent -> getRange() ) {
                    //if (agent != other && agent->getPosition().distanceTo(other->getPosition()) <= agent -> getRange() ) {
                        
                        Vec2 relativePosition = other->getPosition() - agent->getPosition();
                        float angle = std::acos(agent->getVelocity().normalized().dot(relativePosition.normalized()));
                        
                        if (angle <= agent -> getViewAngle() / 2) {
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


void Grid::updateBoids(){
    if(nbThreads>0)
    {
        omp_set_num_threads(nbThreads) ;
        tbb::task_scheduler_init init(nbThreads);
    }
    switch (mode)
    {
    case Mode::Seq:
        updateBoidsSeq();
    case Mode::Omp:
        updateBoidsWithOMP();
    case Mode::Tbb:
        updateBoidsWithTBB();
    default:
        updateBoidsSeq();
    }
    //std::cout<<"Mode: "<<modeToString(mode)<<std::endl;
}

// Update all agents based on the rules
void Grid::updateBoidsSeq() {

    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            auto& cell = cells[x][y];
            for (Agent* agent : cell) {
                auto [neighbors, predators, prey] = findNeighborsPredatorsAndPrey(agent);
                
                agent->computeLaws(neighbors, predators, prey);
                if(agent -> getDestruction()){
                    cell.erase(std::remove(cell.begin(), cell.end(), agent), cell.end());
                    agent -> setGLColor(Color::Grey);
                }
                agent->prepareMove();
            }
        }
    }

    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            for (Agent* agent : cells[x][y]) {
                agent->move();
                wrapPosition(agent);                
            }
        }
    }
}

void Grid::updateBoidsWithOMP() {

    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            auto& cell = cells[x][y];
            for (Agent* agent : cell) {
                auto [neighbors, predators, prey] = findNeighborsPredatorsAndPrey(agent);
                agent->computeLaws(neighbors, predators, prey);

                // Mark agents for destruction in a thread-safe way
                if (agent->getDestruction()) {
                    agent->setGLColor(Color::Grey);
                    #pragma omp critical
                    cell.erase(std::remove(cell.begin(), cell.end(), agent), cell.end());
                }
                agent->prepareMove();
            }
        }
    }

    // Second phase: Moving agents
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            auto& cell = cells[x][y];
            for (Agent* agent : cell) {
                agent->move();
                wrapPosition(agent);
            }
        }
    }

}

void Grid::updateBoidsWithTBB() {
    tbb::parallel_for(tbb::blocked_range2d<int>(0, gridWidth, 0, gridHeight),
        [this](const tbb::blocked_range2d<int>& range) {
            for (int x = range.rows().begin(); x < range.rows().end(); ++x) {
                for (int y = range.cols().begin(); y < range.cols().end(); ++y) {
                    auto& cell = cells[x][y];
                    for (Agent* agent : cell) {
                        auto [neighbors, predators, prey] = findNeighborsPredatorsAndPrey(agent);
                        
                        agent->computeLaws(neighbors, predators, prey);
                        if (agent->getDestruction()) {
                            cell.erase(std::remove(cell.begin(), cell.end(), agent), cell.end());
                            agent->setGLColor(Color::Grey);
                        }
                        agent->prepareMove();
                    }
                }
            }
        });

    tbb::parallel_for(tbb::blocked_range2d<int>(0, gridWidth, 0, gridHeight),
        [this](const tbb::blocked_range2d<int>& range) {
            for (int x = range.rows().begin(); x < range.rows().end(); ++x) {
                for (int y = range.cols().begin(); y < range.cols().end(); ++y) {
                    for (Agent* agent : cells[x][y]) {
                        agent->move();
                        wrapPosition(agent);
                    }
                }
            }
        });
}

void Grid::wrapPosition(Agent* agent) {
   float screenWidth =  gridWidth * cellSize ;
    float screenHeight = gridHeight * cellSize ;

    // Wrap horizontally with continuous movement
    if (agent->getPosition().x < 0) {
        agent->setPosition(Vec2{agent->getPosition().x + screenWidth, agent->getPosition().y}); 
    } else if (agent->getPosition().x >= screenWidth) {
        agent->setPosition(Vec2{agent->getPosition().x - screenWidth, agent->getPosition().y});
    }

    // Wrap vertically with continuous movement
    if (agent->getPosition().y < 0) {
        agent->setPosition(Vec2{agent->getPosition().x, agent->getPosition().y + screenHeight});
    } else if (agent->getPosition().y >= screenHeight) {
        agent->setPosition(Vec2{agent->getPosition().x, agent->getPosition().y - screenHeight});
    }
}

void Grid::bounceOffEdge(Agent* agent) {
    // Adjust velocity based on proximity to grid edges
    if (agent->getPosition().x < margin) {
        agent->setNextVelocity(Vec2{agent->getNextVelocity().x + agent->getTurnFactor(), agent->getNextVelocity().y}.normalized());
    }
    if (agent->getPosition().x > gridWidth * cellSize - margin) {
        agent->setNextVelocity(Vec2{agent->getNextVelocity().x - agent->getTurnFactor(), agent->getNextVelocity().y}.normalized());
    }
    if (agent->getPosition().y > margin) {
        agent->setNextVelocity(Vec2{agent->getNextVelocity().x , agent->getNextVelocity().y - agent->getTurnFactor()}.normalized());
    }
    if (agent->getPosition().y < gridHeight * cellSize - margin) {
        agent->setNextVelocity(Vec2{agent->getNextVelocity().x , agent->getNextVelocity().y + agent->getTurnFactor()}.normalized());
    }
    
}

