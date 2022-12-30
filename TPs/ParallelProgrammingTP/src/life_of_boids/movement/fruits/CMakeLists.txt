add_library(fruits eco.cpp eco.h fruit.cpp fruit.h tree.cpp tree.h)
target_include_directories(fruits
        PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/config
        ${CMAKE_SOURCE_DIR}/src/movement
        ${CMAKE_SOURCE_DIR}/src/movement/obstacles)