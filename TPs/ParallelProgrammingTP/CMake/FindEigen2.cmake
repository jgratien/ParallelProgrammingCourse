#
# Find Eigen2 includes
#
# This module defines
# EIGEN2_INCLUDE_DIRS, where to find headers,
# EIGEN2_LIBRARIES, the libraries to link against to use eigen.
# EIGEN2_FOUND If false, do not try to use eigen.

find_path(EIGEN2_INCLUDE_DIR Eigen
  PATHS ${EIGEN2_INCLUDE_PATH} NO_DEFAULT_PATH)

set( EIGEN2_FOUND "NO" )
if(EIGEN2_INCLUDE_DIR)
  set( EIGEN2_FOUND "YES" )
  set( EIGEN2_INCLUDE_DIRS ${EIGEN2_INCLUDE_DIR})
  set( EIGEN2_FLAGS "USE_EIGEN2")
endif(EIGEN2_INCLUDE_DIR)
