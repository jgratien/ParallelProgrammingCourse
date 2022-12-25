add_subdirectory(movement)
add_subdirectory(config)

add_executable(life-of-boids_TBB main.cpp coupled.cpp main.h)
target_include_directories(life-of-boids_TBB
        PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/config
        movement/
        config/)
target_link_libraries(life-of-boids_TBB PUBLIC movement config ${APPLI_LIBS})
INSTALL (TARGETS life-of-boids_TBB RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
