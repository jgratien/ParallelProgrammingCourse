#include "../include/laws/Separation.hpp"

Separation::Separation(const float& coef_relaxation) : Law(coef_relaxation)
{
}

Separation::Separation() : Law(1.0f)
{
}  // Valeur par défaut de relaxation

Vec2 Separation::compute(Agent& currentAgent, const std::vector<Agent*>& neighbors) const
{
    Vec2 separationForce(0.0f, 0.0f);

    if (!neighbors.empty()) {
        // Calcul de la force de répulsion avec chaque voisin à proximité
        for (const auto& neighbor : neighbors)
        {
            Vec2 repulsion = (currentAgent.getPosition() - neighbor->getPosition()).normalized();
            separationForce += repulsion.normalized();
        }
    }

    return separationForce * relaxation;  // Appliquer le facteur de relaxation
}
