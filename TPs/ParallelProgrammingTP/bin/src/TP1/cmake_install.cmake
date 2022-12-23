# Install script for directory: /gext/maxime.naillon/Project/CPU/src/TP1

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/gext/maxime.naillon/Project/CPU")
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
  if(EXISTS "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_stdthread.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_stdthread.exe")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_stdthread.exe"
         RPATH "/gext/maxime.naillon/Project/CPU/lib:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/maxime.naillon/Project/CPU/bin/helloworld_stdthread.exe")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/gext/maxime.naillon/Project/CPU/bin" TYPE EXECUTABLE FILES "/gext/maxime.naillon/Project/CPU/bin/src/TP1/helloworld_stdthread.exe")
  if(EXISTS "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_stdthread.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_stdthread.exe")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_stdthread.exe"
         OLD_RPATH "/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib:::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/maxime.naillon/Project/CPU/lib:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_stdthread.exe")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_openmp.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_openmp.exe")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_openmp.exe"
         RPATH "/gext/maxime.naillon/Project/CPU/lib:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/maxime.naillon/Project/CPU/bin/helloworld_openmp.exe")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/gext/maxime.naillon/Project/CPU/bin" TYPE EXECUTABLE FILES "/gext/maxime.naillon/Project/CPU/bin/src/TP1/helloworld_openmp.exe")
  if(EXISTS "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_openmp.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_openmp.exe")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_openmp.exe"
         OLD_RPATH "/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib:::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/maxime.naillon/Project/CPU/lib:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_openmp.exe")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_task.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_task.exe")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_task.exe"
         RPATH "/gext/maxime.naillon/Project/CPU/lib:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/maxime.naillon/Project/CPU/bin/helloworld_task.exe")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/gext/maxime.naillon/Project/CPU/bin" TYPE EXECUTABLE FILES "/gext/maxime.naillon/Project/CPU/bin/src/TP1/helloworld_task.exe")
  if(EXISTS "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_task.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_task.exe")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_task.exe"
         OLD_RPATH "/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib:::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/maxime.naillon/Project/CPU/lib:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_task.exe")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_tbb.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_tbb.exe")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_tbb.exe"
         RPATH "/gext/maxime.naillon/Project/CPU/lib:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/maxime.naillon/Project/CPU/bin/helloworld_tbb.exe")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/gext/maxime.naillon/Project/CPU/bin" TYPE EXECUTABLE FILES "/gext/maxime.naillon/Project/CPU/bin/src/TP1/helloworld_tbb.exe")
  if(EXISTS "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_tbb.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_tbb.exe")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_tbb.exe"
         OLD_RPATH "/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib:::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/maxime.naillon/Project/CPU/lib:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_tbb.exe")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_mpi.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_mpi.exe")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_mpi.exe"
         RPATH "/gext/maxime.naillon/Project/CPU/lib:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/maxime.naillon/Project/CPU/bin/helloworld_mpi.exe")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/gext/maxime.naillon/Project/CPU/bin" TYPE EXECUTABLE FILES "/gext/maxime.naillon/Project/CPU/bin/src/TP1/helloworld_mpi.exe")
  if(EXISTS "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_mpi.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_mpi.exe")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_mpi.exe"
         OLD_RPATH "/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib:::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/maxime.naillon/Project/CPU/lib:/softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/maxime.naillon/Project/CPU/bin/helloworld_mpi.exe")
    endif()
  endif()
endif()

