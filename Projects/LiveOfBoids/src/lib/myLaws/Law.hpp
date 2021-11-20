#pragma once
#include "../myMath/Vec2.hpp"
#include "../../resources/model/Agent.hpp"
#include <vector>
//need to add file in CMake.txt

class Law {
protected:
	float m_relaxation;
	std::vector<Vec2> getCoordinatesArray(const std::vector<Agent>&neighbors) const;
	Vec2 computeAgentsBarycenter(const std::vector<Agent>& neighbors) const;
	Law(const float &relaxation);


public:
	virtual Vec2 compute(Agent& currentAgent, const std::vector<Agent*>& neighbors) const = 0;
	//ILaw() : default;
	//~ILaw() : default;
};

