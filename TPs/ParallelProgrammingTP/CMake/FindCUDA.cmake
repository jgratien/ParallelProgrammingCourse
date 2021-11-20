#
# Find CUDA libraries
#
# This module defines
# CUDA_LIBRARIES, the libraries to link against to use blas.
# CUDA_FOUND If false, do not try to use blas.
 

if(ENABLE_CUDA)
foreach(_lib cublas cudart cusparse)
  find_library(LIB_SUB_${_lib} ${_lib}
    PATHS ${CUDA_LIBRARY_PATH} NO_DEFAULT_PATH)
  if(LIB_SUB_${_lib})
    set(CUDA_LIBRARY ${CUDA_LIBRARY} ${LIB_SUB_${_lib}})
  else(LIB_SUB_${_lib})
    set(CUDA_LIBRARY_FAILED "YES")
  endif(LIB_SUB_${_lib})
endforeach(_lib)

if(ENABLE_NVTX)
  foreach(_lib nvToolsExt)
    find_library(LIB_SUB_${_lib} ${_lib}
      PATHS ${CUDA_LIBRARY_PATH} NO_DEFAULT_PATH)
    if(LIB_SUB_${_lib})
      set(CUDA_LIBRARY ${CUDA_LIBRARY} ${LIB_SUB_${_lib}})
    else(LIB_SUB_${_lib})
      set(CUDA_LIBRARY_FAILED "YES")
    endif(LIB_SUB_${_lib}) 
  endforeach(_lib)

  find_path(CUDA_INCLUDE_DIR nvToolsExt.h
  PATHS 
  ${CUDA_INCLUDE_PATH}
  NO_DEFAULT_PATH)

ENDIF(ENABLE_NVTX)

find_path(CUDA_INCLUDE_DIR cusparse_v2.h
 PATHS 
 ${CUDA_INCLUDE_PATH}
 NO_DEFAULT_PATH)
 
find_path(CUDA_SDK_INCLUDE_DIR helper_cuda.h
 PATHS 
 ${CUDA_INCLUDE_PATH}/../samples/common/inc
 NO_DEFAULT_PATH)
 
set(CUDA_FOUND "NO")
if(CUDA_LIBRARY)
  set(CUDA_FOUND "YES")
  set(CUDA_LIBRARIES ${CUDA_LIBRARY})
  set(CUDA_INCLUDE_DIRS ${CUDA_INCLUDE_DIR})
  set(CUDA_SDK_INCLUDE_DIRS  ${CUDA_SDK_INCLUDE_DIR})
endif(CUDA_LIBRARY)
endif(ENABLE_CUDA)

