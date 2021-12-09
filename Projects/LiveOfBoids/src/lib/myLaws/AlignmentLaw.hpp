#pragma once
#include "Law.hpp"
#include "../myMath/Vec2.hpp"
#include <vector>
#include "../../resources/model/Agent.hpp"

class AlignmentLaw : public Law {
public:
	AlignmentLaw(const float& relaxation);
	AlignmentLaw();
	Vec2 compute(Agent& currentAgent, const std::vector<Agent*>& neighbors) const override;
};
