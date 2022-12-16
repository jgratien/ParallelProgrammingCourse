add_subdirectory(agents)
add_subdirectory(obstacles)
add_subdirectory(fruits)

add_library(movement object.cpp object.h)
target_link_libraries(movement
        PUBLIC
        obstacles
        agents
        fruits)
target_include_directories(movement
        PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/config)
