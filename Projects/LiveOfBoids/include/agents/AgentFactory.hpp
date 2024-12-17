#ifndef AGENT_FACTORY_HPP
#define AGENT_FACTORY_HPP

#include <vector>
#include <random>
#include <math.h>
#include "Bird.hpp"
#include "Eagle.hpp"
#include "Prey.hpp"
#include "../config.hpp"
#include "../utils/Vec2.hpp"
#include <memory>
//#include "../graphics/GraphicsContext.hpp"



class AgentFactory {
public:
    AgentFactory();
    AgentFactory(SimulationConfig config);
    AgentFactory(float width, float height, SimulationConfig config);
    AgentFactory(float width, float height);

    void updateFactory(float width, float height);

    float ratio();
    float width();
    float height();

    void createBirds();

    void createEagles();

    void createPreys();

    std::vector<std::shared_ptr<Agent>> getAllAgents();

    /*
    void renderAgents();
    void renderAllAgents();
    */

protected:
    float m_width, m_height, m_ratio, m_offsetX, m_offsetY;
    SimulationConfig m_config;
    //GraphicsContext m_context;
    std::vector<std::shared_ptr<Agent>> agents;
    void calculateRatio();
    void calculateOffset();
    /*void drawTriangles(std::vector<agents::Vertex> vertex_data_triangles);
    void drawPoints(std::vector<agents::Vertex> vertex_data_dots);
    void prepareScreen();*/
};

#endif
