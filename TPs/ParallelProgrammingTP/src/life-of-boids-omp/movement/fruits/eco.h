#pragma once
#include "../object.h"
#include "../obstacles/obstacle.h"

class Eco : public Object {
protected:
    Real m_size;
	bool m_obstacle;
public:
	
	Eco();
	Eco(Real const& x, Real const& y);
    Eco(Real const& x, Real const& y, Real const& size);

    Real get_size() const;
	bool get_obstacle() const;

	bool nearBorder() const;
	void closestObstacle(std::vector<Obstacle> const& obstacles);
	
	~Eco();
};