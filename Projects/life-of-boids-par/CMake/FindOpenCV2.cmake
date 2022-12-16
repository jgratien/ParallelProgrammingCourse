#
# Find OpenCV libraries
#
 
if(NOT OPENCV_ROOT)
  set(OPENCV_ROOT $ENV{OPENCV_ROOT})
endif()

message(status "OPENCV PATH : ${OPENCV_ROOT}")

find_package(OpenCV COMPONENTS opencv_core opencv_highgui QUIET)
#find_library(OPENCV_CORE_LIB opencv_core PATHS ${OPENCV_ROOT})
message(status "OPENCV CORE : ${OPENCV_CORE_LIB}")

set(OPENCV_FIND_QUIETLY ON)
find_package_handle_standard_args(OpenCV
	DEFAULT_MSG 
	OpenCV_FOUND 
	OpenCV_INCLUDE_DIR 
	OpenCV_CORE_LIBRARY
	OpenCV_HIGHGUI_LIBRARY)

set(OPENCV_FOUND "NO")
if(OPENCV_LIBRARY)
  set(OPENCV_FOUND "YES")
  set(OPENCV_LIBRARIES ${OPENCV_LIBRARY})
  set(OPENCV_INCLUDE_DIRS ${OPENCV_INCLUDE_DIR})
endif(OPENCV_LIBRARY)


