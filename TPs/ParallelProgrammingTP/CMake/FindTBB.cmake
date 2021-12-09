if(NOT TBB_ROOT)
  set(TBB_ROOT $ENV{TBB_ROOT})
endif()

if(TBB_ROOT)
  set(_TBB_SEARCH_OPTS NO_DEFAULT_PATH)
else()
  set(_TBB_SEARCH_OPTS)
endif()

if(NOT TBB_FOUND)

  find_library(TBB_LIBRARY_DEBUG
    NAMES tbb_debug
    HINTS ${TBB_ROOT} 
		PATH_SUFFIXES lib lib/intel64/gcc4.4 lib/intel64/cc4.1.0_libc2.4_kernel2.6.16.21
    ${_TBB_SEARCH_OPTS}
    )
  mark_as_advanced(TBB_LIBRARY_DEBUG)

  find_library(TBB_LIBRARY_RELEASE
    NAMES tbb
    HINTS ${TBB_ROOT} 
		PATH_SUFFIXES lib lib/intel64/gcc4.4 lib/intel64/cc4.1.0_libc2.4_kernel2.6.16.21
    ${_TBB_SEARCH_OPTS}
    )
  mark_as_advanced(TBB_LIBRARY_RELEASE)
  message(status "TBB LIB : ${TBB_LIBRARY_RELEASE}")

  find_library(TBB_MAIN_LIBRARY_DEBUG
    NAMES tbb_main
    HINTS ${TBB_ROOT} 
		PATH_SUFFIXES lib/Debug lib lib/intel64/gcc4.4
    ${_TBB_SEARCH_OPTS}
    )
  mark_as_advanced(TBB_MAIN_LIBRARY_DEBUG)

  find_path(TBB_INCLUDE_DIR tbb/tbb_thread.h
    HINTS ${TBB_ROOT} 
		PATH_SUFFIXES include
    ${_TBB_SEARCH_OPTS}
    )
  mark_as_advanced(TBB_INCLUDE_DIR)
  message(status "TBB INC : ${TBB_INCLUDE_DIR}")
  
  if(NOT TBB_LIBRARY_RELEASE)
    find_path(TBB_LIB_DIR libtbb.so
              HINTS ${TBB_ROOT} 
              PATH_SUFFIXES lib lib/intel64/gcc4.4
              ${_TBB_SEARCH_OPTS}
              )
    if(TBB_LIB_DIR)
        set(TBB_LIBRARY_RELEASE ${TBB_LIB_DIR}/libtbb.so)
        set(TBB_LIBRARY_DEBUG ${TBB_LIB_DIR}/libtbb_debug.so)
        set(TBB_FOUND TRUE)
    endif(TBB_LIB_DIR)
   endif(NOT TBB_LIBRARY_RELEASE)
endif()
# pour limiter le mode verbose
set(TBB_FIND_QUIETLY ON)

#find_package_handle_standard_args(TBB
#	DEFAULT_MSG 
#	TBB_INCLUDE_DIR 
#	TBB_LIBRARY_RELEASE 
#	TBB_LIBRARY_DEBUG)

if(TBB_FOUND AND NOT TARGET tbb)
  
  set(TBB_INCLUDE_DIRS ${TBB_INCLUDE_DIR})
    
  set(TBB_LIBRARIES optimized ${TBB_LIBRARY_RELEASE}
                    debug     ${TBB_LIBRARY_DEBUG})

  add_library(tbb UNKNOWN IMPORTED)
	  
	set_target_properties(tbb PROPERTIES 
	  INTERFACE_INCLUDE_DIRECTORIES "${TBB_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "$<$<CONFIG:debug>:${TBB_LIBRARY_DEBUG}>$<$<CONFIG:release>:${TBB_LIBRARY_RELEASE}>")
    
  set_target_properties(tbb PROPERTIES
    IMPORTED_CONFIGURATIONS RELEASE
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
    IMPORTED_LOCATION_RELEASE "${TBB_LIBRARY_RELEASE}")
    
	set_target_properties(tbb PROPERTIES
    IMPORTED_CONFIGURATIONS DEBUG
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
    IMPORTED_LOCATION_DEBUG "${TBB_LIBRARY_DEBUG}")
    
endif()
