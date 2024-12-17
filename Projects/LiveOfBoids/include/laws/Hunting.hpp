#pragma once
#include <vector>

#include "Law.hpp"

struct Hunting : public Law
{
    Hunting(const float& coef_relaxation);
    Hunting();
    Vec2 compute(Agent& currentAgent, const std::vector<Agent*>& preys) const override;
    void kill(Agent& currentAgent, const std::vector<Agent*> preys);
};