#pragma once
#include <vector>
#include <array>

#include "Agent.hpp"
#include "../laws/Cohesion.hpp"
#include "../laws/Separation.hpp"
#include "../laws/Hunting.hpp"

class Eagle : public Agent{
private:
	Cohesion m_cohesion;
	Separation m_separation;
	Hunting m_hunting;

public:
	Eagle();
	Eagle(const int& lVelocity, const int& uVelocity, const float weight, const float height, const float ratio);
	Eagle(const int& lVelocity, const int& uVelocity, const float weight, const float height, const float ratio, const int& bodySize, const int& viewAngle, const float cohesionWeight, const float separationWeight, const float huntingWeight);

	void computeLaws(const std::vector<Agent*>& neighbors,const std::vector<Agent*>& predators,const std::vector<Agent*> preys);
	void print() const;

};