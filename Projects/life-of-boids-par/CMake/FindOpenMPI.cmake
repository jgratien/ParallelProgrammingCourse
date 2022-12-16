

FIND_PROGRAM(MPI_EXEC_NAME mpiexec
  PATHS
 $ENV{MPI_ROOT}
 PATH_SUFFIXES
    bin #   bin64
  NO_DEFAULT_PATH
)

FIND_PATH(MPI_INCLUDE_DIR mpi.h
  PATHS
  $ENV{MPI_ROOT}
  PATH_SUFFIXES
   include #include64
  NO_DEFAULT_PATH
)

foreach(_lib mpi mpi_cxx)
  find_library(LIB_SUB_${_lib} ${_lib}
  PATHS
  $ENV{MPI_ROOT}
  PATH_SUFFIXES
    lib
  NO_DEFAULT_PATH
)
  if(LIB_SUB_${_lib})
    set(MPI_LIBRARY ${MPI_LIBRARY} ${LIB_SUB_${_lib}})
  else(LIB_SUB_${_lib})
    set(MPI_LIBRARY_FAILED "YES")
  endif(LIB_SUB_${_lib})

endforeach(_lib)


SET( MPI_FOUND "NO" )
IF(MPI_INCLUDE_DIR)
  INCLUDE_DIRECTORIES(${MPI_INCLUDE_DIR})
  IF(MPI_LIBRARY)
    IF(MPI_EXEC_NAME)
      SET(MPI_FOUND "YES")
      SET(MPI_LIBRARIES ${MPI_LIBRARY} rt dl) # mpiif dl)
    ENDIF(MPI_EXEC_NAME)
  ENDIF(MPI_LIBRARY)
ENDIF(MPI_INCLUDE_DIR)
