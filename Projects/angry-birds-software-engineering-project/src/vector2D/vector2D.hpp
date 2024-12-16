#ifndef DEF_VECTOR_2D_HPP
#define DEF_VECTOR_2D_HPP

#include <iostream>
#include <cmath>

class Vector2D 
{
    public :
        //constructor for vector
        Vector2D(float x = 0.0f, float y = 0.0f);

        // overloading operator for vector
        Vector2D operator + (const Vector2D& other) const;
        Vector2D operator - (const Vector2D& other) const;
        float operator * (const Vector2D& other);
        Vector2D operator*(const float scalar) const;
        Vector2D operator/(const float scalar) const;
        Vector2D& operator=(const Vector2D& other);
        bool operator==(const Vector2D& other) const;

        // overloading stream operator for vector
        friend std::ostream& operator<<(std::ostream& out, const Vector2D& v);

        // getters and setters
        float getX() const {return x; } ;
        float getY() const { return y; };
        void setX(float const x) { this->x = x; };
        void setY(float const y) { this->y = y; };

        // functions to calculate the norm and distance of the vector
        float euclidean_norm() const ;
        float euclidean_dist (const Vector2D& other) const;

        // function normalize for vector
        Vector2D normalize () const;
        Vector2D scale_to_length(float scalar) const;

    private :
        float x, y;
};

#endif // DEF_VECTOR_2D_HPP