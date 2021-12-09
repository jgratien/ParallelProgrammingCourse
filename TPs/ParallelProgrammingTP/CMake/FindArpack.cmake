#
# Find Arpack includes
#
# This module defines
# ARPACK_INCLUDE_DIRS, where to find headers,
# ARPACK_LIBRARIES, the libraries to link against to use boost.
# ARPACK_FOUND If false, do not try to use boost.

find_path(ARPACK_INCLUDE_DIR arpackf.h
  PATHS ${ARPACK_INCLUDE_PATH} NO_DEFAULT_PATH)

foreach(_lib arpack)
  find_library(LIB_SUB_${_lib} ${_lib}
    PATHS ${ARPACK_LIBRARY_PATH} NO_DEFAULT_PATH)
  if(LIB_SUB_${_lib})
    set(ARPACK_LIBRARY ${ARPACK_LIBRARY} ${LIB_SUB_${_lib}})
  else(LIB_SUB_${_lib})
    set(ARPACK_LIBRARY_FAILED "YES")
  endif(LIB_SUB_${_lib})
endforeach(_lib)

set(ARPACK_FOUND "NO")
if(ARPACK_LIBRARY)
  set(ARPACK_FOUND "YES")
  set(ARPACK_LIBRARIES ${ARPACK_LIBRARY} ${SUPERLU_LIBRARIES} ${LAPACK_LIBRARIES} ${FORTRAN_LIBRARIES})
  set(ARPACK_INCLUDE_DIRS ${ARPACK_INCLUDE_DIR})
  SET(ARPACK_FLAGS "USE_ARPACK")
endif(ARPACK_LIBRARY)

