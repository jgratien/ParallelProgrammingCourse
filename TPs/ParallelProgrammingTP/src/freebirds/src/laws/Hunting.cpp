#include "../include/laws/Hunting.hpp"

Hunting::Hunting(const float& coef_relaxation) : Law(coef_relaxation)
{
}

Hunting::Hunting() : Law(1.0f)
{
}  // Valeur par défaut de relaxation

Vec2 Hunting::compute(Agent& currentAgent, const std::vector<Agent*>& preys) const
{
    Vec2 centerOfMass(0.0f, 0.0f);
    if (!preys.empty()){
        centerOfMass = (computeBarycenter(preys) - currentAgent.getPosition()).normalized();
    }
    //std::cout<< relaxation;
    return centerOfMass * relaxation;
}

void Hunting::kill(Agent& currentAgent, const std::vector<Agent*> preys)
{
    if (!preys.empty()){
        for (auto prey: preys){
            float dist = distance(currentAgent.getPosition(), prey->getPosition());
            if(dist < relaxation/5){
                prey -> setDestruction();
            }
        }
    }
}
