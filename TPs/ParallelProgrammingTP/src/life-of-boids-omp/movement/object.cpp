#include "object.h"

Object::Object() : m_x(0), m_y(0) {}
Object::Object(Real const& x, Real const& y) : m_x(x), m_y(y) {}

Real Object::get_x() const{
    return m_x;
}

Real Object::get_y() const{
    return m_y;
}

Real Object::distance(Object const& object) const {
    Real diff_x = (m_x - object.get_x());
    Real diff_y = (m_y - object.get_y());

    return std::sqrt((diff_x * diff_x) + (diff_y * diff_y));
}

vec2 Object::scale(Object const& obj, Real ratio) {
    return {
            2 * ratio * (((Real)(obj.get_x())) / (Real)(WIDTH)) - ratio,
            2 * (((Real)(obj.get_y())) / (Real)(HEIGHT)) - 1
    };
}

Object::~Object() = default;
