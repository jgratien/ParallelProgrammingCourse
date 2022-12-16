# Install script for directory: /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par")
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

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/softs/binutils/2.32/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/bin/life-of-boids" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/bin/life-of-boids")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/bin/life-of-boids"
         RPATH "/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/bin/life-of-boids")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/bin" TYPE EXECUTABLE FILES "/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/life-of-boids")
  if(EXISTS "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/bin/life-of-boids" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/bin/life-of-boids")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/bin/life-of-boids"
         OLD_RPATH ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/bin/life-of-boids")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/cmake_install.cmake")
  include("/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/config/cmake_install.cmake")

endif()

