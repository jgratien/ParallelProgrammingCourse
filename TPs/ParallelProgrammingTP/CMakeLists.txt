PROJECT(ParallelProgramming)
#CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
cmake_minimum_required(VERSION 3.7.2)
cmake_policy(SET CMP0074 NEW)


#------------------------------------------------------------------------------#
#------------------------------------------------------------------------------#
MESSAGE(STATUS "PROJECT ARCH :${ARCH}")
MESSAGE(STATUS "BUILD ROOT :${BUILDROOT}")
MESSAGE(STATUS "PROJECT ROOT :${PROJECT_ROOT}")
IF (NOT CMAKE_BUILD_TYPE)
  SET (CMAKE_BUILD_TYPE RELEASE CACHE STRING
      "Choose the type of build, options are: None Debug Release."
      FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)

SET(PROJECT_NAME "ParallelProgramming")
SET(PROJECT_HOME_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
SET(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
SET(CMAKE_MODULE_PATH ${PROJECT_HOME_DIR}/CMake)
SET(CMAKE_INSTALL_PREFIX ${PROJECT_HOME_DIR})
#set(MPITYPE OpenMPI)
set(MPITYPE IntelMPI)
set(EIGEN3_INCLUDE_PATH    "$ENV{EIGEN3_ROOT}/include" CACHE PATH "root directory" FORCE)
set(EIGEN3_LIBRARY_PATH    "$ENV{EIGEN3_ROOT}/lib" CACHE PATH "root directory" FORCE)
set(TBB_INCLUDE_PATH       "$ENV{TBB_ROOT}/include" CACHE PATH "root directory" FORCE)
#set(TBB_LIBRARY_PATH       "$ENV{TBB_ROOT}/lib/intel64/gcc4.7" CACHE PATH "root directory" FORCE)
set(TBB_LIBRARY_PATH       "$ENV{TBB_ROOT}/lib" CACHE PATH "root directory" FORCE)
set(BOOST_INCLUDE_PATH     "$ENV{BOOST_ROOT}/include" CACHE PATH "root directory" FORCE)
set(BOOST_LIBRARY_PATH     "$ENV{BOOST_ROOT}/lib" CACHE PATH "root directory" FORCE)
SET(ENABLE_OPENMP TRUE CACHE BOOL "Project enable openmp" FORCE)



###############################################################################
#
# LANGAGE SETTINGS
#

INCLUDE(CMake/CMake.cmake)

###############################################################################
#
# PROJECT SETTINGS
#

SET(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
SET(TEST_DIR ${PROJECT_SOURCE_DIR}/test)
SET(BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)

INCLUDE_DIRECTORIES(${SRC_DIR})

MESSAGE(STATUS "APPLI_LIBS : ${APPLI_LIBS}")

INCLUDE(ParallelProgramming.cmake.in)

SET(EXTERNAL_LIBS ${BOOST_LIBRARIES}
                  ${EIGEN3_LIBRARIES}
                  ${TBB_LIBRARIES}
                  ${OpenCV_LIBRARIES}
                  ${MPI_LIBRARIES})
SET(EXTERNAL_LIBS pthread ${EXTERNAL_LIBS})

SET(APPLI_LIBS ${APPLI_LIB} ${EXTERNAL_LIBS})
SET(APPLI_LIBS ${EXTERNAL_LIBS} rt)

MESSAGE(STATUS "--> APPLI_LIBS : ${APPLI_LIBS}")


SET(CMAKE_VERBOSE TRUE)

set(ENABLE_SHARED_LIB TRUE)
IF(NOT ENABLE_SHARED_LIB)
 SET(BUILD_SHARED_LIBS OFF)
 SET(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
ENDIF(NOT ENABLE_SHARED_LIB)

ADD_DEFINITIONS(-Darch_Linux)

macro(GEN_EXE name)
  SET(EXE_NAME ${name}.exe)
  ADD_EXECUTABLE(${EXE_NAME} ${name}.cpp)
  TARGET_LINK_LIBRARIES(${EXE_NAME} ${APPLI_LIBS} ${EXTERNAL_LIBS})
  INSTALL (TARGETS ${EXE_NAME} RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
endmacro(GEN_EXE)

ADD_SUBDIRECTORY(${SRC_DIR}/TP1)
ADD_SUBDIRECTORY(${SRC_DIR}/TP2)
ADD_SUBDIRECTORY(${SRC_DIR}/TP3)
ADD_SUBDIRECTORY(${SRC_DIR}/TP4)



