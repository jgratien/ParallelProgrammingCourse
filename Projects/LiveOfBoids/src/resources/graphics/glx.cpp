#include "glx.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>

VertexArray VertexArray_new() {
  GLuint vertex_array{};
  glGenVertexArrays(1, &vertex_array);
  glBindVertexArray(vertex_array);

  return VertexArray{vertex_array};
}

void VertexArray_bind(VertexArray& vertexArray) {
  glBindVertexArray(vertexArray.vertex_array);
}

Buffer Buffer_new() {
  GLuint vertex_buffer{};
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

  return Buffer{vertex_buffer};
}

void Buffer_bind(Buffer& buffer, GLenum target) {
  glBindBuffer(target, buffer.vertex_buffer);
}

ShaderProgram ShaderProgram_new(const char* vertex_shader_text, const char* fragment_shader_text) {
  // TODO Manage errors
  const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr);
  glCompileShader(vertex_shader);

  const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr);
  glCompileShader(fragment_shader);

  const GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  return ShaderProgram{program};
}

void ShaderProgram_activate(ShaderProgram& shaderProgram) {
  glUseProgram(shaderProgram.program);
}

GLint ShaderProgram_getAttribLocation(ShaderProgram& shaderProgram, const char* name) {
  const GLint loc = glGetAttribLocation(shaderProgram.program, name);
  return loc;
}

GLint ShaderProgram_getUniformLocation(ShaderProgram& shaderProgram, const char* name) {
  const GLint loc = glGetUniformLocation(shaderProgram.program, name);
  return loc;
}

void saveImage(char* filepath, GLFWwindow* w) {
  int width{}, height{};
  glfwGetFramebufferSize(w, &width, &height);
  GLsizei nrChannels = 3;
  GLsizei stride = nrChannels * width;
  // Most image file formats require the number of bytes used to store a single row of an image to be a multiple of 4.
  // As a default, OpenGL also “likes” packing image data in this way since the default value of GL_PACK_ALIGNMENT is 4.
  // I assume that this may enable OpenGL to do parallel computations more easily.
  //
  // Anyway it could be ok without the update of the stride (then GL_PACK_ALIGNMENT should be 1)
  stride += (stride % 4) ? (4 - stride % 4) : 0;
  GLsizei bufferSize = stride * height;
  std::vector<char> buffer(bufferSize);
  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glReadBuffer(GL_FRONT);
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());

  cv::Mat img(height, width, CV_8UC3, cv::Scalar(0, 0, 0));  // 8 bits unsigned integer with 3 color channels (BGR)
  for (int y = 0; y < img.rows; y++) {
    std::size_t pos = y * stride;
    for (int x = 0; x < img.cols; x++) {
      cv::Vec3b& color = img.at<cv::Vec3b>(height-1-y, x); // vertical flip of OpenGL buffer
      color[2] = buffer[pos++]; // reorder BGR colors
      color[1] = buffer[pos++];
      color[0] = buffer[pos++];
    }
  }
  imwrite(filepath, img);
}
