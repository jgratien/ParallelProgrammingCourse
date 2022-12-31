#include "display.h"

void error_callback(int error, const char* description) {
    std::cerr << "Error[" << error << "]: " << description << "\n";
}

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        std::puts("Export current display");
        char export_filename[] = "export.png";
        saveImage(export_filename, window);
    }
    // Add a new bird by positioning the cursor at the desired location and pressing "b"
    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &cursorX, &cursorY);
        addBird = true;
    }
    // Add a new predator by positioning the cursor at the desired location and pressing "p"
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &cursorX, &cursorY);
        addPredator = true;
    }
}

std::tuple<
        GLFWwindow*,
        VertexArray,
        VertexArray,
        VertexArray,
        VertexArray,
        VertexArray,
        Buffer,
        ShaderProgram,
        GLint> initWindow() {

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Triangle", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Set our call back functions for user interaction
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window); // Initialize GLEW
    gladLoadGL();
    glfwSwapInterval(1);

    int width{}, height{};
    glfwGetFramebufferSize(window, &width, &height); // Get window size

    // Triangle
    // New
    ShaderProgram triangle_shaderProgram
            = ShaderProgram_new(triangle::vertex_shader_text, triangle::fragment_shader_text);
    const GLint mvp_location = ShaderProgram_getUniformLocation(triangle_shaderProgram, "MVP");
    const GLint vpos_location = ShaderProgram_getAttribLocation(triangle_shaderProgram, "vPos");
    const GLint vcol_location = ShaderProgram_getAttribLocation(triangle_shaderProgram, "vCol");
    Buffer triangle_buffer = Buffer_new();

    // Birds triangle
    //New
    VertexArray triangleBird_vertexArray = VertexArray_new();
    // Init
    VertexArray_bind(triangleBird_vertexArray);
    Buffer_bind(triangle_buffer, GL_ARRAY_BUFFER);
    ShaderProgram_activate(triangle_shaderProgram);

    glVertexAttribPointer(
            vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, pos));
    glVertexAttribPointer(
            vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, col));
    glEnableVertexAttribArray(vpos_location);
    glEnableVertexAttribArray(vcol_location);

    // Birds triangle
    //New
    VertexArray trianglePred_vertexArray = VertexArray_new();
    // Init
    VertexArray_bind(trianglePred_vertexArray);
    Buffer_bind(triangle_buffer, GL_ARRAY_BUFFER);
    ShaderProgram_activate(triangle_shaderProgram);

    glVertexAttribPointer(
            vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, pos));
    glVertexAttribPointer(
            vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, col));
    glEnableVertexAttribArray(vpos_location);
    glEnableVertexAttribArray(vcol_location);

    // Obstacle triangle
    //New
    VertexArray triangleObs_vertexArray = VertexArray_new();
    // Init
    VertexArray_bind(triangleObs_vertexArray);
    Buffer_bind(triangle_buffer, GL_ARRAY_BUFFER);
    ShaderProgram_activate(triangle_shaderProgram);

    glVertexAttribPointer(
            vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, pos));
    glVertexAttribPointer(
            vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, col));
    glEnableVertexAttribArray(vpos_location);
    glEnableVertexAttribArray(vcol_location);

    // Tree triangle
    //New
    VertexArray triangleTree_vertexArray = VertexArray_new();
    // Init
    VertexArray_bind(triangleTree_vertexArray);
    Buffer_bind(triangle_buffer, GL_ARRAY_BUFFER);
    ShaderProgram_activate(triangle_shaderProgram);

    glVertexAttribPointer(
            vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, pos));
    glVertexAttribPointer(
            vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, col));
    glEnableVertexAttribArray(vpos_location);
    glEnableVertexAttribArray(vcol_location);


    // Fruit triangle
    //New
    VertexArray triangleFruit_vertexArray = VertexArray_new();
    // Init
    VertexArray_bind(triangleFruit_vertexArray);
    Buffer_bind(triangle_buffer, GL_ARRAY_BUFFER);
    ShaderProgram_activate(triangle_shaderProgram);

    glVertexAttribPointer(
            vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, pos));
    glVertexAttribPointer(
            vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(triangle::Vertex), (void*)offsetof(triangle::Vertex, col));
    glEnableVertexAttribArray(vpos_location);
    glEnableVertexAttribArray(vcol_location);

    return std::make_tuple(window, triangleObs_vertexArray, trianglePred_vertexArray,triangleBird_vertexArray, triangleTree_vertexArray, triangleFruit_vertexArray, triangle_buffer, triangle_shaderProgram, mvp_location);
}


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
        ) {

    int width{}, height{};
    glfwGetFramebufferSize(window, &width, &height); // Get window size
    Real ratio = (Real)width / (Real)height;

    mat4x4 p = triangle::mat4x4_ortho(-ratio, ratio, -1., 1., 1., -1.); // Projection matrix (Visualization operation)

    //Tree
    VertexArray_bind(triangleTree_vertexArray);
    Buffer_bind(triangle_buffer, GL_ARRAY_BUFFER);
    ShaderProgram_activate(triangle_shaderProgram);

    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&p);
    glBindVertexArray(triangleTree_vertexArray.vertex_array);

    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr (3 * trianglesTree.size() * sizeof(triangle::Vertex)), trianglesTree.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, GLsizei(3 * trianglesTree.size()));

    //Fruit
    VertexArray_bind(triangleFruit_vertexArray);
    Buffer_bind(triangle_buffer, GL_ARRAY_BUFFER);
    ShaderProgram_activate(triangle_shaderProgram);

    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&p);
    glBindVertexArray(triangleFruit_vertexArray.vertex_array);

    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr (3 * trianglesFruit.size() * sizeof(triangle::Vertex)), trianglesFruit.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, GLsizei(3 * trianglesFruit.size()));

    //Predators
    VertexArray_bind(trianglePred_vertexArray);
    Buffer_bind(triangle_buffer, GL_ARRAY_BUFFER);
    ShaderProgram_activate(triangle_shaderProgram);

    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&p);
    glBindVertexArray(trianglePred_vertexArray.vertex_array);

    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr (3 * trianglesPredators.size() * sizeof(triangle::Vertex)), trianglesPredators.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, GLsizei(3 * trianglesPredators.size()));

    //Birds
    VertexArray_bind(triangleBird_vertexArray);
    Buffer_bind(triangle_buffer, GL_ARRAY_BUFFER);
    ShaderProgram_activate(triangle_shaderProgram);

    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&p);
    glBindVertexArray(triangleBird_vertexArray.vertex_array);

    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr (3 * trianglesBirds.size() * sizeof(triangle::Vertex)), trianglesBirds.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, GLsizei(3 * trianglesBirds.size()));

    //Obstacles
    VertexArray_bind(triangleObs_vertexArray);
    Buffer_bind(triangle_buffer, GL_ARRAY_BUFFER);
    ShaderProgram_activate(triangle_shaderProgram);

    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&p);
    glBindVertexArray(triangleObs_vertexArray.vertex_array);

    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr (3 * trianglesObs.size() * sizeof(triangle::Vertex)), trianglesObs.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, GLsizei(3 * trianglesObs.size()));

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void display_FPS(std::chrono::time_point<std::chrono::high_resolution_clock>& start,
                double &total_fps,
                int &i, int &j) {
    i++;
    if (i == NUMBER_LOOP_FPS) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> float_ms = end - start;
        std::stringstream ss;
        double fps = (1 / (float_ms.count()) * 1000) * NUMBER_LOOP_FPS;
        std::cout << fps << std::endl;
        total_fps += fps;
        start = std::chrono::high_resolution_clock::now();
        i = 0;
        j++;
    }
}

void endWindow(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}