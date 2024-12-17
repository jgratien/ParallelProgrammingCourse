#include "../include/agents/Eagle.hpp"

Eagle::Eagle() : Agent(-10, 10, 1080, 720, 1.2, 4, 270, 0.4, AgentType::Predator, Color::Red) {};

Eagle::Eagle(const int& lVelocity, const int& uVelocity, const float weight, const float height, const float ratio): 
	Agent(lVelocity, uVelocity, weight, height, ratio, 4, 270, 0.4, AgentType::Predator, Color::Red), m_cohesion(1.0), m_separation(2.0), m_hunting(1.0){};  

Eagle::Eagle(const int& lVelocity, const int& uVelocity, const float weight, const float height, const float ratio, const int& bodySize, const int& viewAngle, const float cohesionWeight, const float separationWeight, const float huntingWeight):
	Agent(lVelocity, uVelocity, weight, height, ratio, bodySize, viewAngle, 0.4, AgentType::Predator, Color::Red), m_cohesion(cohesionWeight), m_separation(separationWeight), m_hunting(huntingWeight){};

void Eagle::computeLaws(const std::vector<Agent*>& neighbors,const std::vector<Agent*>& predators,const std::vector<Agent*> preys) {
	Vec2 vecCohesion = m_cohesion.compute(*this, preys);
	Vec2 vecSeparation = m_separation.compute(*this, neighbors);
	Vec2 vec_displacement = vecCohesion + vecSeparation;

	m_hunting.kill(*this, preys);
	
	m_nextVelocity = m_velocity + vec_displacement; 
	m_nextVelocity.limit(m_minSpeed, m_maxSpeed);
};

void Eagle::print() const {
	std::cout << ">>>Eagle : Position(" << m_position.x << ", " << m_position.y << " ) / Velocity(" << m_velocity.x << ", " << m_velocity.y << " )";
};


