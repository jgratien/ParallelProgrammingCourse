#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>

#include "config/config.h"
#include "glx.hpp"
#include "triangle.hpp"

// Adding agents initialization
extern bool addBird;
extern bool addPredator;
extern double cursorX;
extern double cursorY;

void error_callback(int error, const char* description);

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/);

std::tuple<GLFWwindow*, VertexArray, VertexArray, VertexArray, VertexArray, VertexArray, Buffer, ShaderProgram, GLint> initWindow();

void updateWindow(
        GLFWwindow* window,
        triangle::vertices_t& trianglesObs,
        triangle::vertices_t& trianglesPredators,
        triangle::vertices_t& trianglesBirds,
        triangle::vertices_t& trianglesTree,
        triangle::vertices_t& trianglesFruit,
        VertexArray& triangleObs_vertexArray,
        VertexArray& trianglePred_vertexArray,
        VertexArray& triangleBird_vertexArray,
        VertexArray& triangleTree_vertexArray,
        VertexArray& triangleFruit_vertexArray,
        Buffer& triangle_buffer,
        ShaderProgram& triangle_shaderProgram,
        GLint& mvp_location
        );

void display_FPS(
        std::chrono::time_point<std::chrono::high_resolution_clock>& start,
        double &total_fps,
        int &i,
        int &j
        );

void endWindow(GLFWwindow* window);