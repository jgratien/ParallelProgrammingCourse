#pragma once
#include "../object.h"

class Obstacle : public Object {
protected :
    Real m_height;
    Real m_half_h;
    Real m_width;
    Real m_half_w;

public:
    Obstacle();
    Obstacle(Real const& x, Real const& y, Real const& height, Real const& width);

    Real get_height() const;
    Real get_width() const;

    bool nearBorder() const;
    bool overlap(std::vector<Obstacle>  const& obstacles) const;
    static std::vector<Obstacle> init();

    ~Obstacle();
};

