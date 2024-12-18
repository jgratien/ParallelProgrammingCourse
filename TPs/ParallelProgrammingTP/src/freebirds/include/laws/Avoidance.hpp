#pragma once
#include <vector>

#include "Law.hpp"

struct Avoidance : public Law
{
    Avoidance(const float& relaxation);
    Avoidance();
    Vec2 compute(Agent& currentAgent, const std::vector<Agent*>& predators) const override;
};