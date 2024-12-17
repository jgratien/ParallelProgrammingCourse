#pragma once

//#include "Color.hpp"
#include <array>
#include <vector>

#include "../utils/Vec2.hpp"
#include "../utils/utils.hpp"
#include "../utils/Color.hpp"
#include "../utils/MathUtils.hpp"


static int id = 0;
enum class AgentType { Neutral, Predator, Prey };
class Agent
{
   protected:
    Vec2 m_position;
    Vec2 m_velocity;
    Vec2 m_nextPosition;
    Vec2 m_nextVelocity;
    int m_bodySize;
    int m_viewAngle;
    int m_range;
    vec3 m_GLColor;
    float m_minSpeed;
    float m_maxSpeed;
    bool m_toDestroy;
    float m_turnFactor;
    AgentType m_type;

   protected:
    Agent(const Agent& other); 
    Agent(const int& lVelocity, const int& uVelocity, const float weight, const float height, const float ratio, const int& bodySize, const int& viewAngle,  const float turnFactor, AgentType type, Color color);
    Agent(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float& minSpeed, const float& maxSpeed,  const float turnFactor, AgentType type, Color color);

   public:
    virtual ~Agent() = default;
    int _id = ++id;
    virtual void computeLaws(const std::vector<Agent*>& neighbors,const std::vector<Agent*>& predators,const std::vector<Agent*> prey) = 0;
    void prepareMove();
    void move();

    void setType(AgentType type);
    AgentType getType() const;
    void setPosition(const Vec2& newPos);
    void setNextPosition(const Vec2& newPos);
    void setNextVelocity(const Vec2& newVel);
    void setVelocity(const Vec2& newVel);
    void setSize(int size);
    void setGLColor(Color color);
    Vec2 getPosition() const;
    Vec2 getVelocity() const;
    Vec2 getNextVelocity() const;
    Vec2 getNextPosition() const;
    bool getDestruction();
    void setDestruction();
    float getMinSpeed() const;
    float getMaxSpeed() const;
    int getRange() const;
    int getBodySize() const;
    int getViewAngle() const;
    vec3 getGLColor() const;
    void setTurnFactor(float turnFactor);
    float getTurnFactor() const;


    Agent& operator=(const Agent& other);
};
