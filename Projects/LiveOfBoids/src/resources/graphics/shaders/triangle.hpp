#pragma once

#include <array>
#include <cmath>
#include "../oglTypes.hpp"
#include <cmath>

namespace triangle {

inline mat4x4 mat4x4_identity() {
  mat4x4 M;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      M[i][j] = i == j ? 1.f : 0.f;
  return M;
}

inline mat4x4 mat4x4_mul(mat4x4 const& a, mat4x4 const& b) {
  mat4x4 temp;
  for (int c = 0; c < 4; ++c)
    for (int r = 0; r < 4; ++r) {
      temp[c][r] = 0;
      for (int k = 0; k < 4; ++k)
        temp[c][r] += a[k][r] * b[c][k];
    }
  return temp;
}

inline mat4x4 mat4x4_rotate_Z(mat4x4 const& M, float angle) {
  float s = std::sin(angle);
  float c = std::cos(angle);
  mat4x4 R = {{
      {{c, s, 0, 0}},   //
      {{-s, c, 0, 0}},  //
      {{0, 0, 1, 0}},   //
      {{0, 0, 0, 1}}    //
  }};
  return mat4x4_mul(M, R);
}

inline mat4x4 mat4x4_ortho(float l, float r, float b, float t, float n, float f) {
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

struct Vertex {
  vec2 pos;
  vec3 col;
};

static const std::array<Vertex, 3> vertices = {{
    // (position 2d + color 3d pack)
    {{0, 0}, {1., 0., 0.}},  //
    {{0, 1}, {0., 1., 0.}},  //
    {{1, 0}, {0., 0., 1.}}   //
}};

static const char* const vertex_shader_text
    = "#version 330\n"
      "uniform mat4 MVP;\n"
      "in vec3 vCol;\n"
      "in vec2 vPos;\n"
      "out vec3 color;\n"
      "void main()\n"
      "{\n"
      "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
      "    color = vCol; // pass the color along to the fragment shader\n"
      "}\n";

static const char* const fragment_shader_text
    = "#version 330\n"
      "in vec3 color;\n"
      "out vec4 fragment;\n"
      "void main()\n"
      "{\n"
      "    // Set the fragment color to the color passed from the vertex shader\n"
      "    fragment = vec4(color, 1.0);\n"
      "}\n";

}  // namespace triangle