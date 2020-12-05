# Install script for directory: /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/TP4

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP")
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
  if(EXISTS "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img.exe")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img.exe"
         RPATH "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img.exe")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin" TYPE EXECUTABLE FILES "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4/img.exe")
  if(EXISTS "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img.exe")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img.exe"
         OLD_RPATH "/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img.exe")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_mpi.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_mpi.exe")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_mpi.exe"
         RPATH "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_mpi.exe")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin" TYPE EXECUTABLE FILES "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4/img_mpi.exe")
  if(EXISTS "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_mpi.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_mpi.exe")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_mpi.exe"
         OLD_RPATH "/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_mpi.exe")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean.exe")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean.exe"
         RPATH "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean.exe")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin" TYPE EXECUTABLE FILES "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4/img_kmean.exe")
  if(EXISTS "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean.exe")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean.exe"
         OLD_RPATH "/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean.exe")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean_mpi.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean_mpi.exe")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean_mpi.exe"
         RPATH "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean_mpi.exe")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin" TYPE EXECUTABLE FILES "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4/img_kmean_mpi.exe")
  if(EXISTS "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean_mpi.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean_mpi.exe")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean_mpi.exe"
         OLD_RPATH "/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_kmean_mpi.exe")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_tbb.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_tbb.exe")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_tbb.exe"
         RPATH "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_tbb.exe")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin" TYPE EXECUTABLE FILES "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4/img_tbb.exe")
  if(EXISTS "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_tbb.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_tbb.exe")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_tbb.exe"
         OLD_RPATH "/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/lib:/softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib:/gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/softs/binutils/2.32/bin/strip" "$ENV{DESTDIR}/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/bin/img_tbb.exe")
    endif()
  endif()
endif()

