# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /softs/cmake/3.16.5/bin/cmake

# The command to remove a file.
RM = /softs/cmake/3.16.5/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build

# Include any dependencies generated for this target.
include src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/depend.make

# Include the progress variables for this target.
include src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/progress.make

# Include the compile flags for this target's objects.
include src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/flags.make

src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/obstacle.cpp.o: src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/flags.make
src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/obstacle.cpp.o: ../src/software-engineering-project/movement/obstacles/obstacle.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/obstacle.cpp.o"
	cd /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/movement/obstacles && /softs/gcc/7.3.0/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/obstacles.dir/obstacle.cpp.o -c /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/software-engineering-project/movement/obstacles/obstacle.cpp

src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/obstacle.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/obstacles.dir/obstacle.cpp.i"
	cd /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/movement/obstacles && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/software-engineering-project/movement/obstacles/obstacle.cpp > CMakeFiles/obstacles.dir/obstacle.cpp.i

src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/obstacle.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/obstacles.dir/obstacle.cpp.s"
	cd /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/movement/obstacles && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/software-engineering-project/movement/obstacles/obstacle.cpp -o CMakeFiles/obstacles.dir/obstacle.cpp.s

# Object files for target obstacles
obstacles_OBJECTS = \
"CMakeFiles/obstacles.dir/obstacle.cpp.o"

# External object files for target obstacles
obstacles_EXTERNAL_OBJECTS =

src/software-engineering-project/movement/obstacles/libobstacles.a: src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/obstacle.cpp.o
src/software-engineering-project/movement/obstacles/libobstacles.a: src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/build.make
src/software-engineering-project/movement/obstacles/libobstacles.a: src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libobstacles.a"
	cd /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/movement/obstacles && $(CMAKE_COMMAND) -P CMakeFiles/obstacles.dir/cmake_clean_target.cmake
	cd /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/movement/obstacles && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/obstacles.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/build: src/software-engineering-project/movement/obstacles/libobstacles.a

.PHONY : src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/build

src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/clean:
	cd /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/movement/obstacles && $(CMAKE_COMMAND) -P CMakeFiles/obstacles.dir/cmake_clean.cmake
.PHONY : src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/clean

src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/depend:
	cd /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/software-engineering-project/movement/obstacles /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/movement/obstacles /gext/thomas.bancel/ue3/parallel_cpu/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/software-engineering-project/movement/obstacles/CMakeFiles/obstacles.dir/depend

