#define _USE_MATH_DEFINES
#include "lib/myMath/Vec2.hpp"
#include "lib/myMath/utils.hpp"
#include <iostream>
#include <vector>
#include <cmath>

int main() {
    Vec2 a = Vec2(4.5f, 6.2f);
    Vec2 b = Vec2(2.f, 100.2f);
    std::cout << a << std::endl; // Display the vector
    std::cout << "x: " << a.x << std::endl; // return the x value
    std::cout << "y: " << a.y << std::endl; // return the y value
    std::cout << "norm: " << a.norm() << std::endl; // return the norm of the vector
    std::cout << a.normalize() << std::endl; // display normalized `a` vector
    std::cout << "norm of normalized vector: " << a.normalize().norm() << std::endl; // return `1` because `a.normalized()` has a norm equal to 1
    std::cout << "angle: " << a.angle() << std::endl; //return the angle between `a` and `Vec2(1, 0)`
    std::cout << "angle: " << a.angle(b) << std::endl; //return the angle between `a` and `b`
    std::cout << "dot: " << a.dot(b) << std::endl; //return the scalar product
    Vec2 c;
    c = a; // assign to `c` the values of `a`
    c.x = 10;
    std::cout << "c.x: " << c.x << std::endl;
    std::cout << "a.y: " << c.y << std::endl;
    std::cout << "c = a: ";
    std::cout << c << std::endl;
    Vec2 d = a + b;
    std::cout << "Addition: ";
    std::cout << d << std::endl;
    std::cout << "Substraction: ";
    d = a - b;
    std::cout << d << std::endl;
    std::cout << "Multiplication: ";
    d = a * b; // multiply term by term `a` and `b`
    std::cout << d << std::endl;
    std::cout << "Scale: ";
    d = a * 5.; // multilply all terms of `a` by `5.`
    std::cout << d << std::endl;

    std::vector<Vec2> vecs;
    vecs.push_back(Vec2(0., 0.));
    vecs.push_back(Vec2(0., 1.));
    vecs.push_back(Vec2(1., 0.));
    vecs.push_back(Vec2(1., 1.));
    std::cout << "Distance: " << distance(a, b) << std::endl;
    Vec2 x = barycenter(vecs);
    std::cout << x << std::endl;
    std::cout << (a == Vec2(4.f, 6.2f)) << std::endl;
    Vec2 e = Vec2(1., 0.);
    std::cout << e.rotate((float)M_PI/2.f) << std::endl;
    std::cout << e.rotate((float)-M_PI/2.f) << std::endl;
    std::cout << e.rotate((float)M_PI) << std::endl;
    std::cout << "Rotation: " << Vec2(1.f, 0.f).rotate((float)M_PI / 3.f) << std::endl; //ERORR ON LINUX

    a = Vec2(2.f, 5.f);
    b = Vec2(2.f, 5.f);
    Vec2 X = Vec2(1.f, 0.f);
    std::cout << "AntiClockwise ?" << "\n";
    std::cout << isAntiClockwise(a, X) << "\n";
    std::cout << isAntiClockwise(X, a) << "\n";
    std::cout << isAntiClockwise(b, X) << "\n";
    std::cout << isAntiClockwise(X, a) << "\n";
    
    return 0;
}
