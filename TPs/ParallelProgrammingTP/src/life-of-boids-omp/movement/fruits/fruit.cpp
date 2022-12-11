#include "fruit.h"

Fruit::Fruit() : Eco(), m_alive(true) {}
Fruit::Fruit(Real const& x, Real const& y) : Eco(x,y), m_alive(true) {}
Fruit::Fruit(Real const& x, Real const& y, Real const& size, bool const& alive) : Eco(x,y,size), m_alive(alive) {}

bool& Fruit::get_alive() {
    return m_alive;
}

Fruit::~Fruit() = default;