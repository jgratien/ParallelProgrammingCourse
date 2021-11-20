#pragma once
#include "Law.hpp"
#include "../myMath/Vec2.hpp"
#include <vector>
#include "../../resources/model/Agent.hpp"

class CohesionLaw : public Law {
protected:
	//float m_range = 10;
	//float m_relaxation = 0.9;
public:
	CohesionLaw(const float& relaxation);
	CohesionLaw();
	Vec2 compute(Agent& currentAgent, const std::vector<Agent*>& neighbors) const override;
};