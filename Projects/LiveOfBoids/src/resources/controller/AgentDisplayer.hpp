#pragma once
#include "../model/Agent.hpp"
#include <array>
#include "../../lib/myMath/Vec2.hpp"

template <int m_nbPoints>
class AgentDisplayer {

public:
	virtual std::array<Vec2, m_nbPoints> drawAgent(Agent* a) const = 0;
};