# force default to be empty
set (CMAKE_CXX_FLAGS_RELEASE "" CACHE STRING "release compile flags" FORCE)
set (CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "debug compile flags" FORCE)
set (CMAKE_CXX_FLAGS "" CACHE STRING "common compile flags" FORCE)
################################################################################
#
# EXTERNAL  SETTINGS
#
MESSAGE(STATUS "MPI TYPE : ${MPITYPE}")
IF(${MPITYPE} STREQUAL "OpenMPI")
INCLUDE(CMake/FindOpenMPI.cmake)
ELSE(${MPITYPE} STREQUAL "OpenMPI")
INCLUDE(CMake/FindMPI.cmake)
ENDIF()

include(CMake/FindFortran.cmake)

IF(NOT ENABLE_SHARED_LIB)
 SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
ENDIF(NOT ENABLE_SHARED_LIB)

INCLUDE(CMake/FindBoost.cmake)
INCLUDE(CMake/FindBlas.cmake)
INCLUDE(CMake/FindLapack.cmake)
INCLUDE(CMake/FindMKL.cmake)
INCLUDE(CMake/FindEigen3.cmake)
find_package(TBB)
#INCLUDE(CMake/FindTBB.cmake)
INCLUDE(CMake/FindNUMA.cmake)
INCLUDE(CMake/FindHWLOC.cmake)
INCLUDE(CMake/FindPAPI.cmake)
#INCLUDE(CMake/FindOpenCV.cmake)
# find_package(OpenCV COMPONENTS opencv_core opencv_highgui QUIET)
#INCLUDE(CMake/FindGoogleTools.cmake)
#INCLUDE(CMake/FindXML2.cmake)

#------------------------------------------------------------------------------#

IF(NOT FORTRAN_FOUND)
  MESSAGE(STATUS "PACKAGE: Fortran      : not found")
ELSE(NOT FORTRAN_FOUND)
  MESSAGE(STATUS "PACKAGE: Fortran      : ${FORTRAN_LIBRARIES}")
ENDIF(NOT FORTRAN_FOUND)

# Check if Mpi was found
IF(NOT MPI_FOUND)
  MESSAGE(STATUS "PACKAGE: Mpi          : not found")
ELSE(NOT MPI_FOUND)
  MESSAGE(STATUS "PACKAGE: Mpi          : ${MPITYPE}")
  MESSAGE(STATUS "PACKAGE: Mpi          : ${MPI_INCLUDE_DIR}")
  MESSAGE(STATUS "PACKAGE: Mpi          : ${MPI_LIBRARIES}")

  INCLUDE_DIRECTORIES(${MPI_INCLUDE_DIR})
  ADD_DEFINITIONS(-D_MPI)

ENDIF(NOT MPI_FOUND)



# Check if Boost was found
IF(NOT BOOST_FOUND)
  MESSAGE(FATAL_ERROR "PACKAGE: Boost        : not found")
ELSE(NOT BOOST_FOUND)
  MESSAGE(STATUS "PACKAGE: Boost        : ${BOOST_INCLUDE_DIR}")

  INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIR})
ENDIF(NOT BOOST_FOUND)

# Check if Blas was found
IF(NOT BLAS_FOUND)
  MESSAGE(STATUS "PACKAGE: Blas         : not found")
ELSE(NOT BLAS_FOUND)
  MESSAGE(STATUS "PACKAGE: Blas         : ${BLAS_LIBRARIES}")
  INCLUDE_DIRECTORIES(${BLAS_INCLUDE_DIRS})
ENDIF(NOT BLAS_FOUND)

# Check if Lapack was found
IF(NOT LAPACK_FOUND)
  MESSAGE(STATUS "PACKAGE: Lapack       : not found")
ELSE(NOT LAPACK_FOUND)
  MESSAGE(STATUS "PACKAGE: Lapack       : ${LAPACK_LIBRARIES}")
  INCLUDE_DIRECTORIES(${LAPACK_INCLUDE_DIRS})
ENDIF(NOT LAPACK_FOUND)

IF(MKL_FOUND)
  INCLUDE_DIRECTORIES(${MKL_INCLUDE_DIRS})
  MESSAGE(STATUS "PACKAGE: MKL          : ${MKL_LIBRARIES}")
  ADD_DEFINITIONS(-DUSE_MKL)
ELSE(NOT MKL_FOUND)
  MESSAGE(STATUS "PACKAGE: MKL          : not found")
ENDIF(MKL_FOUND)

IF(NOT EIGEN3_FOUND)
  MESSAGE(STATUS "PACKAGE: EIGEN3       : not found")
ELSE(NOT EIGEN3_FOUND)
  MESSAGE(STATUS "PACKAGE: EIGEN3       : ${EIGEN3_INCLUDE_DIRS}")
  INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIRS})
  ADD_DEFINITIONS(-DUSE_EIGEN3)
ENDIF(NOT EIGEN3_FOUND)






IF(NOT NUMA_FOUND)
  MESSAGE(STATUS "PACKAGE: Numa         : not found")
ELSE(NOT NUMA_FOUND)
  MESSAGE(STATUS "PACKAGE: Numa         : ${NUMA_INCLUDE_DIRS}")
  MESSAGE(STATUS "PACKAGE: Numa         : ${NUMA_LIBRARIES}")
  INCLUDE_DIRECTORIES(${NUMA_INCLUDE_DIRS})
  IF(ENABLE_NUMA)
  ADD_DEFINITIONS(-DUSE_NUMA)
  ENDIF(ENABLE_NUMA)
ENDIF(NOT NUMA_FOUND)

IF(NOT HWLOC_FOUND)
  MESSAGE(STATUS "PACKAGE: HWLOC        : not found")
ELSE(NOT HWLOC_FOUND)
  MESSAGE(STATUS "PACKAGE: HWLOC        : ${HWLOC_INCLUDE_DIRS}")
  MESSAGE(STATUS "PACKAGE: HWLOC        : ${HWLOC_LIBRARIES}")
  INCLUDE_DIRECTORIES(${HWLOC_INCLUDE_DIRS})
  IF(ENABLE_HWLOC)
  ADD_DEFINITIONS(-DUSE_HWLOC )
  ENDIF(ENABLE_HWLOC)
ENDIF(NOT HWLOC_FOUND)

IF(TBB_FOUND)
  MESSAGE(STATUS "PACKAGE: TBB        : ${TBB_INCLUDE_DIRS}")
  MESSAGE(STATUS "PACKAGE: TBB        : ${TBB_LIBRARIES}")
  INCLUDE_DIRECTORIES(${TBB_INCLUDE_DIRS})
  ADD_DEFINITIONS(-DUSE_TBB)
ELSE(TBB_FOUND)
  MESSAGE(STATUS "PACKAGE: TBB          : not found")
ENDIF(TBB_FOUND)


IF(NOT PAPI_FOUND)
  MESSAGE(STATUS "PACKAGE: PAPI         : not found")
ELSE(NOT PAPI_FOUND)
  MESSAGE(STATUS "PACKAGE: PAPI         : ${PAPI_INCLUDE_DIRS}")
  MESSAGE(STATUS "PACKAGE: PAPI         : ${PAPI_LIBRARIES}")

  INCLUDE_DIRECTORIES(${PAPI_INCLUDE_DIRS})
  MESSAGE(STATUS "PACKAGE: PAPI         : enabled with -DUSE_PAPI")
  ADD_DEFINITIONS(-DUSE_PAPI)

ENDIF(NOT PAPI_FOUND)


IF(NOT GOOGLEPROF_FOUND)
  MESSAGE(STATUS "PACKAGE: GoogleProf   : not found")
ELSE(NOT GOOGLEPROF_FOUND)
  MESSAGE(STATUS "PACKAGE: GoogleProf   : ${GOOGLEPROF_INCLUDE_DIRS}")
  MESSAGE(STATUS "PACKAGE: GoogleProf   : ${GOOGLEPROF_LIBRARIES}")
  INCLUDE_DIRECTORIES(${GOOGLEPROF_INCLUDE_DIRS})
  MESSAGE(STATUS "PACKAGE: GoogleProf         : enabled with -DUSE_GOOGLEPROF")
  ADD_DEFINITIONS(-DUSE_GOOGLEPROF)
ENDIF(NOT GOOGLEPROF_FOUND)

IF(NOT XML2_FOUND)
  MESSAGE(STATUS "PACKAGE: XML2         : not found")
ELSE(NOT XML2_FOUND)
  MESSAGE(STATUS "PACKAGE: XML2         : ${XML2_INCLUDE_DIRS}")
  MESSAGE(STATUS "PACKAGE: XML2         : ${XML2_LIBRARIES}")

  INCLUDE_DIRECTORIES(${XML2_INCLUDE_DIRS})
  MESSAGE(STATUS "PACKAGE: XML2         : enabled with -DUSE_XML2")
  ADD_DEFINITIONS(-DUSE_XML2)

ENDIF(NOT XML2_FOUND)

# IF(NOT OPENCV_FOUND)
#   MESSAGE(STATUS "PACKAGE: OpenCV       : not found")
# ELSE(NOT OPENCV_FOUND)
#   MESSAGE(STATUS "PACKAGE: OpenCV       : ${OpenCV_INCLUDE_DIRS}")
#   MESSAGE(STATUS "PACKAGE: OpenCV       : ${OpenCV_LIBRARIES}")
#
#   INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
#   MESSAGE(STATUS "PACKAGE: OPENCV       : enabled with -DUSE_OPENCV")
#   ADD_DEFINITIONS(-DUSE_OPENCV)
#   set(OPENCV_LIBRARIES ${OpenCV_LIBS})
#
# ENDIF(NOT OPENCV_FOUND)


#------------------------------------------------------------------------------#
#
#   LANGAGE SETTINGS
#
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fno-builtin -Wall -Wunknown-pragmas ")

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -DOPTIMIZE")

IF(${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
ELSE()
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3")
ENDIF()

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")


IF(ENABLE_AVX)
  # sandybridge is the first cpu that support AVX
  ADD_DEFINITIONS(-DUSE_AVX)
   if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -mavx")
  elseif(${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
    if( CMAKE_CXX_COMPILER_VERSION MATCHES "(17\\.)+")
      message( status "ICPC VERSION : 2017")
      SET(CMAKE_CXX_FLAGS "-I/soft/irsrvsoft1/expl/Intel_2017/include/icc ${CMAKE_CXX_FLAGS}")
      ADD_DEFINITIONS(-DUSE_ICC2017)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION MATCHES "(16\\.)+")
      message( status "ICPC VERSION : 2016")
      SET(CMAKE_CXX_FLAGS "-I/soft/irsrvsoft1/expl/Intel_2016.2/include ${CMAKE_CXX_FLAGS}")
      ADD_DEFINITIONS(-DUSE_ICC2016)
    endif()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -xAVX")
  endif()
  message(" Enable AVX (-DUSE_AVX)")
ENDIF(ENABLE_AVX)

IF(ENABLE_AVX2)
  # haswell is the first cpu that support AVX2
  ADD_DEFINITIONS(-DUSE_AVX2)
  if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma")
  elseif(${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
    if( CMAKE_CXX_COMPILER_VERSION MATCHES "(17\\.)+")
      message( status "ICPC VERSION : 2017")
      SET(CMAKE_CXX_FLAGS "-I/soft/irsrvsoft1/expl/Intel_2017/include/icc ${CMAKE_CXX_FLAGS}")
      ADD_DEFINITIONS(-DUSE_ICC2017)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION MATCHES "(16\\.)+")
      message( status "ICPC VERSION : 2016")
      SET(CMAKE_CXX_FLAGS "-I/soft/irsrvsoft1/expl/Intel_2016.2/include ${CMAKE_CXX_FLAGS}")
      ADD_DEFINITIONS(-DUSE_ICC2016)
    endif()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -xCORE-AVX2 -fma")
  endif()
  message(" Enable AVX2 (-DUSE_AVX2)")
ENDIF(ENABLE_AVX2)



IF(ENABLE_GPROF)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
  SET(CUDA_NVCC_FLAG "${CUDA_NVCC_FLAG} --profile")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
  SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -pg")
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
ENDIF(ENABLE_GPROF)

IF(ENABLE_NVTX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_NVTX")
  message(" Enable NVTX (-DUSE_NVTX)")
ENDIF(ENABLE_NVTX)

IF(ENABLE_OPENMP)
  ADD_DEFINITIONS(-DUSE_OMP)
  if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
      message(FATAL_ERROR "GCC version :${CMAKE_CXX_COMPILER_VERSION} must be at least 4.9 with OPENMP")
    endif()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
  elseif(${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qopenmp")
  elseif(${CMAKE_C_COMPILER_ID} MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.8)
      message(FATAL_ERROR "GCC version :${CMAKE_CXX_COMPILER_VERSION} must be at least 3.8 with OPENMP")
    endif()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
  endif()
  message(" Enable OpenMP (-DUSE_OMP)")
ENDIF(ENABLE_OPENMP)

IF(ENABLE_PERFCOUNTER)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DUSE_PERFCOUNTER")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DUSE_PERFCOUNTER")
ENDIF(ENABLE_PERFCOUNTER)

#------------------------------------------------------------------------------#

MACRO(MAKE_PACKAGE)
 MESSAGE(STATUS "Package ${CURRENT_LIB} : List of directories ${ARGV}")
 FOREACH(_dir ${ARGV})
   FILE(GLOB LOCAL_SRC_LIST ${_dir}/*.cc ${_dir}/*.cpp ${_dir}/*.f90)
   LIST(APPEND PACKAGE_SRC_LIST ${LOCAL_SRC_LIST})
   FILE(GLOB LOCAL_CU_LIST ${_dir}/*.cu)
   LIST(APPEND PACKAGE_CU_LIST ${LOCAL_CU_LIST})
   FILE(GLOB _rel_dir RELATIVE ${PROJECT_ROOT} ${_dir})
   FILE(GLOB LOCAL_HEADER_LIST ${_dir}/*.h)
   LIST(APPEND PACKAGE_HEADER_LIST ${LOCAL_HEADER_LIST})
   INCLUDE_DIRECTORIES(${BUILDROOT}/${_rel_dir})
 ENDFOREACH (_dir)



 MESSAGE(STATUS "Package ${CURRENT_LIB} List of source files :${PACKAGE_SRC_LIST}")
 IF(ENABLE_SHARED_LIB)
 ADD_LIBRARY(${CURRENT_LIB} SHARED ${PACKAGE_SRC_LIST} ${PACKAGE_AXL_H_LIST} ${PACKAGE_CU_LIST})
 #ADD_LIBRARY(${CURRENT_LIB} SHARED ${PACKAGE_SRC_LIST} ${PACKAGE_AXL_H_LIST} $<TARGET_OBJECTS:${CURRENT_CUDA_LIB}>)
 ELSEIF(NOT ENABLE_SHARED_LIB)
 ADD_LIBRARY(${CURRENT_LIB} STATIC ${PACKAGE_SRC_LIST} ${PACKAGE_AXL_H_LIST} ${PACKAGE_CU_LIST})
 ENDIF(ENABLE_SHARED_LIB)

 target_link_libraries(${CURRENT_LIB} pthread)

 IF(ENABLE_SHARED_LIB)
 INSTALL (TARGETS ${CURRENT_LIB} LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
 ELSEIF(NOT ENABLE_SHARED_LIB)
 INSTALL (TARGETS ${CURRENT_LIB} ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"  LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
 ENDIF(ENABLE_SHARED_LIB)



 MESSAGE(STATUS "Package ${CURRENT_LIB} List of header files :${PACKAGE_HEADER_LIST}")
 FOREACH(HEADER ${PACKAGE_HEADER_LIST})
   FILE(RELATIVE_PATH RELHEADER  ${PROJECT_ROOT}/src/MCGS ${HEADER})
   string(REGEX MATCH "(.*)[/\\]" DIR ${RELHEADER})
   INSTALL(FILES ${HEADER} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${DIR}")
 ENDFOREACH(HEADER PACKAGE_HEADER_LIST HEADERS)

ENDMACRO(MAKE_PACKAGE)


MACRO(INSTALL_HEADERS)
 MESSAGE(STATUS "List of directories ${ARGV}")
 FOREACH(_dir ${ARGV})
   FILE(GLOB LOCAL_HEADER_LIST ${_dir}/*.h)
   LIST(APPEND PACKAGE_HEADER_LIST ${LOCAL_HEADER_LIST})
   INCLUDE_DIRECTORIES(${BUILDROOT}/${_rel_dir})
 ENDFOREACH (_dir)
 MESSAGE(STATUS "List of header files :${PACKAGE_HEADER_LIST}")
 FOREACH(HEADER ${PACKAGE_HEADER_LIST})
   FILE(RELATIVE_PATH RELHEADER  ${PROJECT_ROOT}/src ${HEADER})
   string(REGEX MATCH "(.*)[/\\]" DIR ${RELHEADER})
   INSTALL(FILES ${HEADER} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${DIR}")
 ENDFOREACH(HEADER PACKAGE_HEADER_LIST HEADERS)
ENDMACRO(INSTALL_HEADERS)


# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# force values in cache
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "release compile flags" FORCE)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "debug compile flags" FORCE)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "common compile flags" FORCE)
