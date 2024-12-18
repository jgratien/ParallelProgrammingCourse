#include "../include/laws/Cohesion.hpp"

Cohesion::Cohesion(const float& coef_relaxation) : Law(coef_relaxation)
{
}

Cohesion::Cohesion() : Law(1.0f)
{
}  // Valeur par défaut de relaxation

Vec2 Cohesion::compute(Agent& currentAgent, const std::vector<Agent*>& neighbors) const
{
    Vec2 centerOfMass(0.0f, 0.0f);
    if (!neighbors.empty()){
        centerOfMass = (computeBarycenter(neighbors) - currentAgent.getPosition()).normalized();
    }
    //std::cout<< relaxation;
    return centerOfMass * relaxation;

}
