#pragma once
#include "AgentDisplayer.hpp"
#include <array>
#include "../../lib/myMath/Vec2.hpp"

class DotDisplayer : public AgentDisplayer<1> {
public:
	std::array<Vec2, 1> drawAgent(Agent* a) const override;
};