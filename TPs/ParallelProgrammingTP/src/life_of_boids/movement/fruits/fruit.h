#pragma  once
#include "eco.h"

class Fruit : public Eco {

protected:
    bool m_alive;

public:
    Fruit();
    Fruit(Real const& x, Real const& y);
    Fruit(Real const& x, Real const& y, Real const& size, bool const& alive);

    bool& get_alive();

    ~Fruit();
};
