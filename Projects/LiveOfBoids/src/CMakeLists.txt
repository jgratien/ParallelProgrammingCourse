FIND_PACKAGE(glfw3 3.3 REQUIRED) # REQUIRED means : "fatal if not found"
FIND_PACKAGE(glad REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
find_package(OpenMP)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_package(TBB REQUIRED)

add_subdirectory("lib/myLaws")
add_subdirectory("lib/myMath")
add_subdirectory("lib/myTimer")
add_subdirectory("resources/graphics")
add_subdirectory("resources/model")
add_subdirectory("resources/controller")
add_subdirectory("resources/graphics/shaders")

add_executable(mathPart math_main.cpp)
target_link_libraries(mathPart PUBLIC myMath)

add_executable(main_profiling main_profiling.cpp)
target_link_libraries(main_profiling PUBLIC libGLX myMath shaders myBirds myLaws TBB::TBB opencv::opencv flockGenerator myTimer)

add_executable(life-of-boids main.cpp)
target_link_libraries(life-of-boids PUBLIC libGLX myMath shaders myBirds myLaws opencv::opencv flockGenerator)
