########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(glad_FIND_QUIETLY)
    set(glad_MESSAGE_MODE VERBOSE)
else()
    set(glad_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/gladTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${glad_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(glad_VERSION_STRING "0.1.36")
set(glad_INCLUDE_DIRS ${glad_INCLUDE_DIRS_RELEASE} )
set(glad_INCLUDE_DIR ${glad_INCLUDE_DIRS_RELEASE} )
set(glad_LIBRARIES ${glad_LIBRARIES_RELEASE} )
set(glad_DEFINITIONS ${glad_DEFINITIONS_RELEASE} )


# Only the last installed configuration BUILD_MODULES are included to avoid the collision
foreach(_BUILD_MODULE ${glad_BUILD_MODULES_PATHS_RELEASE} )
    message(${glad_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


