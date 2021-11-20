#
# Find Boost includes
#
# This module defines
# BOOST_INCLUDE_DIR, where to find headers,
# BOOST_FOUND

find_path(BOOST_INCLUDE_DIR boost
  PATHS ${BOOST_INCLUDE_PATH} NO_DEFAULT_PATH)

#if(NOT WIN32)
  foreach(_lib boost_mpi boost_thread boost_serialization boost_chrono boost_system boost_program_options boost_regex boost_filesystem)
  find_library(LIB_SUB_${_lib} ${_lib}
    PATHS ${BOOST_LIBRARY_PATH} NO_DEFAULT_PATH)
  if(LIB_SUB_${_lib})
    set(BOOST_LIBRARY ${BOOST_LIBRARY} ${LIB_SUB_${_lib}})
  else(LIB_SUB_${_lib})
    set(BOOST_LIBRARY_FAILED "YES")
  endif(LIB_SUB_${_lib})
  endforeach(_lib)
  set( BOOST_FOUND "NO" )
  set( BOOST_INCLUDE_DIRS ${BOOST_INCLUDE_DIR})
  if(BOOST_INCLUDE_DIR AND BOOST_LIBRARY)
    set( BOOST_FOUND "YES" )
    set( BOOST_INCLUDE_DIRS ${BOOST_INCLUDE_DIR})
    set( BOOST_LIBRARIES ${BOOST_LIBRARY})
  endif(BOOST_INCLUDE_DIR AND BOOST_LIBRARY)
#else(NOT WIN32)
  # Patch, seuls les includes de dispo aujourd'hui !
#  set( EXTRABOOST_FOUND "YES" )
#  set( EXTRABOOST_INCLUDE_DIRS ${BOOST_INCLUDE_DIR})
#endif(NOT WIN32)