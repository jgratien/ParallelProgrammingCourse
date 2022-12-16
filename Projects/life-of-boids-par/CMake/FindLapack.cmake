#
# Find Lapack libraries
#
# This module defines
# LAPACK_LIBRARIES, the libraries to link against to use blas.
# LAPACK_FOUND If false, do not try to use blas.

find_library(LAPACK_LIBRARY lapack
  PATHS ${LAPACK_LIBRARY_PATH} NO_DEFAULT_PATH)

set(LAPACK_FOUND "NO")
if(LAPACK_LIBRARY)
  set(LAPACK_FOUND "YES")
  set(LAPACK_LIBRARIES ${LAPACK_LIBRARY})
endif(LAPACK_LIBRARY)




