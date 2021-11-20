#include <cmath>
#include <iostream>
#include <string>
#include "Vec2.hpp"
#include <algorithm>


Vec2::Vec2() : x{ 0 }, y{ 0 } {}

Vec2::Vec2(const float x, const float y) : x{ x }, y{ y } {}

Vec2::Vec2(const float a) : x{ a }, y{ a } {}

// Constructeur par copie
Vec2::Vec2(const Vec2& other) noexcept : x{ other.x }, y{ other.y } {}

Vec2& Vec2::operator=(const Vec2& other) // Opeateur par copie
{
    this->x = other.x;
    this->y = other.y;
    return *this;
}

// Constructeur par deplacement
Vec2::Vec2(Vec2&& other) noexcept : x{ std::move(other.x) }, y{ std::move(other.y) } {}

Vec2& Vec2::operator=(Vec2&& other) noexcept // Operateur par deplacement
{
    this->x = std::move(other.x);
    this->y = std::move(other.y);
    return *this;
}


Vec2 Vec2::operator+(const Vec2& other) const
{
    return Vec2(this->x + other.x, this->y + other.y);
}

Vec2 Vec2::operator-(const Vec2& other) const
{
    return Vec2(this->x - other.x, this->y - other.y);
}

Vec2 Vec2::operator*(const Vec2& other) const
{
    return Vec2(this->x * other.x, this->y * other.y);
}

Vec2 Vec2::operator*(const float& other)
{
    return Vec2(this->x * other, this->y * other);
}

Vec2& Vec2::operator+=(const Vec2& other)
{
    this->x += other.x;
    this->y += other.y;
    return *this;
}

Vec2& Vec2::operator-=(const Vec2& other)
{
    this->x -= other.x;
    this->y -= other.y;
    return *this;
}

Vec2& Vec2::operator*=(const Vec2& other)
{
    this->x *= other.x;
    this->y *= other.y;
    return *this;
}

Vec2& Vec2::operator*=(float& other)
{
    this->x *= other;
    this->y *= other;
    return *this;
}

Vec2 Vec2::operator/(const Vec2& other) const
{
    return Vec2(this->x / other.x, this->y / other.y);
}

Vec2 Vec2::operator/(const float& other)
{
    return Vec2(this->x / other, this->y / other);
}

Vec2& Vec2::operator/=(const Vec2& other)
{
    this->x /= other.x;
    this->y /= other.y;
    return *this;
}

Vec2& Vec2::operator/=(float& other)
{
    this->x /= other;
    this->y /= other;
    return *this;
}

bool Vec2::operator==(const Vec2& other) const
{
    if (this->x == other.x && this->y == other.y) {
        return true;
    }
    else {
        return false;
    }
}

std::ostream& operator<<(std::ostream& os, const Vec2& obj)
{
    return os << obj.string();
}

float Vec2::dot(const Vec2& other) const {
    return this->x * other.x + this->y * other.y;
}

float Vec2::norm() const {
    return std::sqrt(this->x * this->x + this->y * this->y);
}

Vec2 Vec2::normalize() {
    float norm = this->norm();
    Vec2 normalized_vec = Vec2(0, 0);
    if (norm != 0) {
        normalized_vec = Vec2(this->x / norm, this->y / norm);
    }
    return normalized_vec;
}

float Vec2::angle() const {
    float angle = (float)std::acos(std::min(1., std::max(-1., (double)this->x / this->norm())));
    return (this->y > 0) ? angle : -angle;
}

float Vec2::angle(const Vec2& other) const {
    float norm = this->norm() * other.norm();
    if (norm == 0.f) {
        return 0.f;
    }
    else {
        return (float)std::acos(std::min(1., std::max(-1., (double)this->dot(other) / norm)));
    }
}

Vec2 Vec2::rotate(const float angle) {
    return Vec2(std::cos(angle) * this->x - std::sin(angle) * this->y, std::sin(angle) * this->x + std::cos(angle) * this->y);
}

std::string Vec2::string() const {
    return "Vec2(" + std::to_string(this->x) + ", " + std::to_string(this->y) + ")";
}
