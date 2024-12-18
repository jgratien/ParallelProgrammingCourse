########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(glad_COMPONENT_NAMES "")
if(DEFINED glad_FIND_DEPENDENCY_NAMES)
  list(APPEND glad_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES glad_FIND_DEPENDENCY_NAMES)
else()
  set(glad_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(glad_PACKAGE_FOLDER_RELEASE "/home/nsnidi/.conan2/p/b/gladab6415e82aab0/p")
set(glad_BUILD_MODULES_PATHS_RELEASE )


set(glad_INCLUDE_DIRS_RELEASE "${glad_PACKAGE_FOLDER_RELEASE}/include")
set(glad_RES_DIRS_RELEASE )
set(glad_DEFINITIONS_RELEASE )
set(glad_SHARED_LINK_FLAGS_RELEASE )
set(glad_EXE_LINK_FLAGS_RELEASE )
set(glad_OBJECTS_RELEASE )
set(glad_COMPILE_DEFINITIONS_RELEASE )
set(glad_COMPILE_OPTIONS_C_RELEASE )
set(glad_COMPILE_OPTIONS_CXX_RELEASE )
set(glad_LIB_DIRS_RELEASE "${glad_PACKAGE_FOLDER_RELEASE}/lib")
set(glad_BIN_DIRS_RELEASE )
set(glad_LIBRARY_TYPE_RELEASE STATIC)
set(glad_IS_HOST_WINDOWS_RELEASE 0)
set(glad_LIBS_RELEASE glad)
set(glad_SYSTEM_LIBS_RELEASE dl)
set(glad_FRAMEWORK_DIRS_RELEASE )
set(glad_FRAMEWORKS_RELEASE )
set(glad_BUILD_DIRS_RELEASE )
set(glad_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(glad_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${glad_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${glad_COMPILE_OPTIONS_C_RELEASE}>")
set(glad_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${glad_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${glad_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${glad_EXE_LINK_FLAGS_RELEASE}>")


set(glad_COMPONENTS_RELEASE )