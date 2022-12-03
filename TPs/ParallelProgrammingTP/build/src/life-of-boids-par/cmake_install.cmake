# Install script for directory: /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-par

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP")
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
  if(EXISTS "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids"
         RPATH "/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin" TYPE EXECUTABLE FILES "/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par/life-of-boids")
  if(EXISTS "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids"
         OLD_RPATH "/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/life-of-boids")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par/movement/cmake_install.cmake")
  include("/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par/config/cmake_install.cmake")

endif()

