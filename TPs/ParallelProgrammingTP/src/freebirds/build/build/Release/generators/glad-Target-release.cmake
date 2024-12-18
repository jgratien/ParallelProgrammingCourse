# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(glad_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(glad_FRAMEWORKS_FOUND_RELEASE "${glad_FRAMEWORKS_RELEASE}" "${glad_FRAMEWORK_DIRS_RELEASE}")

set(glad_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET glad_DEPS_TARGET)
    add_library(glad_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET glad_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${glad_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${glad_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### glad_DEPS_TARGET to all of them
conan_package_library_targets("${glad_LIBS_RELEASE}"    # libraries
                              "${glad_LIB_DIRS_RELEASE}" # package_libdir
                              "${glad_BIN_DIRS_RELEASE}" # package_bindir
                              "${glad_LIBRARY_TYPE_RELEASE}"
                              "${glad_IS_HOST_WINDOWS_RELEASE}"
                              glad_DEPS_TARGET
                              glad_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "glad"    # package_name
                              "${glad_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${glad_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET glad::glad
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${glad_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${glad_LIBRARIES_TARGETS}>
                 )

    if("${glad_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET glad::glad
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     glad_DEPS_TARGET)
    endif()

    set_property(TARGET glad::glad
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${glad_LINKER_FLAGS_RELEASE}>)
    set_property(TARGET glad::glad
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${glad_INCLUDE_DIRS_RELEASE}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET glad::glad
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Release>:${glad_LIB_DIRS_RELEASE}>)
    set_property(TARGET glad::glad
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${glad_COMPILE_DEFINITIONS_RELEASE}>)
    set_property(TARGET glad::glad
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${glad_COMPILE_OPTIONS_RELEASE}>)

########## For the modules (FindXXX)
set(glad_LIBRARIES_RELEASE glad::glad)
