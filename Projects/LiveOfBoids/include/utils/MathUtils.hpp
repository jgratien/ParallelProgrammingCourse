#ifndef FREE_BIRDS_MATH_UTILS_HPP
#define FREE_BIRDS_MATH_UTILS_HPP

#include <array>
#include "Vec2.hpp"

// Type definitions
using vec2 = std::array<float, 2>;
using vec3 = std::array<float, 3>;
using vec4 = std::array<float, 4>;
using mat3x3 = std::array<vec3, 3>;
using mat4x4 = std::array<vec4, 4>;
using mat2x6 = std::array<vec2, 6>;

// Function declarations
mat3x3 vertex_transform_2d(float width, float height);
mat4x4 mat4x4_identity();
mat4x4 mat4x4_mul(mat4x4 const& a, mat4x4 const& b);
mat4x4 mat4x4_rotate_Z(mat4x4 const& M, float angle);
mat4x4 mat4x4_ortho(float l, float r, float b, float t, float n, float f);
mat2x6 mat2x6_triangle(Vec2 position, Vec2 velocity, float size = 7);

#endif  // FREE_BIRDS_MATH_UTILS_HPP