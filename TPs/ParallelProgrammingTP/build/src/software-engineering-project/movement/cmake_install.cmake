# Install script for directory: /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/software-engineering-project/movement

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RELEASE")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/movement/agents/cmake_install.cmake")
  include("/gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/movement/obstacles/cmake_install.cmake")
  include("/gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/movement/fruits/cmake_install.cmake")

endif()

