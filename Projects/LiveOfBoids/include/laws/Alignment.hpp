#pragma once
#include <vector>

#include "Law.hpp"

struct Alignment : public Law
{
    Alignment(const float& relaxation);
    Alignment();
    Vec2 compute(Agent& currentAgent, const std::vector<Agent*>& neighbors) const override;
};