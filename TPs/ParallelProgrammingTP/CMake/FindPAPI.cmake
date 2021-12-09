#
# Find MKL libraries
#
# This module defines
# BLAS_LIBRARIES, the libraries to link against to use blas.
# BLAS_FOUND If false, do not try to use blas.
 

 
foreach(_lib papi)
  find_library(LIB_SUB_${_lib} ${_lib}
    PATHS ${PAPI_LIBRARY_PATH} NO_DEFAULT_PATH)
  if(LIB_SUB_${_lib})
    set(PAPI_LIBRARY ${PAPI_LIBRARY} ${LIB_SUB_${_lib}})
  else(LIB_SUB_${_lib})
    set(PAPI_LIBRARY_FAILED "YES")
  endif(LIB_SUB_${_lib})
endforeach(_lib)

find_path(PAPI_INCLUDE_DIR papi.h
 PATHS 
 ${PAPI_INCLUDE_PATH}
 NO_DEFAULT_PATH)

set(PAPI_FOUND "NO")
if(PAPI_LIBRARY)
  set(PAPI_FOUND "YES")
  set(PAPI_LIBRARIES ${PAPI_LIBRARY})
  set(PAPI_INCLUDE_DIRS ${PAPI_INCLUDE_DIR})
endif(PAPI_LIBRARY)


