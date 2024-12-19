/*
 * Vector2D.h
 *
 *  Created on: Dec 7, 2024
 *      Author: ossaradj
 */

#ifndef SRC_LIVEOFBOIDS_VECTOR_2D_H_
#define SRC_LIVEOFBOIDS_VECTOR_2D_H_

#include <iostream>
#include <cmath>

class Vector2D 
{
    public :
        //constructor for vector
        Vector2D(float x = 0.0f, float y = 0.0f) : m_x(x), m_y(y) {};

        // overloading operator for vector
        Vector2D operator + (const Vector2D& other) const
        {
            return Vector2D(this->m_x + other.m_x, this->m_y + other.m_y);
        };

        Vector2D operator - (const Vector2D& other) const
        {
            return Vector2D(this->m_x - other.m_x, this->m_y - other.m_y);
        };

        float operator * (const Vector2D& other) const
        {
            return this->m_x * other.m_x + this->m_y * other.m_y;
        };

        Vector2D operator*(const float scalar) const
        {
            return Vector2D(this->m_x * scalar, this->m_y * scalar);
        };

        Vector2D operator/(const float scalar) const
        {
            return Vector2D(this->m_x / scalar, this->m_y / scalar);
        };

        Vector2D& operator=(const Vector2D& other)
        {
            this->m_x = other.m_x;
            this->m_y = other.m_y;
            return *this;
        };

        bool operator==(const Vector2D& other) const
        {
            return this->m_x==other.m_x && this->m_y==other.m_y;
        };

        // overloading stream operator for vector
        friend std::ostream& operator<<(std::ostream& out, const Vector2D& v)
        {
            out << "(" << v.m_x << ","  << v.m_y << ")";
            return out;
        };

        // getters and setters
        float getX() const {return m_x; } ;
        float getY() const { return m_y; };
        void setX(float const x) { this->m_x = x; };
        void setY(float const y) { this->m_y = y; };

        // functions to calculate the norm and distance of the vector
        float euclidean_norm() const
        {
            return std::sqrt(m_x*m_x + m_y*m_y);
        };

        float euclidean_dist (const Vector2D& other) const
        {
            return std::sqrt(std::pow((this->m_x - other.m_x),2) + std::pow((this->m_y - other.m_y),2));
        };

        // function normalize for vector
        Vector2D normalize () const
        {
            if (this->euclidean_norm() > 0)
                return Vector2D(this->m_x /this->euclidean_norm(), this->m_y /this->euclidean_norm());
            return Vector2D(this->m_x, this->m_y);
        };

        Vector2D scale_to_length(float scalar) const
        {
            if (this->euclidean_norm() > 0) {
                float scale_factor = scalar / this->euclidean_norm();
                return Vector2D(this->m_x * scale_factor, this->m_y * scale_factor);
            }
            return Vector2D(this->m_x, this->m_y);
        };

    private :
        float m_x, m_y;
};

#endif /* SRC_LIVEOFBOIDS_VECTOR_2D_H_ */