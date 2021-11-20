#
# Find Eigen3 includes
#
# This module defines
# EIGEN3_INCLUDE_DIRS, where to find headers,
# EIGEN3_LIBRARIES, the libraries to link against to use eigen.
# EIGEN3_FOUND If false, do not try to use eigen.

find_path(EIGEN3_INCLUDE_DIR Eigen
  PATHS ${EIGEN3_INCLUDE_PATH} NO_DEFAULT_PATH)

set( EIGEN3_FOUND "NO" )
if(EIGEN3_INCLUDE_DIR)
  set( EIGEN3_FOUND "YES" )
  set( EIGEN3_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR})
  set( EIGEN3_FLAGS "USE_EIGEN3")
endif(EIGEN3_INCLUDE_DIR)
