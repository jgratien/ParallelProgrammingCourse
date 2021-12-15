FIND_PACKAGE(glfw3 3.3 REQUIRED) # REQUIRED means : "fatal if not found"
FIND_PACKAGE(glad REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(OpenMP REQUIRED)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

FIND_PACKAGE(TBB REQUIRED)

#add_library(libGLX "resources/graphics/glx.cpp" "resources/graphics/glx.hpp" "resources/graphics/GraphicalManager.hpp" "resources/graphics/GraphicalManager.cpp" "resources/graphics/graphics.hpp" "resources/graphics/graphics.cpp" "resources/graphics/oglTypes.hpp")
#target_link_libraries(libGLX PUBLIC glfw::glfw glad::glad opencv::opencv)

add_subdirectory("lib/myLaws")
add_subdirectory("lib/myMath")
add_subdirectory("resources/graphics")
add_subdirectory("resources/model")
add_subdirectory("resources/controller")
add_subdirectory("lib/Timers")

add_executable(mathPart math_main.cpp)
target_link_libraries(mathPart PUBLIC myMath)



add_subdirectory("resources/graphics/shaders")

add_executable(graphicsTest graphics_main.cpp)
target_link_libraries(graphicsTest PUBLIC libGLX myMath shaders)

add_executable(main_profiling main_profiling.cpp)
target_link_libraries(main_profiling PUBLIC libGLX myMath shaders myBirds myLaws opencv::opencv flockGenerator TBB::TBB mytimers)

add_executable(life-of-boids main.cpp)
target_link_libraries(life-of-boids PUBLIC libGLX myMath shaders myBirds myLaws mytimers opencv::opencv flockGenerator)

#add_executable(main_opencv main_opencv.cpp)
#target_link_libraries(main_opencv PUBLIC opencv::opencv)
