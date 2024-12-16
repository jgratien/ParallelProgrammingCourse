#include "laws.hpp"
#include <iostream>

Vector2D FlockingLaw::cohesion(const AgentState& current, const std::vector<AgentState>& neighbors)
{
    if (neighbors.empty()){
        return Vector2D(0,0);
    };

    Vector2D center_of_mass(0,0);
    for (const auto& neighbor : neighbors){
        center_of_mass = center_of_mass + neighbor.position;
    }

    center_of_mass = center_of_mass / static_cast<float>(neighbors.size());
    Vector2D desired_velocity = (center_of_mass - current.position).normalize() * current.max_speed;
    Vector2D force = desired_velocity - current.velocity;

    return limitForce(force, current.max_force);
}

Vector2D FlockingLaw::separation(const AgentState& current, const std::vector<AgentState>& neighbors)
{
    if (neighbors.empty()){
        return Vector2D(0,0);
    };

    Vector2D repulsion_force(0,0);
    for (const auto& neighbor : neighbors){
        float distance = current.position.euclidean_dist(neighbor.position);
        Vector2D repulsion = (current.position - neighbor.position)/(distance*distance);

        
        // if (distance < 2*current.agent_size){ 
        //     repulsion = repulsion * 2.0; // Double the repulsion strength for overlap
        // }         
        repulsion_force = repulsion_force + repulsion;
        }



    if(repulsion_force.euclidean_norm() > 0){
        repulsion_force = repulsion_force.normalize() * current.max_speed;
        Vector2D force = repulsion_force - current.velocity;
        return limitForce(force, current.max_force);
    }

    return Vector2D(0,0);
}

Vector2D FlockingLaw::alignement(const AgentState& current, const std::vector<AgentState>& neighbors)
{
    Vector2D average_velocity(0,0);
    float total_weight = 0.0f;

    for( const auto& neighbor : neighbors){
        //average_velocity = average_velocity + neighbors.velocity;
        float distance = current.position.euclidean_dist(neighbor.position);
        float weight = 1.0f / (1.0f + distance);
    
        average_velocity = average_velocity + neighbor.velocity * weight;
        total_weight += weight;
    }

    if (total_weight > 0) {
    average_velocity = average_velocity / total_weight;
    }
    //average_velocity = average_velocity/static_cast<float>(neighbors.size());
    average_velocity = average_velocity.normalize() * current.max_speed;
    Vector2D force = average_velocity - current.velocity;

    return limitForce(force, current.max_force);

}

Vector2D FlockingLaw::limitForce(const Vector2D& force, float max_force) {
    if (force.euclidean_norm() > max_force) {
        return force.scale_to_length(max_force);
    }
    return force;
}