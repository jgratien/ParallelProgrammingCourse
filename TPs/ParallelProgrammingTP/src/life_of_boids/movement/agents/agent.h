#pragma once
#include "../object.h"
#include "../obstacles/obstacle.h"

class Agent;
//using agents_t = std::unordered_map<size_t, Agent>;

class Predator;
using predators_t = std::unordered_map<size_t, Predator>;

class Bird;
using birds_t = std::unordered_map<size_t, Bird>;

enum class state {
    near_obstacle,
    near_predator,
    near_prey, // only for the predator
    near_predatorANDseparation,
    near_fruit,
    near_fruitANDseparation,
    separation,
    alignment,
    cohesion,
    constant
};

class Agent : public Object {
protected:
    Real m_angle;
    state m_state;
    size_t m_index;

public:
    Agent();
    Agent(Real const& x, Real const& y);
    Agent(Real const& x, Real const& y, Real const& angle);
    Agent(Real const& x, Real const& y, Real const& angle, size_t& index);

    Real get_angle() const;
    state get_state() const;
    size_t& get_index();

    Real angle (Agent const& a) const;
    bool insideFieldView(Agent const& a) const;

    bool operator==(Agent & a) const;
    bool overlap(Agent& a) const;
    bool overlap(birds_t& birds, predators_t& predators) const;

    std::vector<Real> closestObstacle(std::vector<Obstacle> const& obstacles);

    void windowUpdate(Real speed);
    void constantUpdate(Real speed);

    void obstacleLaw(std::vector<Real> const&  obstacle);
    void separationLaw(std::vector<Real> const&  predator);

    ~Agent();
};
