#ifndef LAW_HPP
#define LAW_HPP
#include "../vector2D/vector2D.hpp"
#include <vector>

struct AgentState 
{
    Vector2D position;
    Vector2D velocity;
    float max_speed;
    float max_force;
    float agent_size;
};

class FlockingLaw {
public:
    static Vector2D cohesion(const AgentState& current, const std::vector<AgentState>& neighbors);
    static Vector2D separation(const AgentState& current, const std::vector<AgentState>& neighbors);
    static Vector2D alignement(const AgentState& current, const std::vector<AgentState>& neighbors);

private:
    static Vector2D limitForce(const Vector2D& force, float max_force);
};
#endif // LAW_HPP