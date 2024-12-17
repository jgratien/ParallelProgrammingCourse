#include "AgentFactory.hpp"

void AgentFactory::calculateRatio(){
    //m_ratio = std::max(std::min(std::max(1.2 * m_config.numAgents / 2000, 1.2), 1.2 * m_config.numAgents / 500), 1.1);
    m_ratio = 1;
}

void AgentFactory::calculateOffset(){
    m_offsetX = (m_width - m_ratio * m_width) / 2;
    m_offsetY = (m_height - m_ratio * m_height) / 2;
}

AgentFactory::AgentFactory()
    //: m_width(1080), m_height(720), m_config() ,m_context(agents::ShaderType){
    : m_width(1080), m_height(720), m_config(){

    calculateRatio();
    calculateOffset();
    createPreys();
    createBirds();
    createEagles();
}

AgentFactory::AgentFactory(SimulationConfig config)
    //: m_width(1080), m_height(720), m_config(config) , m_context(agents::ShaderType){
    : m_width(1080), m_height(720), m_config(config){

    calculateRatio();
    calculateOffset();
    createPreys();
    createBirds();
    createEagles();
}

AgentFactory::AgentFactory(float width, float height) 
    //: m_width(width), m_height(height), m_config(), m_context(agents::ShaderType) {
    : m_width(width), m_height(height), m_config() {

    calculateRatio();
    calculateOffset();
    createPreys();
    createBirds();
    createEagles();
}

AgentFactory::AgentFactory(float width, float height, SimulationConfig config) 
    //: m_width(width), m_height(height), m_config(config),m_context(agents::ShaderType) {
    : m_width(width), m_height(height), m_config(config){

    calculateRatio();
    calculateOffset();
    createPreys();
    createBirds();
    createEagles();
}

void AgentFactory::updateFactory(float width, float height){
    m_width = width;
    m_height = height;
    calculateRatio();
    calculateOffset();
}

float AgentFactory::ratio(){
    return m_ratio;
}

float AgentFactory::width(){
    return m_width;
}

float AgentFactory::height(){
    return m_height;
}

void AgentFactory::createBirds() {
    for (int i = 0; i < m_config.numAgents; ++i) {
        // Create each Bird dynamically and store as a shared_ptr<Agent>
        auto bird = std::make_shared<Bird>(
            randomGenerationOnScreen(m_width, m_height, m_ratio), 
            randomMovementGeneration(m_config.minSpeed, m_config.maxSpeed), 
            m_config.agentSize, 
            m_config.viewAngle, 
            m_config.range, 
            m_config.minSpeed, 
            m_config.maxSpeed, 
            m_config.alignmentWeight, 
            m_config.cohesionWeight, 
            m_config.separationWeight, 
            m_config.avoidanceWeight, 
            m_config.huntingWeight
        );

        // Add the shared_ptr<Bird> to the agents vector
        agents.push_back(bird);
    }
}


void AgentFactory::createEagles() {
    for (int i = 0; i < m_config.numPredators; ++i) {
        // Create each Eagle dynamically and store as a shared_ptr<Agent>
        auto eagle = std::make_shared<Eagle>(
            static_cast<int>(m_config.minSpeed) - 1, 
            static_cast<int>(m_config.maxSpeed) + 10, 
            m_width, m_height, m_ratio, 
            m_config.agentSize + 10, 
            m_config.viewAngle, 
            m_config.cohesionWeight + 1, 
            m_config.separationWeight + 2, 
            m_config.huntingWeight
        );

        // Add the shared_ptr<Eagle> to the agents vector
        agents.push_back(eagle);
    }
}

void AgentFactory::createPreys() {
    for (int i = 0; i < m_config.numPreys; ++i) {
        // Create each Prey dynamically and store as a shared_ptr<Agent>
        auto prey = std::make_shared<Prey>(
            static_cast<int>(m_config.minSpeed), 
            static_cast<int>(m_config.maxSpeed) - static_cast<int>(m_config.minSpeed), 
            m_width, 
            m_height, 
            m_ratio // Default constructor parameters
        );

        // Add the shared_ptr<Prey> to the agents vector
        agents.push_back(prey);
    }
}

std::vector<std::shared_ptr<Agent>> AgentFactory::getAllAgents() {
    
    return agents;
}

/*void AgentFactory::prepareScreen(){
    mat3x3 transform = vertex_transform_2d(m_width, m_height);
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT);
    m_context.initialize(GL_ARRAY_BUFFER);
    m_context.getAgentShader().setUniforms();
    m_context.getAgentShader().setParameters(transform, m_config.agentSize, m_config.maxSpeed);
    m_context.uploadData<agents::Vertex>(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
}

void AgentFactory::drawTriangles(std::vector<agents::Vertex> vertex_data_triangles){
    m_context.uploadData(GL_ARRAY_BUFFER, vertex_data_triangles.size() * sizeof(agents::Vertex), vertex_data_triangles.data(), GL_STREAM_DRAW);
    m_context.drawData(GL_TRIANGLES, vertex_data_triangles.size());
}

void AgentFactory::drawPoints(std::vector<agents::Vertex> vertex_data_dots){
    m_context.uploadData(GL_ARRAY_BUFFER, vertex_data_dots.size() * sizeof(agents::Vertex), vertex_data_dots.data(), GL_STREAM_DRAW);
    m_context.drawData(GL_POINTS, vertex_data_dots.size());
}

void AgentFactory::renderAgents() {
    std::vector<agents::Vertex> vertex_data_dots;
    std::vector<agents::Vertex> vertex_data_triangles;

    for (const auto& agent : agents) {
        // Dynamic cast to check if the agent is a Prey
        if (auto prey = dynamic_cast<Prey*>(agent.get())) {
            // Custom behavior for Prey
            vertex_data_dots.push_back(
                agents::Vertex{ {prey->getPosition().x, 
                prey->getPosition().y}, 
                {prey->getVelocity().x,
                prey->getVelocity().y}, 
                prey->getGLColor(), 
                m_config.colorMode, 
                m_config.shaderType});

        } else {
            // Default behavior for other agents
            mat2x6 result = mat2x6_triangle(
                agent.get()->getPosition() + Vec2(m_offsetX, m_offsetY), 
                agent.get()->getVelocity(), agent.get()->getBodySize());
			for (int j = 0; j < (int)result.size(); ++j) {
				vertex_data_triangles.push_back(
                    agents::Vertex{ result[j], 
                    {agent.get()->getVelocity().x,agent.get()->getVelocity().y},
                    agent.get()->getGLColor(), 
                    m_config.colorMode, 
                    m_config.shaderType});
			}
        }
    }

    prepareScreen();
    drawPoints(vertex_data_dots);
    drawTriangles(vertex_data_triangles);
}*/


