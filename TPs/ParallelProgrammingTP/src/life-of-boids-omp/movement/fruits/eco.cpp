#include "eco.h"

Eco::Eco() : Object(), m_size(0), m_obstacle(false) {}
Eco::Eco(Real const& x, Real const& y) : Object(x, y), m_size(0), m_obstacle(false) {}
Eco::Eco(Real const& x, Real const& y, Real const& size) : Object(x, y), m_size(size), m_obstacle(false) {}

Real Eco::get_size() const {
    return m_size;
}

bool Eco::get_obstacle() const {
    return m_obstacle;
}

bool Eco::nearBorder() const {

    return (m_x < (Real)CLOSE) || (((Real)WIDTH - (Real)CLOSE < m_x) && (m_x < (Real)WIDTH)) ||
           (m_y < CLOSE) || (((Real)HEIGHT - (Real)CLOSE) < m_y && (m_y < (Real)HEIGHT));
}

void Eco::closestObstacle(std::vector<Obstacle> const& obstacles) {

    auto check = [this](Obstacle const& obstacle) {
        return ( this->distance(obstacle) < 1.5*(std::max(obstacle.get_height() * HALF, obstacle.get_width() * HALF) + m_size) );
    };
    
    m_obstacle = std::any_of(obstacles.begin(), obstacles.end(), check);
}

Eco::~Eco() = default;