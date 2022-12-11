add_subdirectory(movement)
add_subdirectory(config)

add_executable(life-of-boids-tbb main.cpp coupled.cpp main.h)
target_include_directories(life-of-boids-tbb
        PUBLIC
        movement/
        config/)
target_link_libraries(life-of-boids-tbb PUBLIC movement config ${APPLI_LIBS})
INSTALL (TARGETS life-of-boids-tbb RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
