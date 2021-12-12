cmake_minimum_required(VERSION 3.17)

## Forbids in-source builds (placed before PROJECT keyword)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)                                                                                                                                               

SET(CMAKE_COLOR_MAKEFILE ON)
# This message also avoid warning : "Manually-specified variables were not used by the project"
message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

#------------------------------------------------------
project(life_of_boids
        VERSION "12.5.9")
#------------------------------------------------------

set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PROJECT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

#------------------------------------------------------

# Generate position independent (aka -fPIC) code even for static libs
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#------------------------------------------------------

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#------------------------------------------------------

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CUSTOM_CXX_FLAGS "${CUSTOM_CXX_FLAGS} -Wall -Wextra")
    if (WIN32 AND MINGW) # https://stackoverflow.com/questions/16596876/object-file-has-too-many-sections
        set(CUSTOM_CXX_FLAGS "${CUSTOM_CXX_FLAGS} -Wa,-mbig-obj")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(CUSTOM_CXX_FLAGS "${CUSTOM_CXX_FLAGS} -Wall -Wextra")
    set(CUSTOM_CXX_FLAGS "${CUSTOM_CXX_FLAGS} -Wsign-compare -Wunused -Wunused-member-function -Wunused-private-field")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CUSTOM_CXX_FLAGS "${CUSTOM_CXX_FLAGS} /W4 /Zc:__cplusplus")
    # workaround like https://github.com/nlohmann/json/issues/1408
    # to avoid error like: carma\third_party\armadillo-code\include\armadillo_bits/arma_str.hpp(194): error C2039: '_snprintf': is not a member of 'std' (compiling source file carma\tests\src\bindings.cpp)
    ADD_DEFINITIONS(-DHAVE_SNPRINTF)
endif ()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CUSTOM_CXX_FLAGS}")

#------------------------------------------------------

add_subdirectory(src)

#------------------------------------------------------

enable_testing()
add_subdirectory(tests)

#------------------------------------------------------
