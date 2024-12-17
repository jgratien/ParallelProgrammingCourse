#pragma once
#include <iostream>
#include <string>
#include <cmath>

struct Vec2 {
    float x;
    float y;

    // Constructeurs
    Vec2(); // Constructeur par défaut initialisant à zéro
    Vec2(float x_val, float y_val); // Initialisation avec deux valeurs
    explicit Vec2(float value); // Initialisation avec une seule valeur pour les deux composantes
    Vec2(const Vec2& other) noexcept; // Constructeur de copie
    Vec2(Vec2&& other) noexcept; // Constructeur de déplacement
    ~Vec2() = default;

    // Opérateurs
    Vec2 operator+(const Vec2& other) const; // Addition de deux vecteurs
    Vec2& operator+=(const Vec2& other); // Addition et assignation
    Vec2& operator+(const float& other); //Addition d'un vecteur et un float

    Vec2 operator-(const Vec2& other) const; // Soustraction de deux vecteurs
    Vec2& operator-=(const Vec2& other); // Soustraction et assignation

    Vec2 operator*(const Vec2& other) const; // Multiplication terme à terme
    Vec2 operator*(float scalar) const; // Multiplication par un scalaire

    Vec2 operator/(const Vec2& other) const; // Division terme à terme
    Vec2 operator/(float scalar) const; // Division par un scalaire

    Vec2& operator*=(const Vec2& other); // Multiplication terme à terme et assignation
    Vec2& operator*=(float scalar); // Multiplication par un scalaire et assignation

    Vec2& operator/=(const Vec2& other); // Division terme à terme et assignation
    Vec2& operator/=(float scalar); // Division par un scalaire et assignation

    Vec2& operator=(const Vec2& other); // Assignation
    Vec2& operator=(Vec2&& other) noexcept; // Assignation par déplacement
    bool operator==(const Vec2& other) const; // Comparaison d'égalité

    // Méthodes supplémentaires
    float dot(const Vec2& other) const; // Produit scalaire
    float magnitude() const; // Norme du vecteur
    Vec2 normalized() const; // Normalisation du vecteur
    float angle() const; // Angle avec l'axe des x
    float angleBetween(const Vec2& other) const; // Angle entre deux vecteurs
    Vec2 rotated(float angle) const; // Rotation du vecteur
    std::string toString() const; // Conversion en chaîne de caractères
    float distanceTo(const Vec2& other) const;
    float wrappedDistanceTo(const Vec2& other, float gridWidth, float gridHeight) const;
    Vec2 wrappedPositionRelativeTo(const Vec2& reference, float gridWidth, float gridHeight) const;
    void limit(float min, float max);
    // Opérateur d'affichage pour ostream
    friend std::ostream& operator<<(std::ostream& os, const Vec2& vec);

};

std::ostream& operator<<(std::ostream& os, const Vec2& obj);