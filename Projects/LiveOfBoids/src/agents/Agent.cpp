#include "../include/agents/Agent.hpp"

Agent::Agent(const Agent& other)
    : m_position(other.m_position),
      m_velocity(other.m_velocity),
      m_nextPosition(other.m_nextPosition),
      m_nextVelocity(other.m_nextVelocity),
      m_bodySize(other.m_bodySize),
      m_viewAngle(other.m_viewAngle),
      m_minSpeed(other.m_minSpeed),
      m_maxSpeed(other.m_maxSpeed),
      m_range(other.m_range),
      m_toDestroy(other.m_toDestroy),
      m_GLColor(other.m_GLColor),
      m_turnFactor(other.m_turnFactor),
      m_type(other.m_type) {}

Agent::Agent(const int& lVelocity, const int& uVelocity, const float weight, const float height, const float ratio, const int& bodySize, const int& viewAngle,  const float turnFactor, AgentType type = AgentType::Neutral, Color color = Color::Default)
{
    Vec2 pos = randomGenerationOnScreen(weight, height, ratio);
    Vec2 vel = randomMovementGeneration(lVelocity, uVelocity);
    m_position = pos;
    m_velocity = vel;
    m_nextPosition = pos + 1;
    m_nextVelocity = vel + 1;
    m_bodySize = bodySize;
    m_viewAngle = viewAngle;
    m_minSpeed = 1;
    m_maxSpeed = 1;
    m_toDestroy = false;
    m_range = 50;
    m_type = type;
    m_turnFactor = turnFactor;
    m_GLColor = getColor(color);
};

Agent::Agent(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float& minSpeed, const float& maxSpeed,  const float turnFactor, AgentType type = AgentType::Neutral, Color color = Color::Default)
{
    m_position = position;
    m_velocity = velocity;
    m_bodySize = bodySize;
    m_viewAngle = viewAngle;
    m_minSpeed = minSpeed;
    m_maxSpeed = maxSpeed;
    m_range = range;
    m_type = type;
    m_toDestroy = false;
    m_turnFactor = turnFactor;
    m_GLColor = getColor(color);

};

void Agent::setType(AgentType type){
    m_type = type;
};

AgentType Agent::getType() const{
    return m_type;
};

Vec2 Agent::getPosition() const
{
    return m_position;
};

Vec2 Agent::getVelocity() const
{
    return m_velocity;
};

Vec2 Agent::getNextPosition() const
{
    return m_nextPosition;
};

Vec2 Agent::getNextVelocity() const
{
    return m_nextVelocity;
};

int Agent::getRange() const
{
    return m_range;
};

int Agent::getViewAngle() const
{
    return m_viewAngle;
};

int Agent::getBodySize() const
{
    return m_bodySize;
}

float Agent::getMinSpeed() const
{
    return m_minSpeed;
}

float Agent::getMaxSpeed() const
{
    return m_maxSpeed;
}

float Agent::getTurnFactor() const
{
    return m_turnFactor;
}

bool Agent::getDestruction()
{
    return m_toDestroy;
}

void Agent::setDestruction()
{
    m_toDestroy = true;
}

void Agent::setTurnFactor(float turnFactor)
{
    m_turnFactor = turnFactor;
}

void Agent::move()
{
    m_position = m_nextPosition;
    m_velocity = m_nextVelocity;
};

void Agent::prepareMove()
{
    m_nextPosition = m_position + m_nextVelocity;
}

void Agent::setPosition(const Vec2& newPos)
{
    m_position = newPos;
};

void Agent::setVelocity(const Vec2& newVel)
{
    m_velocity = newVel;
}

void Agent::setSize(int size)
{
    m_bodySize = size;
}

void Agent::setNextVelocity(const Vec2& newVel)
{
    m_nextVelocity = newVel;
}

void Agent::setNextPosition(const Vec2& newPos)
{
    m_nextPosition = newPos;
}

void Agent::setGLColor(Color color){
    m_GLColor = getColor(color);
};

vec3 Agent::getGLColor() const
{
    return m_GLColor;
};

Agent& Agent::operator=(const Agent& other)
{
    m_position = other.m_position;
    m_velocity = other.m_velocity;
    m_nextPosition = other.m_nextPosition;
    m_nextVelocity = other.m_nextVelocity;
    m_bodySize = other.m_bodySize;
    m_range = other.m_range;
    m_viewAngle = other.m_viewAngle;
    m_minSpeed = other.m_minSpeed;
    m_maxSpeed = other.m_maxSpeed;
    m_type = other.m_type;
    m_toDestroy = other.m_toDestroy;
    m_GLColor = other.m_GLColor;
    m_turnFactor = other.m_turnFactor;

    return *this;
};


