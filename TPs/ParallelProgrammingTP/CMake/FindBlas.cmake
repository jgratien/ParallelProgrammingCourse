#
# Find Blas libraries
#
# This module defines
# BLAS_LIBRARIES, the libraries to link against to use blas.
# BLAS_FOUND If false, do not try to use blas.

find_library(BLAS_LIBRARY blas
  PATHS ${BLAS_LIBRARY_PATH} NO_DEFAULT_PATH)

set(BLAS_FOUND "NO")
if(BLAS_LIBRARY)
  set(BLAS_FOUND "YES")
  set(BLAS_LIBRARIES ${BLAS_LIBRARY})
endif(BLAS_LIBRARY)




