#
# Find the Hypre includes and library
#
# This module defines
# MTL_INCLUDE_DIR, where to find headers,
# MTL_FOUND, If false, do not try to use MTL.

find_path(MTL_INCLUDE_DIR boost/numeric/mtl/mtl.hpp
  PATHS ${MTL_INCLUDE_PATH} NO_DEFAULT_PATH)

#find_library(MTL_LIBRARY pmtl4_security
#  PATHS ${MTL_LIBRARY_PATH} NO_DEFAULT_PATH)

set(MTL_FOUND "NO")
if(MTL_INCLUDE_DIR)
  set(MTL_FOUND "YES")
  set(MTL_INCLUDE_DIRS ${MTL_INCLUDE_DIR})
  set(MTL_FLAGS USE_MTL4)
  if(MTL_LIBRARY)
    set(MTL_LIBRARIES ${MTL_LIBRARY})
    set(MTL_FLAGS ${MTL_FLAGS} USE_PMTL4 MTL_HAS_MPI)
  endif(MTL_LIBRARY)
endif(MTL_INCLUDE_DIR)
