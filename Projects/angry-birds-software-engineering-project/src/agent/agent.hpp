#ifndef DEF_AGENT_HPP
#define DEF_AGENT_HPP

#include "../vector2D/vector2D.hpp"
#include <array>
#include <cstddef>
#include <vector>
#include "../law/laws.hpp"

using mat3x3 = std::array<std::array<float, 3>, 3>;

class Agent 
{
    public :
        //constructor
        Agent(Vector2D pos, Vector2D vel);
        // ~Agent();

        struct UpdateState {
            Vector2D newPosition;
            Vector2D newVelocity;
        };

        // overloading stream operator for agent
        friend std::ostream& operator<<(std::ostream& out, const Agent& a);

        // getters
        Vector2D getPosition() const { return position; };
        float getPositionX() const { return position.getX(); };
        float getPositionY() const { return position.getY(); };
        Vector2D getVelocity() const { return velocity; };
        float getVelocityX() const { return velocity.getX(); };
        float getVelocityY() const { return velocity.getY(); };
        float getMaxSpeed() const { return max_speed; }
        float getAgentSize() const { return agent_size; }

        // setters
        void setPosition(Vector2D const& position) { this->position = position; };
        void setPositionX(const float x) { this->position.setX(x); };
        void setPositionY(const float y) { this->position.setY(y); };
        void setVelocity(Vector2D const& velocity) { this->velocity = velocity; };
        void setVelocityX(const float x) { this->velocity.setX(x); };
        void setVelocityY(const float y) { this->velocity.setY(y); };

        // get offsetof for OPENGL
        static size_t getPositionOffsetof() { return offsetof(Agent, position); };
        static size_t getVelocityOffsetof() { return offsetof(Agent, velocity); };

        std::vector<Agent> get_neighbors(const std::vector<Agent>& agents, float radius) const;
        //void update(const std::vector<Agent>& agents, int *width, int *height);

        UpdateState calculateNewState(const std::vector<Agent>& agents, int* width, int* height, Vector2D& randomVelocity) const;
        void applyState(const UpdateState& state);

        void cross_boundary(int *width, int *height);
        void updateVertices();
        const std::array<Vector2D, 3>& getVertices() const {
            return vertices;
        }

    AgentState getState() const {
        return AgentState{position, velocity, max_speed, max_force,agent_size};
    }

    
    std::vector<AgentState> getNeighborStates(const std::vector<Agent>& neighbors) const {
        std::vector<AgentState> neighbor_states;
        neighbor_states.reserve(neighbors.size());
        for (const auto& neighbor : neighbors) {
            neighbor_states.push_back(neighbor.getState());
        }
        return neighbor_states;
    }

bool isInViewRange(const Agent& other, const float& radius) const;
        

    private :
        Vector2D position;
        Vector2D velocity;

        float max_force = 0.4f;
        float max_speed = 4.5f;
        std::array<Vector2D, 3> vertices;

        float view_range_dist = 120.0f;    
        float view_range_angle = 270.0f;
        float agent_size = 6.0f;
};


#endif // DEF_AGENT_HPP