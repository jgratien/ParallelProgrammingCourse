# Check if TBB_ROOT is set
if(NOT DEFINED ENV{TBB_ROOT})
    message(FATAL_ERROR "Environment variable TBB_ROOT is not set. Please set it to the TBB installation directory.")
endif()

# Define TBB paths
set(TBB_INCLUDE_DIR "$ENV{TBB_ROOT}/include")
set(TBB_LIBRARY_DIR "$ENV{TBB_ROOT}/lib")

# Check if paths exist
if(NOT EXISTS ${TBB_INCLUDE_DIR})
    message(FATAL_ERROR "TBB include directory not found at ${TBB_INCLUDE_DIR}")
endif()
if(NOT EXISTS ${TBB_LIBRARY_DIR})
    message(FATAL_ERROR "TBB library directory not found at ${TBB_LIBRARY_DIR}")
endif()

# Add TBB include and library paths
include_directories(${TBB_INCLUDE_DIR})
link_directories(${TBB_LIBRARY_DIR})

find_package(OpenMP REQUIRED)

# Manually specify the TBB library to link
set(TBB_LIBRARIES tbb tbbmalloc)

# Find all .cpp files in the source directory
file(GLOB_RECURSE ALL_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Exclude `main.cpp` from the list of source files
list(REMOVE_ITEM ALL_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# Create a library from the remaining .cpp files
add_library(my_libraries STATIC ${ALL_CPP_FILES})

# Include directories for the library
target_include_directories(my_libraries PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/laws
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/agents
    ${TBB_INCLUDE_DIR} # Add TBB include path
)

# Link TBB to the library
if(OpenMP_CXX_FOUND)
    target_link_libraries(my_libraries PUBLIC ${TBB_LIBRARIES} OpenMP::OpenMP_CXX)
endif()

# Define the executable
add_executable(freebirds main.cpp)

# Link the library to the executable
target_link_libraries(freebirds PUBLIC my_libraries)
