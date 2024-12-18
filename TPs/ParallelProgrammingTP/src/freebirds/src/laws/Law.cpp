#include "../include/laws/Law.hpp"

// Initialisation du facteur de relaxation dans le constructeur de la classe Law
Law::Law(const float& coef_relaxation) : relaxation{coef_relaxation}
{
}

// Retourne un vecteur de coordonnées (Vec2) basé sur les positions d'un vecteur d'agents voisins
std::vector<Vec2> Law::getPositions(const std::vector<Agent*>& neighbors) const
{
    // Crée un vecteur de points de même taille que le nombre de voisins
    std::vector<Vec2> positions(neighbors.size());

    // Remplit le vecteur avec les positions de chaque agent dans le voisinage
    for (size_t i = 0; i < neighbors.size(); ++i)
    {   
        if(not neighbors[i]->getDestruction()){
            float x = neighbors[i]->getPosition().x;
            float y = neighbors[i]->getPosition().y;
            positions[i] = Vec2{x, y};
        }
        
    }

    return positions;
}

// Calcule et retourne le barycentre d'un groupe d'agents
Vec2 Law::computeBarycenter(const std::vector<Agent*>& neighbors) const
{
    // Utilise getPositions() pour obtenir un vecteur de positions des voisins
    std::vector<Vec2> positions = getPositions(neighbors);

    // Retourne le barycentre des positions calculées
    return barycenter(positions);
}
