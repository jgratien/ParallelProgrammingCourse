#pragma once
#include <vector>
#include <array>
#include "glx.hpp"
#include "shaders/lines.hpp"
#include "shaders/points.hpp"
#include "shaders/triangle.hpp"
#include "../model/Flock.hpp"
#include "../controller/AgentDisplayer.hpp"
#include "../controller/flock_generator.hpp"

extern Flock* MAIN_pFLOCK;

class GraphicalManager {
private:
	GLFWwindow* m_window;
	int m_height;
	int m_width;
	Color m_background_color;

	GLint m_transform_location;
	GLint m_pointSize_location;
	GLint m_maxSpeedSquared_location;
	GLint m_position_location;
	GLint m_velocity_location;

	GLint m_transform_location2;
	GLint m_vpos_location2;
	GLint m_vcol_location2;

public:
	ShaderProgram points_shaderProgram;
	VertexArray points_vertexArray;
	Buffer points_buffer;

	ShaderProgram lines_shaderProgram;
	VertexArray lines_vertexArray;
	Buffer lines_buffer;

public:
	~GraphicalManager();
	GraphicalManager(Color myBackgroundColor, bool fullScreen);
	bool mainLoop();
	std::vector<points::Point> createPoints(unsigned int number);
};


void error_callback(int error, const char* description);

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
