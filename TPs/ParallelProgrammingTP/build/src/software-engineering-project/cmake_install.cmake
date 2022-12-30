# Install script for directory: /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/software-engineering-project

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP")
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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids"
         RPATH "/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/opencv/4.6.0/lib64:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin" TYPE EXECUTABLE FILES "/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/life-of-boids")
  if(EXISTS "$ENV{DESTDIR}/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids"
         OLD_RPATH "/softs/opencv/4.6.0/lib64:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/opencv/4.6.0/lib64:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/movement/cmake_install.cmake")
  include("/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/config/cmake_install.cmake")

endif()

