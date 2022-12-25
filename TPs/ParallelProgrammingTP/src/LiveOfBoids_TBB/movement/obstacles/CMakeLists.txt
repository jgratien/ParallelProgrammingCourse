add_library(obstacles_tbb obstacle.cpp obstacle.h)
target_include_directories(obstacles_tbb
        PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/config
        ${CMAKE_SOURCE_DIR}/src/movement)