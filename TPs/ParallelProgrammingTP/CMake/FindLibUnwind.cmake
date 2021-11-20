#
# Find the libunwind includes and library
#
# This module defines
# LIBUNWIND_INCLUDE_DIRS, where to find headers,
# LIBUNWIND_LIBRARIES, the libraries to link against to use libunwind.
# LIBUNWIND_FOUND, If false, do not try to use libunwind.
 
find_path(LIBUNWIND_INCLUDE_DIR libunwind.h)
 
find_library(LIBUNWIND_LIBRARY unwind-generic)

set(LIBUNWIND_FOUND "NO")
if(LIBUNWIND_INCLUDE_DIR AND LIBUNWIND_LIBRARY)
  set(LIBUNWIND_FOUND "YES")
  set(LIBUNWIND_LIBRARIES ${LIBUNWIND_LIBRARY})
  set(LIBUNWIND_INCLUDE_DIRS ${LIBUNWIND_INCLUDE_DIR})
endif(LIBUNWIND_INCLUDE_DIR AND LIBUNWIND_LIBRARY)
