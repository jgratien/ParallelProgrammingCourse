#
# Find xml2 libraries
#
# This module defines
# XML2_LIBRARIES, 
# XML2_FOUND If false, do not try to use blas.
 
message(status "SEARCH XML2 PATHS : ${XML2_LIBRARY_PATH} ${XML2_INCLUDE_PATH}")
 
foreach(_lib xml2)
  find_library(LIB_SUB_${_lib} ${_lib}
    PATHS ${XML2_LIBRARY_PATH} NO_DEFAULT_PATH)
  message(status "FIND : ${LIB_SUB_${_lib}}")
  if(LIB_SUB_${_lib})
    set(XML2_LIBRARY ${XML2_LIBRARY} ${LIB_SUB_${_lib}})
  else(LIB_SUB_${_lib})
    set(XML2_LIBRARY_FAILED "YES")
  endif(LIB_SUB_${_lib})
endforeach(_lib)

find_path(XML2_INCLUDE_DIR libxml/xmlversion.h
 PATHS 
 ${XML2_INCLUDE_PATH}
 NO_DEFAULT_PATH)
message(status "LIBXML2 : ${XML2_LIBRARY}")
set(XML2_FOUND "NO")
if(XML2_LIBRARY)
  set(XML2_FOUND "YES")
  set(XML2_LIBRARIES ${XML2_LIBRARY})
  set(XML2_INCLUDE_DIRS ${XML2_INCLUDE_DIR})
  set(XML2_FLAGS "USE_XML2")
endif(XML2_LIBRARY)

