#include "vector2D.hpp"

Vector2D::Vector2D(float x, float y)
{
    this->x = x;
    this->y = y;
};

Vector2D Vector2D::operator+(const Vector2D& other) const
{
    return Vector2D(this->x + other.x, this->y + other.y);
};

Vector2D Vector2D::operator-(const Vector2D& other) const
{
    return Vector2D(this->x - other.x, this->y - other.y);
};

float Vector2D::operator*(const Vector2D& other)
{
    return this->x * other.x + this->y * other.y;
};

Vector2D Vector2D::operator*(const float scalar) const
{
    return Vector2D(this->x * scalar, this->y * scalar);
};

Vector2D Vector2D::operator/(const float scalar) const
{
    return Vector2D(this->x / scalar, this->y / scalar);
};

Vector2D& Vector2D::operator=(const Vector2D& other)
{
    this->x = other.x;
    this->y = other.y;
    return *this;
};

bool Vector2D::operator==(const Vector2D& other) const {
    return this->x==other.x && this->y==y;
};

std::ostream& operator<<(std::ostream& out, const Vector2D& v)
{
    out << "(" << v.x << ","  << v.y << ")";
    return out;
};

float Vector2D::euclidean_norm() const
{
    return std::sqrt(x*x+ y*y);
};

float Vector2D::euclidean_dist (const Vector2D& other) const
{
    return std::sqrt(std::pow((this->x - other.x),2) + std::pow((this->y - other.y),2));
};

Vector2D Vector2D::normalize () const
{
    if (this->euclidean_norm() > 0)
        return Vector2D(this->x /this->euclidean_norm(), this->y /this->euclidean_norm());
    return Vector2D(this->x, this->y);
};

Vector2D Vector2D::scale_to_length(const float scalar) const
{
    if (this->euclidean_norm() > 0) {
        float scale_factor = scalar / this->euclidean_norm();
        return Vector2D(this->x * scale_factor, this->y * scale_factor);
    }
    return Vector2D(this->x, this->y);
};