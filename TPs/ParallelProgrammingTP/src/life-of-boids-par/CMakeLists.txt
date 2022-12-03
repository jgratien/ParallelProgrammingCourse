add_subdirectory(movement)
add_subdirectory(config)

add_executable(life-of-boids main.cpp coupled.cpp main.h)
target_include_directories(life-of-boids
        PUBLIC
        movement/
        config/)
target_link_libraries(life-of-boids PUBLIC movement config ${APPLI_LIBS})
INSTALL (TARGETS life-of-boids RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
