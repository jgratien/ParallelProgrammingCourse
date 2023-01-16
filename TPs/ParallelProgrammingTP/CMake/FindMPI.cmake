#
# Find the MpiCh includes and library
#
# This module defines
# MPI_INCLUDE_DIR, where to find headers,
# MPI_LIBRARIES, the libraries to link against to use Mpi.
# MPI_FOUND, If false, do not try to use Mpi.

FIND_PROGRAM(MPI_EXEC_NAME mpiexec
  PATHS 
 $ENV{MPI_ROOT}
 PATH_SUFFIXES
   bin64
  NO_DEFAULT_PATH
)

FIND_PATH(MPI_INCLUDE_DIR mpi.h
  PATHS 
  $ENV{MPI_ROOT}
  PATH_SUFFIXES
  include include64
  NO_DEFAULT_PATH
)

FIND_LIBRARY(MPI_LIBRARY NAMES libmpi.so
  PATHS 
  $ENV{MPI_ROOT}
  PATH_SUFFIXES
  lib64 lib/x86_64-linux-gnu
  NO_DEFAULT_PATH
)

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
