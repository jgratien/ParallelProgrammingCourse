#
# Find MKL libraries
#
# This module defines
# MKL_LIBRARIES, the libraries to link against to use blas.
# MKL_FOUND If false, do not try to use blas.

foreach(_lib mkl_intel_lp64 mkl_core mkl_intel_thread)
  find_library(LIB_SUB_${_lib} ${_lib}
    PATHS ${MKL_LIBRARY_PATH} NO_DEFAULT_PATH)
  if(LIB_SUB_${_lib})
    set(MKL_LIBRARY ${MKL_LIBRARY} ${LIB_SUB_${_lib}})
  else(LIB_SUB_${_lib})
    set(MKL_LIBRARY_FAILED "YES")
  endif(LIB_SUB_${_lib})
endforeach(_lib)

foreach(_lib iomp5)
  find_library(LIB_SUB_${_lib} ${_lib}
    PATHS ${OMP_LIBRARY_PATH} NO_DEFAULT_PATH)
  if(LIB_SUB_${_lib})
    set(MKL_LIBRARY ${MKL_LIBRARY} ${LIB_SUB_${_lib}})
  else(LIB_SUB_${_lib})
    set(MKL_LIBRARY_FAILED "YES")
  endif(LIB_SUB_${_lib})
endforeach(_lib)

find_path(MKL_INCLUDE_DIR mkl_cblas.h
  PATHS ${MKL_INCLUDE_PATH} NO_DEFAULT_PATH)

set(MKL_FOUND "NO")
if(MKL_LIBRARY)
  set(MKL_FOUND "YES")
  # Probl�me avec cette commande !!!
  #GET_FILENAME_COMPONENT(MKL_LIB_PATH ${MKL_LIBRARY} PATH)
  set(MKL_LIBRARIES ${MKL_LIBRARY})
  set(MKL_INCLUDE_DIRS ${MKL_INCLUDE_DIR})
endif(MKL_LIBRARY)




