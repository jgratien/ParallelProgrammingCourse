add_subdirectory(movement)
add_subdirectory(config)

add_executable(life-of-boids-omp main.cpp coupled.cpp main.h)
target_include_directories(life-of-boids-omp
        PUBLIC
        movement/
        config/)
target_link_libraries(life-of-boids-omp PUBLIC movement_omp config_omp ${APPLI_LIBS})
INSTALL (TARGETS life-of-boids-omp RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
