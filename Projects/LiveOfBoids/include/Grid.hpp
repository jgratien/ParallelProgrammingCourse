#ifndef GRID_H
#define GRID_H

#include <vector>
#include "./agents/Agent.hpp"
#include <string>

enum class Mode { Seq, Omp, Tbb, Default};

class Grid {
private:
    int gridWidth, gridHeight, nbThreads; // Number of cells in each dimension
    float cellSize, margin;            // Size of each cell
    Mode mode;
    std::vector<std::vector<std::vector<Agent*>>> cells;

public:
    Grid(int width, int height, float cellSize, float margin, std::string mode, int nb_threads);
    void clear();
    void addBoid(Agent* agent);
    std::vector<Agent*> findNeighbors(Agent* agent);
    std::tuple<std::vector<Agent*>, std::vector<Agent*>, std::vector<Agent*>> findNeighborsPredatorsAndPrey(Agent* agent);
    void updateBoids();
    void updateBoidsSeq();
    void updateBoidsWithOMP();
    void updateBoidsWithTBB();
    void wrapPosition(Agent* agent);
    void bounceOffEdge(Agent* agent);
};

#endif // GRID_H
