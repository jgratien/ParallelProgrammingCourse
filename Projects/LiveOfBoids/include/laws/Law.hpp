#pragma once
#include <vector>

#include "../utils/Vec2.hpp"
#include "../agents/Agent.hpp"
#include "../utils/utils.hpp"

class Law
{
    // Relaxation coefficient
   protected:
    float relaxation;
    std::vector<Vec2> getPositions(const std::vector<Agent*>& neighbors) const;
    Vec2 computeBarycenter(const std::vector<Agent*>& neighbors) const;
    Law(const float& coef_relaxation);

   public:
    virtual Vec2 compute(Agent& currentAgent, const std::vector<Agent*>& neighbors) const = 0;

    virtual ~Law() = default;
};
