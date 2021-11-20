#include "AgentDisplayer.hpp"
#include "DotDisplayer.hpp"
#include <array>
#include "../../lib/myMath/Vec2.hpp"

std::array<Vec2, 1> DotDisplayer::drawAgent(Agent* a) const {
	return { (*a).getPosition() };
};