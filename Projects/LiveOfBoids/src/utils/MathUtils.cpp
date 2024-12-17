#include "../include/utils/MathUtils.hpp"

#include <cmath>
#include <random>

mat3x3 vertex_transform_2d(float width, float height)
{
    return mat3x3{{{{2.f / width, 0.f, 0.f}}, {{0.f, -2.f / height, 0.f}}, {{-1.f, 1.f, 1.f}}}};
}

mat4x4 mat4x4_identity()
{
    mat4x4 M;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            M[i][j] = i == j ? 1. : 0.;
    return M;
}

mat4x4 mat4x4_mul(mat4x4 const& a, mat4x4 const& b)
{
    mat4x4 temp;
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
        {
            temp[c][r] = 0;
            for (int k = 0; k < 4; ++k)
                temp[c][r] += a[k][r] * b[c][k];
        }
    return temp;
}

mat4x4 mat4x4_rotate_Z(mat4x4 const& M, float angle)
{
    float s = std::sin(angle);
    float c = std::cos(angle);
    mat4x4 R = {{{{c, s, 0, 0}}, {{-s, c, 0, 0}}, {{0, 0, 1, 0}}, {{0, 0, 0, 1}}}};
    return mat4x4_mul(M, R);
}

mat4x4 mat4x4_ortho(float l, float r, float b, float t, float n, float f)
{
    mat4x4 M{};
    M[0][0] = 2 / (r - l);
    M[1][1] = 2 / (t - b);
    M[2][2] = -2 / (f - n);
    M[3][0] = -(r + l) / (r - l);
    M[3][1] = -(t + b) / (t - b);
    M[3][2] = -(f + n) / (f - n);
    M[3][3] = 1;
    return M;
}

mat2x6 mat2x6_triangle(Vec2 position, Vec2 velocity, float size) {
	float angle = velocity.angle();
	Vec2 front{ position + Vec2{ 1.f * size, 0 }.rotated(angle) } ;
	Vec2 backLeft{ position + Vec2{ - 1.f * size, 1.f * size }.rotated(angle) } ;
	Vec2 backRight{ position + Vec2{ -1.f * size, -1.f * size }.rotated(angle) } ;

    vec2 m_front{front.x, front.y};
    vec2 m_position{position.x, position.y};
    vec2 m_backLeft{backLeft.x, backLeft.y};
    vec2 m_backRight{backRight.x, backRight.y};

	mat2x6 r = { m_position, m_front, m_backLeft, m_position, m_front, m_backRight};
	return r;
}

