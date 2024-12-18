#pragma once
#include <tuple>
#include <vector>

#include "Vec2.hpp"

float distance(const Vec2 a, const Vec2 b);

Vec2 barycenter(const std::vector<Vec2> vecs);

float radians(const float angle);

float degrees(const float angle);

bool isAntiClockwise(const Vec2 a, const Vec2 b);

Vec2 randomVec2Generation(int infBoundary, int supBoundary);

Vec2 randomGenerationOnScreen(int width, int height, float ratio);

Vec2 randomMovementGeneration(int infBoundary, int supBoundary);

float random_float(int inf = 0, int sup = 1);

int researchX(const float& target, const std::vector<std::tuple<float, float, int>>& v);

int researchY(const float& target, const std::vector<std::tuple<float, float, int>>& v);
