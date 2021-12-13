

function(conan_message MESSAGE_OUTPUT)
    if(NOT CONAN_CMAKE_SILENT_OUTPUT)
        message(${ARGV${0}})
    endif()
endfunction()


macro(conan_find_apple_frameworks FRAMEWORKS_FOUND FRAMEWORKS FRAMEWORKS_DIRS)
    if(APPLE)
        foreach(_FRAMEWORK ${FRAMEWORKS})
            # https://cmake.org/pipermail/cmake-developers/2017-August/030199.html
            find_library(CONAN_FRAMEWORK_${_FRAMEWORK}_FOUND NAME ${_FRAMEWORK} PATHS ${FRAMEWORKS_DIRS} CMAKE_FIND_ROOT_PATH_BOTH)
            if(CONAN_FRAMEWORK_${_FRAMEWORK}_FOUND)
                list(APPEND ${FRAMEWORKS_FOUND} ${CONAN_FRAMEWORK_${_FRAMEWORK}_FOUND})
            else()
                message(FATAL_ERROR "Framework library ${_FRAMEWORK} not found in paths: ${FRAMEWORKS_DIRS}")
            endif()
        endforeach()
    endif()
endmacro()


function(conan_package_library_targets libraries package_libdir deps out_libraries out_libraries_target build_type package_name)
    unset(_CONAN_ACTUAL_TARGETS CACHE)
    unset(_CONAN_FOUND_SYSTEM_LIBS CACHE)
    foreach(_LIBRARY_NAME ${libraries})
        find_library(CONAN_FOUND_LIBRARY NAME ${_LIBRARY_NAME} PATHS ${package_libdir}
                     NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
        if(CONAN_FOUND_LIBRARY)
            conan_message(STATUS "Library ${_LIBRARY_NAME} found ${CONAN_FOUND_LIBRARY}")
            list(APPEND _out_libraries ${CONAN_FOUND_LIBRARY})
            if(NOT ${CMAKE_VERSION} VERSION_LESS "3.0")
                # Create a micro-target for each lib/a found
                string(REGEX REPLACE "[^A-Za-z0-9.+_-]" "_" _LIBRARY_NAME ${_LIBRARY_NAME})
                set(_LIB_NAME CONAN_LIB::${package_name}_${_LIBRARY_NAME}${build_type})
                if(NOT TARGET ${_LIB_NAME})
                    # Create a micro-target for each lib/a found
                    add_library(${_LIB_NAME} UNKNOWN IMPORTED)
                    set_target_properties(${_LIB_NAME} PROPERTIES IMPORTED_LOCATION ${CONAN_FOUND_LIBRARY})
                    set(_CONAN_ACTUAL_TARGETS ${_CONAN_ACTUAL_TARGETS} ${_LIB_NAME})
                else()
                    conan_message(STATUS "Skipping already existing target: ${_LIB_NAME}")
                endif()
                list(APPEND _out_libraries_target ${_LIB_NAME})
            endif()
            conan_message(STATUS "Found: ${CONAN_FOUND_LIBRARY}")
        else()
            conan_message(STATUS "Library ${_LIBRARY_NAME} not found in package, might be system one")
            list(APPEND _out_libraries_target ${_LIBRARY_NAME})
            list(APPEND _out_libraries ${_LIBRARY_NAME})
            set(_CONAN_FOUND_SYSTEM_LIBS "${_CONAN_FOUND_SYSTEM_LIBS};${_LIBRARY_NAME}")
        endif()
        unset(CONAN_FOUND_LIBRARY CACHE)
    endforeach()

    if(NOT ${CMAKE_VERSION} VERSION_LESS "3.0")
        # Add all dependencies to all targets
        string(REPLACE " " ";" deps_list "${deps}")
        foreach(_CONAN_ACTUAL_TARGET ${_CONAN_ACTUAL_TARGETS})
            set_property(TARGET ${_CONAN_ACTUAL_TARGET} PROPERTY INTERFACE_LINK_LIBRARIES "${_CONAN_FOUND_SYSTEM_LIBS};${deps_list}")
        endforeach()
    endif()

    set(${out_libraries} ${_out_libraries} PARENT_SCOPE)
    set(${out_libraries_target} ${_out_libraries_target} PARENT_SCOPE)
endfunction()


include(FindPackageHandleStandardArgs)

conan_message(STATUS "Conan: Using autogenerated FindPNG.cmake")
# Global approach
set(PNG_FOUND 1)
set(PNG_VERSION "1.6.37")

find_package_handle_standard_args(PNG REQUIRED_VARS
                                  PNG_VERSION VERSION_VAR PNG_VERSION)
mark_as_advanced(PNG_FOUND PNG_VERSION)


set(PNG_INCLUDE_DIRS "C:/Users/julian.auriac/.conan/data/libpng/1.6.37/_/_/package/ce650d9f1f1c1c0839cf0694a55c1351ddbed859/include")
set(PNG_INCLUDE_DIR "C:/Users/julian.auriac/.conan/data/libpng/1.6.37/_/_/package/ce650d9f1f1c1c0839cf0694a55c1351ddbed859/include")
set(PNG_INCLUDES "C:/Users/julian.auriac/.conan/data/libpng/1.6.37/_/_/package/ce650d9f1f1c1c0839cf0694a55c1351ddbed859/include")
set(PNG_RES_DIRS )
set(PNG_DEFINITIONS )
set(PNG_LINKER_FLAGS_LIST
        "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:>"
        "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:>"
        "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:>"
)
set(PNG_COMPILE_DEFINITIONS )
set(PNG_COMPILE_OPTIONS_LIST "" "")
set(PNG_COMPILE_OPTIONS_C "")
set(PNG_COMPILE_OPTIONS_CXX "")
set(PNG_LIBRARIES_TARGETS "") # Will be filled later, if CMake 3
set(PNG_LIBRARIES "") # Will be filled later
set(PNG_LIBS "") # Same as PNG_LIBRARIES
set(PNG_SYSTEM_LIBS )
set(PNG_FRAMEWORK_DIRS )
set(PNG_FRAMEWORKS )
set(PNG_FRAMEWORKS_FOUND "") # Will be filled later
set(PNG_BUILD_MODULES_PATHS )

conan_find_apple_frameworks(PNG_FRAMEWORKS_FOUND "${PNG_FRAMEWORKS}" "${PNG_FRAMEWORK_DIRS}")

mark_as_advanced(PNG_INCLUDE_DIRS
                 PNG_INCLUDE_DIR
                 PNG_INCLUDES
                 PNG_DEFINITIONS
                 PNG_LINKER_FLAGS_LIST
                 PNG_COMPILE_DEFINITIONS
                 PNG_COMPILE_OPTIONS_LIST
                 PNG_LIBRARIES
                 PNG_LIBS
                 PNG_LIBRARIES_TARGETS)

# Find the real .lib/.a and add them to PNG_LIBS and PNG_LIBRARY_LIST
set(PNG_LIBRARY_LIST libpng16)
set(PNG_LIB_DIRS "C:/Users/julian.auriac/.conan/data/libpng/1.6.37/_/_/package/ce650d9f1f1c1c0839cf0694a55c1351ddbed859/lib")

# Gather all the libraries that should be linked to the targets (do not touch existing variables):
set(_PNG_DEPENDENCIES "${PNG_FRAMEWORKS_FOUND} ${PNG_SYSTEM_LIBS} ZLIB::ZLIB")

conan_package_library_targets("${PNG_LIBRARY_LIST}"  # libraries
                              "${PNG_LIB_DIRS}"      # package_libdir
                              "${_PNG_DEPENDENCIES}"  # deps
                              PNG_LIBRARIES            # out_libraries
                              PNG_LIBRARIES_TARGETS    # out_libraries_targets
                              ""                          # build_type
                              "PNG")                                      # package_name

set(PNG_LIBS ${PNG_LIBRARIES})

foreach(_FRAMEWORK ${PNG_FRAMEWORKS_FOUND})
    list(APPEND PNG_LIBRARIES_TARGETS ${_FRAMEWORK})
    list(APPEND PNG_LIBRARIES ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${PNG_SYSTEM_LIBS})
    list(APPEND PNG_LIBRARIES_TARGETS ${_SYSTEM_LIB})
    list(APPEND PNG_LIBRARIES ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(PNG_LIBRARIES_TARGETS "${PNG_LIBRARIES_TARGETS};ZLIB::ZLIB")
set(PNG_LIBRARIES "${PNG_LIBRARIES};ZLIB::ZLIB")

set(CMAKE_MODULE_PATH "C:/Users/julian.auriac/.conan/data/libpng/1.6.37/_/_/package/ce650d9f1f1c1c0839cf0694a55c1351ddbed859/" ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH "C:/Users/julian.auriac/.conan/data/libpng/1.6.37/_/_/package/ce650d9f1f1c1c0839cf0694a55c1351ddbed859/" ${CMAKE_PREFIX_PATH})

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.0")
    # Target approach
    if(NOT TARGET PNG::PNG)
        add_library(PNG::PNG INTERFACE IMPORTED)
        if(PNG_INCLUDE_DIRS)
            set_target_properties(PNG::PNG PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                                  "${PNG_INCLUDE_DIRS}")
        endif()
        set_property(TARGET PNG::PNG PROPERTY INTERFACE_LINK_LIBRARIES
                     "${PNG_LIBRARIES_TARGETS};${PNG_LINKER_FLAGS_LIST}")
        set_property(TARGET PNG::PNG PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     ${PNG_COMPILE_DEFINITIONS})
        set_property(TARGET PNG::PNG PROPERTY INTERFACE_COMPILE_OPTIONS
                     "${PNG_COMPILE_OPTIONS_LIST}")
        
        # Library dependencies
        include(CMakeFindDependencyMacro)

        if(NOT ZLIB_FOUND)
            find_dependency(ZLIB REQUIRED)
        else()
            message(STATUS "Dependency ZLIB already found")
        endif()

    endif()
endif()

foreach(_BUILD_MODULE_PATH ${PNG_BUILD_MODULES_PATHS})
    include(${_BUILD_MODULE_PATH})
endforeach()
