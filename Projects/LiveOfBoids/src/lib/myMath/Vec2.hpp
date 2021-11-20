#pragma once
#include <iostream>


struct Vec2 {
    float x;
    float y;

public:
    Vec2();
    Vec2(const float, const float);
    Vec2(const float);
    Vec2(const Vec2& other) noexcept;
    Vec2(Vec2&& other) noexcept;
    ~Vec2() {};

public:
    Vec2 operator+(const Vec2& other) const; // add vectors
    Vec2& operator+=(const Vec2& other); // add vectors

    Vec2 operator-(const Vec2& other) const; // substract vectors
    Vec2& operator-=(const Vec2& other); // substract vectors

    Vec2 operator*(const Vec2& other) const; // multiply vectors term by term
    Vec2 operator*(const float& other); // scale the vector

    Vec2 operator/(const Vec2& other) const; // divide vectors term by term
    Vec2 operator/(const float& other); // scale the vector

    Vec2& operator*=(const Vec2& other); // multiply vectors term by term
    Vec2& operator*=(float& other); // scale the vector

    Vec2& operator/=(const Vec2& other); // divide vectors term by term
    Vec2& operator/=(float& other); // scale the vector

    Vec2& operator=(const Vec2& other); // assign vector
    Vec2& operator=(Vec2&& other) noexcept;
    bool operator==(const Vec2& other) const; // compare current vector with vector

    float dot(const Vec2& other) const; // return the scalar product
    float norm() const; // return the norm of the vector
    Vec2 normalize(); // return the current vector normalized
    float angle() const; // return the angle between the current vector and `Vec2(1, 0)`
    float angle(const Vec2& other) const; // return the angle between the current vector and an other vector
    Vec2 rotate(const float angle); // rotate the vector anti-clockwise
    std::string string() const; // return a string useful when you need to display the vector
};

std::ostream& operator<<(std::ostream& os, const Vec2& obj);