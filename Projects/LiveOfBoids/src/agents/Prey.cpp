#include "../include/agents/Prey.hpp"

Prey::Prey() : Agent(0, 0, 1080, 720, 1.2, 4, 0, 0.0, AgentType::Prey, Color::Green) {};

Prey::Prey(const int& lVelocity, const int& uVelocity, const float weight, const float height, const float ratio): 
	Agent(lVelocity, uVelocity, weight, height, ratio, 4, 0, 0.0, AgentType::Prey, Color::Green), m_cohesion(1.0), m_separation(2.0){
        m_nextPosition = m_position;
        m_nextVelocity = m_velocity;
    };  // Range = Cohesion Range = Allignment Range = 30 

Prey::Prey(const int& lVelocity, const int& uVelocity, const float weight, const float height, const float ratio, const int& bodySize, const int& viewAngle, const float cohesionWeight, const float separationWeight):
	Agent(lVelocity, uVelocity, weight, height, ratio, bodySize, viewAngle, 0.0, AgentType::Prey, Color::Green), m_cohesion(cohesionWeight), m_separation(separationWeight){};


void Prey::computeLaws(const std::vector<Agent*>& neighbors,const std::vector<Agent*>& predators,const std::vector<Agent*> preys) {

};

void Prey::print() const {
	std::cout << ">>>Prey : Position(" << m_position.x << ", " << m_position.y << " ) / Velocity(" << m_velocity.x << ", " << m_velocity.y << " )";
};