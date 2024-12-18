cmake_minimum_required(VERSION 3.17)

# CMake utils
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

## Forbids in-source builds (placed before PROJECT keyword)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This message also avoid warning : "Manually-specified variables were not used by the project"
message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

#------------------------------------------------------
project(free_birds
        VERSION "0.0.0")
#------------------------------------------------------

set(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PROJECT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

#------------------------------------------------------

option(ENABLE_COVERAGE "Enable coverage target" OFF)
option(ENABLE_MEMCHECK "Enable memcheck on tests" OFF)

#------------------------------------------------------

# Default build type is RelWIthDebInfo
if (NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
            "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel"
            FORCE)
else ()
    string(REGEX MATCH "^(Debug|Release|RelWithDebInfo|MinSizeRel)$" VALID_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
    if (VALID_BUILD_TYPE STREQUAL "")
        message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'")
    endif ()
endif ()

#------------------------------------------------------

# Can be locally overridden using set_target_properties + https://cmake.org/cmake/help/latest/prop_tgt/LANG_VISIBILITY_PRESET.html
#set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#set(CMAKE_POLICY_DEFAULT_CMP0063 NEW) # Honor the visibility properties for all target types

#------------------------------------------------------

# Generate position independent (aka -fPIC) code even for static libs
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#------------------------------------------------------

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#------------------------------------------------------

if (ENABLE_COVERAGE)
    # --coverage option is used to compile and link code instrumented for coverage analysis.
    # The option is a synonym for -fprofile-arcs -ftest-coverage (when compiling) and -lgcov (when linking).
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
endif ()

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

if (ENABLE_STATIC_ANALYSIS)
    # search for clang-tidy (while be used while adding library or executable)
    find_program(CLANG_TIDY clang-tidy)
    if (CLANG_TIDY)
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})
        message(STATUS "Static analysis enabled while compiling.")
    else ()
        message(FATAL_ERROR "Static analysis requested but not available.")
    endif ()
endif ()

#------------------------------------------------------

# valgrind must be checked before adding new subdirectories
# before include(CTest)
if (ENABLE_MEMCHECK)
    find_program(VALGRIND_EXECUTABLE valgrind)
    if (NOT VALGRIND_EXECUTABLE)
        MESSAGE(FATAL_ERROR "ENABLE_MEMCHECK is requested but valgrind executable cannot be found")
    endif ()
    set(MEMORYCHECK_COMMAND_OPTIONS "--tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=50 --error-exitcode=1 --errors-for-leak-kinds=definite")
    set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} -v") # verbose run with details
    # set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} -q") # for quiet runs
    # set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --gen-suppressions=all") # for setting up suppressions
    # set(MEMORYCHECK_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/.valgrind-suppressions")
    MESSAGE(STATUS "Memcheck on tests enabled")
endif ()

#------------------------------------------------------

add_subdirectory(src)


#------------------------------------------------------

# unit tests coverage

if (ENABLE_COVERAGE)
    find_program(LCOV lcov)
    if (NOT LCOV)
        logFatalError("lcov not found, cannot perform coverage.")
    endif ()

    # coveralls.io does not support striped paths
    #find_program (SED NAMES sed)
    #if (NOT SED)
    #    logFatalError("Unable to find sed")
    #else()
    #    # message(STATUS "sed found at ${SED}")
    #endif (NOT SED)

    # Don't forget '' around each pattern
    set(LCOV_EXCLUDE_PATTERN "'${CMAKE_CURRENT_SOURCE_DIR}/dependencies/*'")

    add_custom_target(coverage
            # Cleanup previously generated profiling data
            COMMAND ${LCOV} --base-directory ${CMAKE_CURRENT_SOURCE_DIR} --directory ${CMAKE_CURRENT_BINARY_DIR} --zerocounters
            # Initialize profiling data with zero coverage for every instrumented line of the project
            # This way the percentage of total lines covered will always be correct, even when not all source code files were loaded during the test(s)
            COMMAND ${LCOV} --base-directory ${CMAKE_CURRENT_SOURCE_DIR} --directory ${CMAKE_CURRENT_BINARY_DIR} --capture --initial --output-file coverage_base.info
            # Run tests
            COMMAND ${CMAKE_CTEST_COMMAND} -j ${PROCESSOR_COUNT}
            # Collect data from executions
            COMMAND ${LCOV} --base-directory ${CMAKE_CURRENT_SOURCE_DIR} --directory ${CMAKE_CURRENT_BINARY_DIR} --capture --output-file coverage_ctest.info
            # Combine base and ctest results
            COMMAND ${LCOV} --add-tracefile coverage_base.info --add-tracefile coverage_ctest.info --output-file coverage_full.info
            # Extract only project data (--no-capture or --remove options may be used to select collected data)
            COMMAND ${LCOV} --remove coverage_full.info ${LCOV_EXCLUDE_PATTERN} --output-file coverage_filtered.info
            COMMAND ${LCOV} --extract coverage_filtered.info '${CMAKE_CURRENT_SOURCE_DIR}/*' --output-file coverage.info
            # coveralls.io does not support striped paths
            #COMMAND ${SED} -i.bak 's|SF:${CMAKE_CURRENT_SOURCE_DIR}/|SF:|g' coverage.info
            DEPENDS all_test_binaries
            COMMENT "Running test coverage."
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            )

    find_program(GENHTML genhtml)
    if (NOT GENHTML)
        message(WARNING "genhtml not found, cannot perform report-coverage.")
    else ()
        add_custom_target(coverage-report
                COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/coverage"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/coverage"
                COMMAND ${GENHTML} -o coverage -t "${CMAKE_PROJECT_NAME} test coverage" --ignore-errors source --legend --num-spaces 4 coverage.info
                COMMAND ${LCOV} --list coverage.info
                DEPENDS coverage
                COMMENT "Building coverage html report."
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                )
    endif ()
else ()
    add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "*** Use CMAKE_BUILD_TYPE=Coverage option in cmake configuration to enable code coverage ***"
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMENT "Inform about not available code coverage."
            )
    add_custom_target(coverage-report DEPENDS coverage)
endif ()

#------------------------------------------------------

# search for clang-format and add target
find_program(CLANG_FORMAT clang-format)
if (CLANG_FORMAT)
    exec_program(${CLANG_FORMAT} ARGS -version
            OUTPUT_VARIABLE CLANG_FORMAT_RAW_VERSION)
    string(REGEX MATCH "[1-9][0-9]*\\.[0-9]+\\.[0-9]+"
            CLANG_FORMAT_VERSION ${CLANG_FORMAT_RAW_VERSION})
    if (CLANG_FORMAT_VERSION VERSION_GREATER_EQUAL "9.0.0")
        add_custom_target(clang-format
                COMMAND echo "running ${CLANG_FORMAT} ..."
                COMMAND ${CMAKE_COMMAND}
                -DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
                -DCLANG_FORMAT="${CLANG_FORMAT}"
                -P ${PROJECT_SOURCE_DIR}/cmake/ClangFormatProcess.cmake)
        message(STATUS "clang-format target for updating code format is available")
    else()
        message(WARNING "incompatible clang-format found (<6.0.0); clang-format target is not available.")
        add_custom_target(clang-format
                COMMAND ${CMAKE_COMMAND} -E echo ""
                COMMAND ${CMAKE_COMMAND} -E echo "*** code formatting not available since clang-format version is incompatible ***"
                COMMAND ${CMAKE_COMMAND} -E echo ""
                COMMENT "Inform about not available code format."
                )
    endif()
else ()
    message(WARNING "clang-format no found; clang-format target is not available.")
    add_custom_target(clang-format
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "*** code formatting not available since clang-format has not been found ***"
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMENT "Inform about not available code format."
            )
endif ()

#------------------------------------------------------