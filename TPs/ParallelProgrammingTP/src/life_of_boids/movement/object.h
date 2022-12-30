#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include "../config/config.h"


class Object {
protected:
    Real m_x;
    Real m_y;

public:
    Object();
    Object(Real const& x, Real const& y);

    Real get_x() const;
    Real get_y() const;
    Real distance(Object const& object) const;

    static vec2 scale(Object const& obj, Real ratio=RATIO);
    
    ~Object();

};


namespace calc {

    inline Real modulo(Real const& a, Real const& b) {
        Real mod(a);
        if (a < 0) {
            while (mod < 0)
                mod += b;
        }
        else {
            while (mod >= b)
                mod -= b;
        }
        return mod;
    }

    inline vec2 normVector(vec2 const& v) {
        float inv_norm = 1 / sqrt(v[0] * v[0] + v[1] * v[1]);
        return { v[0] * inv_norm, v[1] * inv_norm };
    }

    inline Real angleVector(vec2 v1, vec2 v2) {
        Real dot = v1[0] * v2[0] + v1[1] * v2[1];
        Real det = v1[0] * v2[1] - v1[1] * v2[0];
        Real angle = atan2(det, dot);
        return angle;
    }

    inline Real angle(Real const weight, Real const y, Real const x, Real const old_angle) {
        return (1 - weight) * atan2(y, x) + weight * old_angle;
    }
};
