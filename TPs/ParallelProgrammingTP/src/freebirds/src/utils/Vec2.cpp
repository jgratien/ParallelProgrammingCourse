#include "../include/utils/Vec2.hpp"

// Constructeur par défaut - initialise les coordonnées à zéro
Vec2::Vec2() : x(0.0f), y(0.0f) {}

// Constructeur avec valeurs spécifiques
Vec2::Vec2(float x_val, float y_val) : x(x_val), y(y_val) {}

// Constructeur avec une seule valeur pour les deux composantes
Vec2::Vec2(float value) : x(value), y(value) {}

// Constructeur de copie
Vec2::Vec2(const Vec2& other) noexcept : x(other.x), y(other.y) {}

// Constructeur de déplacement
Vec2::Vec2(Vec2&& other) noexcept : x(std::move(other.x)), y(std::move(other.y)) {}

// Addition de deux vecteurs
Vec2 Vec2::operator+(const Vec2& other) const {
    return Vec2(x + other.x, y + other.y);
}

Vec2& Vec2::operator+(const float& other) {
    x += other;
    y += other;
    return *this;
}

Vec2& Vec2::operator+=(const Vec2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

// Soustraction de deux vecteurs
Vec2 Vec2::operator-(const Vec2& other) const {
    return Vec2(x - other.x, y - other.y);
}

Vec2& Vec2::operator-=(const Vec2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

// Multiplication terme à terme
Vec2 Vec2::operator*(const Vec2& other) const {
    return Vec2(x * other.x, y * other.y);
}

// Multiplication par un scalaire
Vec2 Vec2::operator*(float scalar) const {
    return Vec2(x * scalar, y * scalar);
}

// Division terme à terme
Vec2 Vec2::operator/(const Vec2& other) const {
    return Vec2(x / other.x, y / other.y);
}

// Division par un scalaire
Vec2 Vec2::operator/(float scalar) const {
    return Vec2(x / scalar, y / scalar);
}

// Multiplication terme à terme et assignation
Vec2& Vec2::operator*=(const Vec2& other) {
    x *= other.x;
    y *= other.y;
    return *this;
}

// Multiplication par un scalaire et assignation
Vec2& Vec2::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

// Division terme à terme et assignation
Vec2& Vec2::operator/=(const Vec2& other) {
    x /= other.x;
    y /= other.y;
    return *this;
}

// Division par un scalaire et assignation
Vec2& Vec2::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

// Assignation par copie
Vec2& Vec2::operator=(const Vec2& other) {
    if (this != &other) {
        x = other.x;
        y = other.y;
    }
    return *this;
}

// Assignation par déplacement
Vec2& Vec2::operator=(Vec2&& other) noexcept {
    if (this != &other) {
        x = std::move(other.x);
        y = std::move(other.y);
    }
    return *this;
}

// Comparaison d'égalité
bool Vec2::operator==(const Vec2& other) const {
    return x == other.x && y == other.y;
}

// Produit scalaire
float Vec2::dot(const Vec2& other) const {
    return x * other.x + y * other.y;
}

// Norme du vecteur
float Vec2::magnitude() const {
    return std::sqrt(x * x + y * y);
}

// Normalisation du vecteur
Vec2 Vec2::normalized() const {
    float mag = magnitude();
    return (mag != 0.0f) ? Vec2(x / mag, y / mag) : Vec2(0.0f, 0.0f);
}

// Angle avec l'axe des x
float Vec2::angle() const {
    return std::atan2(y, x);
}

// Angle entre deux vecteurs
float Vec2::angleBetween(const Vec2& other) const {
    float dot_product = dot(other);
    float mags = magnitude() * other.magnitude();
    return (mags != 0.0f) ? std::acos(dot_product / mags) : 0.0f;
}

// Rotation du vecteur d'un angle donné
Vec2 Vec2::rotated(float angle) const {
    float cos_angle = std::cos(angle);
    float sin_angle = std::sin(angle);
    return Vec2(x * cos_angle - y * sin_angle, x * sin_angle + y * cos_angle);
}

// Conversion en chaîne de caractères
std::string Vec2::toString() const {
     return "Vec2(" + std::to_string(this->x) + ", " + std::to_string(this->y) + ")";
}

// Opérateur d'affichage pour ostream
std::ostream& operator<<(std::ostream& os, const Vec2& vec) {
    os << vec.toString();
    return os;
}

float Vec2::distanceTo(const Vec2& other) const {
        return std::sqrt((this->x - other.x) * (this->x - other.x) + (this->y - other.y) * (this->y - other.y));
}

float Vec2::wrappedDistanceTo(const Vec2& other, float gridWidth, float gridHeight) const {
                // Compute direct distances along each axis
                float dx = std::abs(this->x - other.x);
                float dy = std::abs(this->y - other.y);

                // Adjust for wrapping if shorter distance is available by wrapping
                if (dx > gridWidth / 2) dx = gridWidth - dx;
                if (dy > gridHeight / 2) dy = gridHeight - dy;

                // Return Euclidean distance considering wrapping
                return std::sqrt(dx * dx + dy * dy);
            }

Vec2 Vec2::wrappedPositionRelativeTo(const Vec2& reference, float gridWidth, float gridHeight) const {
    Vec2 adjustedPosition = *this;  // Start with the current position

    // Compute the shortest wrapped distance along the x-axis
    float dx = this->x - reference.x;
    if (std::abs(dx) > gridWidth / 2) {
        adjustedPosition.x = (dx > 0) ? this->x - gridWidth : this->x + gridWidth;
    }

    // Compute the shortest wrapped distance along the y-axis
    float dy = this->y - reference.y;
    if (std::abs(dy) > gridHeight / 2) {
        adjustedPosition.y = (dy > 0) ? this->y - gridHeight : this->y + gridHeight;
    }

    return adjustedPosition;
}


void Vec2::limit(float min, float max) {
    float mag = std::sqrt(this-> x * this-> x + this -> y * this -> y);
    if (mag != 0.0){
        if (mag > max) {
            this->x = (this->x / mag) * max;
            this->y = (this->y / mag) * max;
        } else  if (min > mag){
            this->x = (this->x / mag) * min;
            this->y = (this->y / mag) * min;
        }
    }
}