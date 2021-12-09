#include "Agent.hpp"
#include "../../lib/myMath/Vec2.hpp"
#include "../../lib/myMath/utils.hpp"

Agent::Agent(const Vec2& position, const Vec2& velocity, const int& bodySize, const int& viewAngle, const int& range, const float & maxSpeed, const Color& color)
{
	m_color = color;
	switch (m_color)
	{
	case Color::Red:
		m_GLColor = { 0.82745f, 0.184313f, 0.184313f };
		break;
	case Color::Green:
		m_GLColor = { 0.219607f, 0.55686f, 0.235294f };
		break;
	case Color::Blue:
		m_GLColor = { 0.09f, 0.99f, 0.99f };
		break;
	case Color::Yellow:
		m_GLColor = { 0.95f, 0.99f, 0.50f };
		break;
	case Color::Purple:
		m_GLColor = { 0.87f, 0.25f, 0.98f };
		break;
	case Color::Grey:
		m_GLColor = { 0.62f, 0.62f, 0.62f };
		break;
	case Color::Predator:
		m_GLColor = { 0.63f, 0.53f, 0.4980f };
		break;
	case Color::LightRed:
		m_GLColor = {0.8980f, 0.4509f, 0.4509f};
		break;
	default:
		m_GLColor = { 0.97f, 0.97f, 0.97f };
		break;
	}

	m_position = position;
	m_velocity = velocity;
	m_bodySize = bodySize;
	m_viewAngle = viewAngle;
	m_range = range;
	m_maxSpeed = maxSpeed;
	m_toDestroy = false;

};

Agent::Agent(const int &lVelocity, const int& uVelocity, const int& bodySize,
	const int& viewAngle) {
	Vec2 pos = randomVec2Generation(0, 1000);
	Vec2 vel = randomVec2Generation(lVelocity, uVelocity);
	m_position = pos;
	m_velocity = vel;
	m_nextPosition = pos + 1;
	m_nextVelocity = vel + 1;
	m_bodySize = bodySize;
	m_viewAngle = viewAngle;
	m_toDestroy = false;
	m_range = 50;
};

Agent::Agent(const Vec2& position, const Vec2& velocity, const int& bodySize, 
	const int& viewAngle, const int& range) {
	m_position = position;
	m_velocity = velocity;
	m_bodySize = bodySize;
	m_viewAngle = viewAngle;
	m_range = range;
	m_toDestroy = false;
};


Vec2 Agent::getPosition() const{
	return m_position; 
};

Vec2 Agent::getVelocity() const {
	return m_velocity;
};

Vec2 Agent::getNextPosition() const {
	return m_nextPosition;
};

Vec2 Agent::getNextVelocity() const {
	return m_nextVelocity;
};

int Agent::getRange() const {
	return m_range;
};

int Agent::getViewAngle() const {
	return m_viewAngle;
};

int Agent::getBodySize() const {
	return m_bodySize;
}

bool Agent::getDestruction() {
	return m_toDestroy;
}

void Agent::setDestruction() {
	m_toDestroy = true;
}

void Agent::move() {
	m_position = m_nextPosition;
	m_velocity = m_nextVelocity;
};

void Agent::prepareMove() {
	m_nextPosition = m_position + m_nextVelocity;
}

void Agent::setPosition(const Vec2& newPos) {
	m_position = newPos;
};

void Agent::setVelocity(const Vec2& newVel) {
	m_velocity = newVel;
}

void Agent::setSize(int size) {
	m_bodySize = size;
}

void Agent::setNextVelocity(const Vec2& newVel)
{
	m_nextVelocity = newVel;
}

void Agent::setNextPosition(const Vec2& newPos) {
	m_nextPosition = newPos;
}

std::array<float, 3> Agent::getGLColor() const {
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
	m_toDestroy = false;

	return *this;
};

