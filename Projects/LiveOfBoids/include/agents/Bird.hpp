#pragma once
#include <vector>
#include <array>

#include "Agent.hpp"
#include "../laws/Separation.hpp"
#include "../laws/Cohesion.hpp"
#include "../laws/Alignment.hpp"
#include "../laws/Avoidance.hpp"
#include "../laws/Hunting.hpp"



class Bird : public Agent{
private:
	Cohesion m_cohesion;
	Alignment m_alignment;
	Separation m_separation;
	Avoidance m_avoidance;
	Hunting m_hunting;

public:
	Bird();
	Bird(const Vec2& position, const Vec2& velocity);
	Bird(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float& minSpeed, const float& maxSpeed, const Cohesion &cohesion, const Alignment &alignment, const Separation &separation, const Avoidance &avoidance, const Hunting &hunting);
	Bird(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float& minSpeed, const float& maxSpeed,  const float cohesionWeight, const float alignmentWeight, const float separationWeight, const float avoidanceWeight, const float huntingWeight);
	Bird(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float& minSpeed, const float& maxSpeed);

	//void computeLaws(const std::vector<Agent*>& neighborsBird) override;
	void computeLaws(const std::vector<Agent*>& neighbors,const std::vector<Agent*>& predators,const std::vector<Agent*> preys);
	void print() const;

};