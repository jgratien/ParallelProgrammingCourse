add_subdirectory(agents)
add_subdirectory(obstacles)
add_subdirectory(fruits)

add_library(movement_tbb object.cpp object.h)
target_link_libraries(movement_tbb
        PUBLIC
        obstacles_tbb
        agents_tbb
        fruits_tbb)
target_include_directories(movement
        PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/config)