#ifndef LIFE_OF_BOIDS_SRC_GLX_HPP
#define LIFE_OF_BOIDS_SRC_GLX_HPP

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>


struct VertexArray {
  GLuint vertex_array;
};

VertexArray VertexArray_new();
void VertexArray_bind(VertexArray& vertexArray);

struct Buffer {
  GLuint vertex_buffer;
};

Buffer Buffer_new();
void Buffer_bind(Buffer& buffer, GLenum target);

struct ShaderProgram {
  GLuint program;
};

ShaderProgram ShaderProgram_new(const char* vertex_shader_text, const char* fragment_shader_text);
void ShaderProgram_activate(ShaderProgram& shaderProgram);
GLint ShaderProgram_getAttribLocation(ShaderProgram& shaderProgram, const char* name);
GLint ShaderProgram_getUniformLocation(ShaderProgram& shaderProgram, const char* name);

void saveImage(char* filepath, GLFWwindow* w);

#endif  // LIFE_OF_BOIDS_SRC_GLX_HPP
