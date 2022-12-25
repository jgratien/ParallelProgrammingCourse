FIND_PACKAGE(glfw3 3.3 REQUIRED) # REQUIRED means : "fatal if not found"
FIND_PACKAGE(glad REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)

add_library(display display.cpp display.h triangle.hpp glx.cpp glx.hpp)
target_link_libraries(display PUBLIC movement glfw::glfw glad::glad opencv::opencv)
target_include_directories(display
        PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/config
        ${CMAKE_SOURCE_DIR}/src/movement
        ${CMAKE_SOURCE_DIR}/src/movement/agents
        ${CMAKE_SOURCE_DIR}/src/movement/fruits
        ${CMAKE_SOURCE_DIR}/src/movement/obstacles)