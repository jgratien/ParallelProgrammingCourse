#pragma once
#include <vector>

#include "Law.hpp"

struct Separation : public Law
{
    Separation(const float& coef_relaxation);
    Separation();
    Vec2 compute(Agent& currentAgent, const std::vector<Agent*>& neighbors) const override;
};