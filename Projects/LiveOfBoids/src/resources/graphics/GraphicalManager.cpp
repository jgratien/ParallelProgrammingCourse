#ifndef __GNUC__
#pragma region declarations
#endif
#include <vector>
#include <tuple>
#include "GraphicalManager.hpp"
#include "glx.hpp"
#include "graphics.hpp"
#include "../controller/TriangleDisplayer.hpp"
#include "../controller/DotDisplayer.hpp"
#include "shaders/lines.hpp"
#include "shaders/points.hpp"
#include "shaders/triangle.hpp"
#include "../model/Flock.hpp"
#include "../model/Bird.hpp"
#include "../model/Eagle.hpp"
#include <string>
#include <iomanip>
#include <chrono>
#include <omp.h>
#include <sstream>
#ifndef __GNUC__
#pragma endregion
#endif

bool run_boids = true; //used to pause the runtime
bool manual_hunt = false; //used when user presses or relase CTRL to generate a manual predator or one with a law
bool prettyAgents = true; //to change agent display
Agent* e; //used to add a new eagle in the key callback

GraphicalManager::GraphicalManager(Color myBackgroundColor, bool fullScreen) {

	m_background_color = myBackgroundColor;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	// Specifying some OpenGL variables
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

	if (fullScreen) {
		m_window = glfwCreateWindow(1000, 850, "Life-of-boids", primaryMonitor, nullptr);
	}
	else {
		m_window = glfwCreateWindow(1000, 850, "Life-of-boids", nullptr, nullptr);
	}


	if (!m_window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}


	glfwSetKeyCallback(m_window, key_callback);
	glfwSetMouseButtonCallback(m_window, mouse_button_callback);

	glfwMakeContextCurrent(m_window);
	gladLoadGL();
	glfwSwapInterval(1);


	glfwGetFramebufferSize(m_window, &m_width, &m_height);

	//BACKGROUND COLOR
	switch (m_background_color)
	{
	case Color::Red:
		glClearColor(1.f, 0.f, 0.f, 1.0f);
		break;
	case Color::Green:
		glClearColor(0.219607f, 0.55686f, 0.235294f, 1.0f);
		break;
	case Color::Blue:
		glClearColor(0.f, 0.f, 1.f, 1.0f);
		break;
	case Color::Black:
		glClearColor(0.25882f, 0.25882f, 0.25882f, 1.0f);
		break;
	case Color::BlueGrey:
		//glClearColor(0.1019607f, 0.1372549f, 0.4941176f, 1.0f);
		glClearColor(0.128f, 0.128f, 0.128f, 1.0f);
		break;
	case Color::Grey:
		glClearColor(0.62f, 0.62f, 0.62f, 1.0f);
		break;
	default:
		glClearColor(0.07f, 0.13f, 0.17f, 1.f);
		break;
	}

	// Points part
	// new
	points_shaderProgram = ShaderProgram_new(points::vertex_shader_text, points::fragment_shader_text);
	points_vertexArray = VertexArray_new();
	points_buffer = Buffer_new();

	VertexArray_bind(points_vertexArray);
	Buffer_bind(points_buffer, GL_ARRAY_BUFFER);
	ShaderProgram_activate(points_shaderProgram);

	m_transform_location = ShaderProgram_getUniformLocation(points_shaderProgram, "transform");
	m_pointSize_location = ShaderProgram_getUniformLocation(points_shaderProgram, "pointSize");
	m_maxSpeedSquared_location = ShaderProgram_getUniformLocation(points_shaderProgram, "maxSpeedSquared");
	m_position_location = ShaderProgram_getAttribLocation(points_shaderProgram, "position");
	m_velocity_location = ShaderProgram_getAttribLocation(points_shaderProgram, "velocity");

	glVertexAttribPointer(
		m_position_location, 2, GL_FLOAT, GL_FALSE, sizeof(points::Vertex), (void*)offsetof(points::Vertex, pos));
	glVertexAttribPointer(
		m_velocity_location, 2, GL_FLOAT, GL_FALSE, sizeof(points::Vertex), (void*)offsetof(points::Vertex, col));

	glEnableVertexAttribArray(m_position_location);
	glEnableVertexAttribArray(m_velocity_location);

	glEnable(GL_PROGRAM_POINT_SIZE);


	// Lines part
	// new
	lines_shaderProgram = ShaderProgram_new(lines::vertex_shader_text, lines::fragment_shader_text);
	lines_vertexArray = VertexArray_new();
	lines_buffer = Buffer_new();
	// init
	VertexArray_bind(lines_vertexArray);
	Buffer_bind(lines_buffer, GL_ARRAY_BUFFER);
	ShaderProgram_activate(lines_shaderProgram);

	m_transform_location2 = ShaderProgram_getUniformLocation(lines_shaderProgram, "transform");
	m_vpos_location2 = ShaderProgram_getAttribLocation(lines_shaderProgram, "vPos");
	m_vcol_location2 = ShaderProgram_getAttribLocation(lines_shaderProgram, "vCol");

	glVertexAttribPointer(
		m_vpos_location2, 2, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, pos));
	glVertexAttribPointer(
		m_vcol_location2, 3, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, col));

	glEnableVertexAttribArray(m_vpos_location2);
	glEnableVertexAttribArray(m_vcol_location2);
}


GraphicalManager::~GraphicalManager() {
	glfwDestroyWindow(m_window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}


bool GraphicalManager::mainLoop() {
	TriangleDisplayer triangleDisplay{};
	DotDisplayer dotDisplayer{};

	std::vector<triangle::Vertex> vertex_data_triangle;
	std::vector<points::Vertex> vertex_data_dots;

	if (!glfwWindowShouldClose(m_window)) {
		auto start = std::chrono::high_resolution_clock::now();

		glfwGetFramebufferSize(m_window, &m_width, &m_height);

		mat3x3 transform = points::vertex_transform_2d((float)m_width, (float)m_height);

		glViewport(0, 0, m_width, m_height);
		glClear(GL_COLOR_BUFFER_BIT);


		{
			if ((*MAIN_pFLOCK).optimized_computing) {
				(*MAIN_pFLOCK).updateAgents();
			}

			int nb_threads = 4;
			omp_set_num_threads(nb_threads);
			#pragma omp parallel for shared(MAIN_pFLOCK)
			for (int i = 0; i < (*MAIN_pFLOCK).getPopSize(); ++i){
				Agent *bird = (*MAIN_pFLOCK)[i];
				std::tuple<std::vector<Agent*>, std::vector<Agent*>> allNeighbors;

				if ((*MAIN_pFLOCK).optimized_computing) {
					allNeighbors = (*MAIN_pFLOCK).computeNeighbors(*bird);
				}
				else {
					allNeighbors = (*MAIN_pFLOCK).computeNeighborsOrigin(*bird);
				}

				std::vector<Agent*> bVec = std::get<0>(allNeighbors);
				std::vector<Agent*> eVec = std::get<1>(allNeighbors);

				if (run_boids) {
					(*bird).computeLaws(bVec, eVec);
					(*bird).prepareMove();
				}


				if (run_boids) {
					(*bird).setNextPosition(keepPositionInScreen((*bird).getNextPosition(), (float)m_width, (float)m_height));
					(*bird).move();
				}

				if (prettyAgents) {
                    // Fill vertex array of groups of 6 points each for double triangles
                    mat2x6 result = triangleDisplay.drawAgent(bird);
                    #pragma omp critical
                    {
                        for (int j = 0; j < result.size(); ++j) {
                            vertex_data_triangle.push_back(triangle::Vertex{ {result[j].x, result[j].y }, (*bird).getGLColor() });
                        }
                    }
                }
                else {
                    // Fill vertex array of points for each agents
                    Vec2 res = (dotDisplayer.drawAgent(bird))[0];
                    #pragma omp critical
                    {
                        vertex_data_dots.push_back(points::Vertex{ {res.x, res.y}, (*bird).getGLColor() });
                    }
                }

			}

			(*MAIN_pFLOCK).destroyAgents();

			if (prettyAgents) {
				// DRAW AGENTS AS TRIANGLES
				VertexArray_bind(lines_vertexArray);
				Buffer_bind(lines_buffer, GL_ARRAY_BUFFER);
				ShaderProgram_activate(lines_shaderProgram);

				glUniformMatrix3fv(m_transform_location2, 1, GL_FALSE, (const GLfloat*)&transform);
				glBindVertexArray(lines_vertexArray.vertex_array);
				glBufferData(GL_ARRAY_BUFFER, vertex_data_triangle.size() * sizeof(triangle::Vertex), vertex_data_triangle.data(), GL_STREAM_DRAW);
				glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertex_data_triangle.size());
			}
			else if (!prettyAgents) {
				// DRAW AGENTS AS DOTS
				VertexArray_bind(points_vertexArray);
				Buffer_bind(points_buffer, GL_ARRAY_BUFFER);
				ShaderProgram_activate(points_shaderProgram);

				glUniformMatrix3fv(m_transform_location, 1, GL_FALSE, (const GLfloat*)&transform);
				glUniform1f(m_pointSize_location, 3.f);
				glUniform1f(m_maxSpeedSquared_location, 10.f);
				glBindVertexArray(points_vertexArray.vertex_array);

				glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
				glBufferData(GL_ARRAY_BUFFER, vertex_data_dots.size() * sizeof(points::Vertex), vertex_data_dots.data(), GL_STREAM_DRAW);
				glDrawArrays(GL_POINTS, 0, (GLsizei)vertex_data_dots.size());
			}
		}

		//FPS calculations
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		std::ostringstream oss;
		oss << "Population : " << (int)(*MAIN_pFLOCK).getPopSize() << "                                                       FPS : " << (int)(1 / (duration.count() * 10e-7)) << std::endl;
		glfwSetWindowTitle(m_window, oss.str().c_str());

		// Swap buffers
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	return glfwWindowShouldClose(m_window);
}

std::vector<points::Point> GraphicalManager::createPoints(unsigned int number) {
	std::vector<points::Point> points(number);
	auto get_pos = [=](float t) {
		return Vec2{ (float)(m_width * (0.5 + 0.4 * cos(t))), (float)(m_height * (0.5 + 0.4 * sin(t))) };
	};

	float v = 0;
	for (auto& p : points) {
		v += 1.0;
		p = points::Point{ get_pos(v), Vec2{} };
	}

	return points;
}


/** Prints the error number and description
  *
  * @param error Error ID.
  * @param description Error text description.
  */
void error_callback(int error, const char* description) {
	std::cerr << "Error[" << error << "]: " << description << "\n";
}

/**
  * Actions to be taken when specific keys are pressed
  *
  * @param window our window
  * @param key which key
  * @param scancode
  * @param action action taken (press, unpress, repeat...)
  */
void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
	// If the user presses (GLFW_PRESS) escape key (GLFW_KEY_ESCAPE)
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		(*MAIN_pFLOCK).addAgent(new Bird{ Vec2{0 , 0}, Vec2{2.f,2.f},6, 300,50, 15.f, Color::Predator, CohesionLaw{0.0002f}, AlignmentLaw{0.3f}, SeparationLaw{0.9f} });
	}

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		//Change l'affichage de triangles en points
		(*MAIN_pFLOCK).destroyLastAgent();
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		//Change l'affichage de triangles en points
		prettyAgents = !prettyAgents;
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		//Met l'affichage en pause
		run_boids = !run_boids;
	}

	//Deplacement du predator principal
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		if (e != nullptr)
			e->setVelocity(Vec2{ e->getVelocity().x, e->getVelocity().y + 30 });
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		if (e != nullptr)
			e->setVelocity(Vec2{ e->getVelocity().x, e->getVelocity().y - 30 });
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		if (e != nullptr)
			e->setVelocity(Vec2{ e->getVelocity().x - 30, e->getVelocity().y });
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		if (e != nullptr)
			e->setVelocity(Vec2{ e->getVelocity().x + 30, e->getVelocity().y });
	}


	//Option pour generer des manual pedators ou non
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
		//Met l'affichage en pause
		manual_hunt = !manual_hunt;
	}
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
		//Met l'affichage en pause
		manual_hunt = !manual_hunt;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (MAIN_pFLOCK != nullptr) {
			double xpos, ypos;
			//getting cursor position
			glfwGetCursorPos(window, &xpos, &ypos);
			//Adding a new eagle and setting it to e so we can control it with ZQSD keys

			HuntingLaw hunt(MAIN_pFLOCK, manual_hunt);
			Color eagleColor = manual_hunt ? Color::LightRed : Color::Red;

			(*MAIN_pFLOCK).addAgent(e = new Eagle{ Vec2{(float)xpos, (float)ypos}, Vec2{-10.f,0.f},10, 50,100, 10.f, eagleColor , hunt });
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		if (MAIN_pFLOCK != nullptr) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			(*MAIN_pFLOCK).setAgentsToBeDestroyed(Vec2{ (float)xpos, (float)ypos }, 100);
		}
	}

}
