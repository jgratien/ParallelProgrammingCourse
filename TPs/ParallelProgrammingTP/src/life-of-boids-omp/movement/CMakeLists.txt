add_subdirectory(agents)
add_subdirectory(obstacles)
add_subdirectory(fruits)

add_library(movement_omp object.cpp object.h)
target_link_libraries(movement_omp
        PUBLIC
        obstacles_omp
        agents_omp
        fruits_omp)
target_include_directories(movement_omp
        PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/config)
