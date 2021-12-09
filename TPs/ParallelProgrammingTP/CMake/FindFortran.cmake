#
# Find Fortran includes
#
# This module defines
# FORTRAN_LIBRARIES, the libraries to link against to use Fortran.
# FORTRAN_FOUND If false, do not try to use Fortran.
IF(NOT WIN32)
foreach(FORTRAN_LIB ifcore imf svml irc)
  find_library(FORTRAN_SUB_${FORTRAN_LIB} ${FORTRAN_LIB}
    PATHS ${FORTRAN_LIBRARY_PATH} NO_DEFAULT_PATH)
  if(FORTRAN_SUB_${FORTRAN_LIB})
    set(FORTRAN_LIBRARY ${FORTRAN_LIBRARY} ${FORTRAN_SUB_${FORTRAN_LIB}})
  else(FORTRAN_SUB_${FORTRAN_LIB})
    set(FORTRAN_LIBRARY_FAILED "YES")
  endif(FORTRAN_SUB_${FORTRAN_LIB})
endforeach(FORTRAN_LIB)
else(NOT WIN32)
foreach(FORTRAN_LIB libifcoremt.lib libircmt.lib)
  find_library(FORTRAN_SUB_${FORTRAN_LIB} ${FORTRAN_LIB}
    PATHS ${FORTRAN_LIBRARY_PATH} NO_DEFAULT_PATH)
  if(FORTRAN_SUB_${FORTRAN_LIB})
    set(FORTRAN_LIBRARY ${FORTRAN_LIBRARY} ${FORTRAN_SUB_${FORTRAN_LIB}})
  else(FORTRAN_SUB_${FORTRAN_LIB})
    set(FORTRAN_LIBRARY_FAILED "YES")
  endif(FORTRAN_SUB_${FORTRAN_LIB})
endforeach(FORTRAN_LIB)
endif(NOT WIN32)
set(FORTRAN_FOUND "NO")
if(NOT FORTRAN_LIBRARY_FAILED)
  set(FORTRAN_FOUND "YES")
  set(FORTRAN_LIBRARIES ${FORTRAN_LIBRARY})
  set(FORTRAN_FLAGS "_F2C")
endif(NOT FORTRAN_LIBRARY_FAILED)
