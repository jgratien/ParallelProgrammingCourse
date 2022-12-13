#pragma once

#include "agent.h"
#include "predator.h"
#include "../obstacles/fruit.h"

class Bird : public Agent {

protected:
    bool m_alive;
public:

    Bird();
    Bird(Real const& x, Real const& y);
    Bird(Real const& x, Real const& y, Real const& angle);
    Bird(Real const& x, Real const& y, Real const& angle, size_t& index);

    bool get_alive() const;

    std::vector<Real> neighbours(birds_t& birds);
    std::vector<Real> closestPredator(predators_t& predators);
    std::vector<Real> closestFruit(std::vector<Fruit>& fruits);

    void cohesionLaw(std::vector<Real> const& group);
    void alignmentLaw(std::vector<Real> const& group);
    void biSeparationLaw(std::vector<Real> const& bird, std::vector<Real> const&  predator);
    void fruitLaw(std::vector<Real> const& f);
    void biFruitLaw(std::vector<Real> const& f, std::vector<Real> const& bird);

    bool update(std::vector<Obstacle>const& obstacles, predators_t& predators, birds_t& birds, std::vector<Fruit>& fruits);
    
    static birds_t init(std::vector<Obstacle> const& obstacles, predators_t& predators, int& num_birds);
    ~Bird();
};