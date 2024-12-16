#include "agent.hpp"

using namespace std;

Agent::Agent(Vector2D pos, Vector2D vel)
{
  this->position = pos;
  this->velocity = vel;
  this->updateVertices();
};

// Agent::~Agent() {};

std::ostream& operator<<(std::ostream& out, const Agent& a)
{
  out << "position = " << a.position << " velocity = " << a.velocity;
  return out;
};

void Agent::updateVertices()
{
  float size = getAgentSize();
  float angle = atan2(velocity.getY(),velocity.getX());
  float baseAngle = 2.5f * M_PI / 3.0f;

  vertices[0] = position + Vector2D(size * cos(angle), size * sin(angle));
  vertices[1] = position + Vector2D(size * cos(angle + baseAngle), size * sin(angle + baseAngle));
  vertices[2] = position + Vector2D(size * cos(angle - baseAngle), size * sin(angle - baseAngle));
}


std::vector<Agent> Agent::get_neighbors(const std::vector<Agent>& agents, float radius) const
{
  vector<Agent> neighbors;
  for(auto& other: agents)
  {
    if((&other != this)  && isInViewRange(other, radius) )
      neighbors.push_back(other);
  }
  return neighbors;
};

void Agent::cross_boundary(int *width, int *height)
{
  int offset = 10;
  
  if(this->position.getX() > (*width + offset))
    this->position.setX(0 - offset);
  else if(this->position.getX() < -offset)
    this->position.setX(*width + offset);
  if(this->position.getY() > *height + offset)
    this->position.setY(0 - offset);
  else if(this->position.getY() < -offset)
    this->position.setY(*height + offset);  
};

Agent::UpdateState Agent::calculateNewState(const std::vector<Agent>& agents, int* width, int* height, Vector2D& randomVelocity) const 
{
    Vector2D acceleration;

    float separation_range = 40; 
    float alignment_range = 75;
    float cohesion_range = 100;

    acceleration = acceleration + FlockingLaw::separation(this->getState(), getNeighborStates(this->get_neighbors(agents,separation_range)));
    acceleration = acceleration + FlockingLaw::alignement(this->getState(), getNeighborStates(this->get_neighbors(agents,alignment_range)));
    acceleration = acceleration + FlockingLaw::separation(this->getState(), getNeighborStates(this->get_neighbors(agents,cohesion_range)))*0.8;

    //std::cout<<"Acceleration :"<< acceleration <<std::endl;
    Vector2D newVelocity = this->velocity + acceleration;
    
    // Ensure minimum speed
    if(newVelocity.euclidean_norm() < this->max_speed * 0.5) { // enforce minimum speed of 0.5 * max_speed
        newVelocity = newVelocity.normalize() * this->max_speed * 0.5;
    }
    
    // Cap maximum speed
    if(newVelocity.euclidean_norm() > this->max_speed) {
        newVelocity.scale_to_length(this->max_speed);
    }

    Vector2D newPosition = this->position + newVelocity;
    return {newPosition, newVelocity};
}

void Agent::applyState(const UpdateState& state)
{
    this->position = state.newPosition;
    this->velocity = state.newVelocity;
    this->updateVertices();
}

bool Agent::isInViewRange(const Agent& other, const float& radius) const {
    float distance = this->position.euclidean_dist(other.position);
    if (distance > radius)
        return false;
        
    Vector2D to_other = other.position - this->position;
    float angle = atan2(to_other.getY(), to_other.getX()) - 
                  atan2(velocity.getY(), velocity.getX());

    while (angle > M_PI) angle -= 2 * M_PI;
    while (angle < -M_PI) angle += 2 * M_PI;
    // std::cout<<"Angle : "<< angle <<std::endl;

    // std::cout<< "Angle limite : "<< view_range_angle * M_PI / 180.0f << std::endl;
    
    return std::abs(angle) <= (view_range_angle * M_PI / 180.0f);
    }





