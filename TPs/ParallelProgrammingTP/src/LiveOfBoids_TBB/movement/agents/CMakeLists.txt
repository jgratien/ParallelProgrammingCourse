add_library(agents_tbb agent.cpp agent.h bird.cpp bird.h predator.cpp predator.h)
target_include_directories(agents_tbb
        PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/config
        ${CMAKE_SOURCE_DIR}/src/movement
        ${CMAKE_SOURCE_DIR}/src/movement/obstacles
        ${CMAKE_SOURCE_DIR}/src/movement/fruits
        )