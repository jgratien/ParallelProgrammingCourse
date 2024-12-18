#pragma once
#include <vector>

#include "Law.hpp"

struct Cohesion : public Law
{
    Cohesion(const float& coef_relaxation);
    Cohesion();
    Vec2 compute(Agent& currentAgent, const std::vector<Agent*>& neighbors) const override;
};