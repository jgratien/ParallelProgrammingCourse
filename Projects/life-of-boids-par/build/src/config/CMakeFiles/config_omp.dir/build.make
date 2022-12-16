# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /softs/cmake/3.25.1/bin/cmake

# The command to remove a file.
RM = /softs/cmake/3.25.1/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build

# Include any dependencies generated for this target.
include src/config/CMakeFiles/config_omp.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/config/CMakeFiles/config_omp.dir/compiler_depend.make

# Include the progress variables for this target.
include src/config/CMakeFiles/config_omp.dir/progress.make

# Include the compile flags for this target's objects.
include src/config/CMakeFiles/config_omp.dir/flags.make

src/config/CMakeFiles/config_omp.dir/config.cpp.o: src/config/CMakeFiles/config_omp.dir/flags.make
src/config/CMakeFiles/config_omp.dir/config.cpp.o: /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/config/config.cpp
src/config/CMakeFiles/config_omp.dir/config.cpp.o: src/config/CMakeFiles/config_omp.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/config/CMakeFiles/config_omp.dir/config.cpp.o"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/config && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/config/CMakeFiles/config_omp.dir/config.cpp.o -MF CMakeFiles/config_omp.dir/config.cpp.o.d -o CMakeFiles/config_omp.dir/config.cpp.o -c /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/config/config.cpp

src/config/CMakeFiles/config_omp.dir/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/config_omp.dir/config.cpp.i"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/config && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/config/config.cpp > CMakeFiles/config_omp.dir/config.cpp.i

src/config/CMakeFiles/config_omp.dir/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/config_omp.dir/config.cpp.s"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/config && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/config/config.cpp -o CMakeFiles/config_omp.dir/config.cpp.s

# Object files for target config_omp
config_omp_OBJECTS = \
"CMakeFiles/config_omp.dir/config.cpp.o"

# External object files for target config_omp
config_omp_EXTERNAL_OBJECTS =

src/config/libconfig_omp.a: src/config/CMakeFiles/config_omp.dir/config.cpp.o
src/config/libconfig_omp.a: src/config/CMakeFiles/config_omp.dir/build.make
src/config/libconfig_omp.a: src/config/CMakeFiles/config_omp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libconfig_omp.a"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/config && $(CMAKE_COMMAND) -P CMakeFiles/config_omp.dir/cmake_clean_target.cmake
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/config && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/config_omp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/config/CMakeFiles/config_omp.dir/build: src/config/libconfig_omp.a
.PHONY : src/config/CMakeFiles/config_omp.dir/build

src/config/CMakeFiles/config_omp.dir/clean:
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/config && $(CMAKE_COMMAND) -P CMakeFiles/config_omp.dir/cmake_clean.cmake
.PHONY : src/config/CMakeFiles/config_omp.dir/clean

src/config/CMakeFiles/config_omp.dir/depend:
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/config /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/config /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/config/CMakeFiles/config_omp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/config/CMakeFiles/config_omp.dir/depend

