#include "../include/agents/Bird.hpp"

Bird::Bird() : Agent(-10, 10, 4, 720, 1.2, 4, 270, 0.4, AgentType::Neutral), m_cohesion(1.0), m_alignment(3.0), m_separation(1.0), m_avoidance(1.0), m_hunting(1.0){}; // View Angle 270° Par défault

Bird::Bird(const Vec2& position, const Vec2& velocity) : 
	Agent(position, velocity, 5, 270, 100, 1, 1, 0.4, AgentType::Neutral), m_cohesion(1.0), m_alignment(3.0), m_separation(1.0), m_avoidance(1.0), m_hunting(1.0){};  // Range = Cohesion Range = Allignment Range = 30 

Bird::Bird(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float& minSpeed, const float& maxSpeed,  const float cohesionWeight, const float alignmentWeight, const float separationWeight, const float avoidanceWeight, const float huntingWeight):
	Agent(position, velocity, bodySize, viewAngle, range, minSpeed, maxSpeed, 0.4, AgentType::Neutral), m_cohesion(cohesionWeight), m_alignment(alignmentWeight), m_separation(separationWeight), m_avoidance(avoidanceWeight), m_hunting(huntingWeight){
};

Bird::Bird(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float& minSpeed, const float& maxSpeed) :
	Bird(position, velocity, bodySize, viewAngle, range, minSpeed, maxSpeed, 1.0, 3.0, 1.0, 1.0, 1.0){
};

Bird::Bird(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float& minSpeed, const float& maxSpeed, const Cohesion& cohesion, const Alignment& alignment, const Separation& separation, const Avoidance& avoidance, const Hunting& hunting) :
	Bird(position, velocity, bodySize, viewAngle, range, minSpeed, maxSpeed) {
	m_cohesion = cohesion;
	m_alignment = alignment;
	m_separation = separation;
};


void Bird::computeLaws(const std::vector<Agent*>& neighbors,const std::vector<Agent*>& predators,const std::vector<Agent*> preys) {
	Vec2 vecCohesion = m_cohesion.compute(*this, neighbors) + m_cohesion.compute(*this, preys);
	Vec2 vecAlignment = m_alignment.compute(*this, neighbors);
	Vec2 vecSeparation = m_separation.compute(*this, neighbors);
	Vec2 vecAvoidance = m_avoidance.compute(*this, predators);
	Vec2 vecHunting = m_hunting.compute(*this, preys);
	Vec2 vec_displacement = vecAlignment + vecCohesion + vecSeparation + vecAvoidance + vecHunting;

	m_hunting.kill(*this, preys);

	m_nextVelocity = m_velocity + vec_displacement; 
	m_nextVelocity.limit(m_minSpeed, m_maxSpeed);
};

void Bird::print() const {
	std::cout << ">>>Bird : Position(" << m_position.x << ", " << m_position.y << " ) / Velocity(" << m_velocity.x << ", " << m_velocity.y << " )";
};


