add_library(obstacles_omp obstacle.cpp obstacle.h)
target_include_directories(obstacles_omp
        PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/config
        ${CMAKE_SOURCE_DIR}/src/movement)
