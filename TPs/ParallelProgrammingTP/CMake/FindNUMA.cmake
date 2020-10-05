#
# Find NUMA libraries
#
# This module defines
# NUMA_LIBRARIES, the libraries to link against to use numa.
# NUMA_FOUND If false, do not try to use numa.
 

if(ENABLE_NUMA) 
foreach(_lib numa)
  find_library(LIB_SUB_${_lib} ${_lib}
    PATHS ${NUMA_LIBRARY_PATH} NO_DEFAULT_PATH)
  if(LIB_SUB_${_lib})
    set(NUMA_LIBRARY ${NUMA_LIBRARY} ${LIB_SUB_${_lib}})
  else(LIB_SUB_${_lib})
    set(NUMA_LIBRARY_FAILED "YES")
  endif(LIB_SUB_${_lib})
endforeach(_lib)

find_path(NUMA_INCLUDE_DIR numa.h
 PATHS 
 ${NUMA_INCLUDE_PATH}
 NO_DEFAULT_PATH)

set(NUMA_FOUND "NO")
if(NUMA_LIBRARY)
  set(NUMA_FOUND "YES")
  set(NUMA_LIBRARIES ${NUMA_LIBRARY})
  set(NUMA_INCLUDE_DIRS ${NUMA_INCLUDE_DIR})
endif(NUMA_LIBRARY)
endif(ENABLE_NUMA)

