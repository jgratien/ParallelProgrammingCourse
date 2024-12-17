#include "../include/laws/Avoidance.hpp"

Avoidance::Avoidance(const float& coef_relaxation) : Law(coef_relaxation)
{
}

Avoidance::Avoidance() : Law(1.0f)
{
}  // Valeur par défaut de relaxation

Vec2 Avoidance::compute(Agent& currentAgent, const std::vector<Agent*>& predators) const
{
    Vec2 escapeDirection(0.0f, 0.0f);
    if (!predators.empty()) {
        // Calcul de la vitesse moyenne des voisins dans le champ de vision de l'agent courant
        for (const auto& predator : predators)
        {
            Vec2 dist = predator->getPosition() - currentAgent.getPosition();
            if (dist.x > 0){
                escapeDirection.x -= 0.1 * relaxation;
            } 
            if (dist.x < 0){
                escapeDirection.x += 0.1 * relaxation;
            }
            if (dist.y > 0){
                escapeDirection.y -= 0.1 * relaxation;
            } 
            if (dist.y < 0){
                escapeDirection.y += 0.1 * relaxation;
            }
        }
    }

    return escapeDirection * relaxation;  // Appliquer le facteur de relaxation
}