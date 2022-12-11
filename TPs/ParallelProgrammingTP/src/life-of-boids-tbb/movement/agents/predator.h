#pragma once
#include "agent.h"
#include "predator.h"
#include "bird.h"

class Predator : public Agent {

public:
    Predator();
    Predator(Real const& x, Real const& y);
    Predator(Real const& x, Real const& y, Real const& angle);
    Predator(Real const& x, Real const& y, Real const& angle, size_t& index);

    std::vector<Real> neighbours(birds_t& birds, predators_t& predators);
    
    void preyLaw(std::vector<Real> const& birds);

    void update(std::vector<Obstacle>const& obstacles, predators_t& predators, birds_t& birds);
    
    static predators_t init(std::vector<Obstacle> const& obstacles);
    
    ~Predator();
};
