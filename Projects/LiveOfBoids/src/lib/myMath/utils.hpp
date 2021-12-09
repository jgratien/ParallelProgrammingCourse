#pragma once
#include "Vec2.hpp"
#include <vector>
#include <tuple>

float distance(const Vec2 a, const Vec2 b);

Vec2 barycenter(const std::vector<Vec2> vecs);

float radians(const float angle);

float degrees(const float angle);

bool isAntiClockwise(const Vec2 a, const Vec2 b);

Vec2 randomVec2Generation(const int& infBoundary, const int& supBoundary);

float random_float(const int &inf, const int &sup);

int researchX(const float& target, const std::vector<std::tuple<float, float, int>>& v);

int researchY(const float& target, const std::vector<std::tuple<float, float, int>>& v);
