#include "../include/laws/Alignment.hpp"

Alignment::Alignment(const float& coef_relaxation) : Law(coef_relaxation)
{
}

Alignment::Alignment() : Law(1.0f)
{
}  // Valeur par défaut de relaxation

Vec2 Alignment::compute(Agent& currentAgent, const std::vector<Agent*>& neighbors) const
{
    Vec2 averageVelocity(0.0f, 0.0f);
    if (!neighbors.empty()) {
        // Calcul de la vitesse moyenne des voisins dans le champ de vision de l'agent courant
        for (const auto& neighbor : neighbors)
        {
            averageVelocity += neighbor->getVelocity();
        }
        averageVelocity = averageVelocity.normalized();
        averageVelocity = (averageVelocity + currentAgent.getVelocity()).normalized();
    
    }

    return averageVelocity * relaxation;  // Appliquer le facteur de relaxation
}
