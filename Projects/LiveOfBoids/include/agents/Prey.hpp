#pragma once
#include <vector>
#include <array>

#include "Agent.hpp"
#include "../laws/Cohesion.hpp"
#include "../laws/Separation.hpp"

class Prey : public Agent{
private:
	Cohesion m_cohesion;
	Separation m_separation;

public:
	Prey();
	Prey(const int& lVelocity, const int& uVelocity, const float weight, const float height, const float ratio);
	Prey(const int& lVelocity, const int& uVelocity, const float weight, const float height, const float ratio, const int& bodySize, const int& viewAngle, const float cohesionWeight, const float separationWeight);

	void computeLaws(const std::vector<Agent*>& neighbors,const std::vector<Agent*>& predators,const std::vector<Agent*> preys);
	void print() const;

};