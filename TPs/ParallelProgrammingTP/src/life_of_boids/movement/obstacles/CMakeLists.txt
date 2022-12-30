add_library(obstacles obstacle.cpp obstacle.h)
target_include_directories(obstacles
        PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/config
        ${CMAKE_SOURCE_DIR}/src/movement)