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
CMAKE_SOURCE_DIR = /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build

# Include any dependencies generated for this target.
include src/life-of-boids-par/CMakeFiles/life-of-boids.dir/depend.make

# Include the progress variables for this target.
include src/life-of-boids-par/CMakeFiles/life-of-boids.dir/progress.make

# Include the compile flags for this target's objects.
include src/life-of-boids-par/CMakeFiles/life-of-boids.dir/flags.make

src/life-of-boids-par/CMakeFiles/life-of-boids.dir/main.cpp.o: src/life-of-boids-par/CMakeFiles/life-of-boids.dir/flags.make
src/life-of-boids-par/CMakeFiles/life-of-boids.dir/main.cpp.o: ../src/life-of-boids-par/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/life-of-boids-par/CMakeFiles/life-of-boids.dir/main.cpp.o"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par && /softs/gcc/7.3.0/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/life-of-boids.dir/main.cpp.o -c /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-par/main.cpp

src/life-of-boids-par/CMakeFiles/life-of-boids.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/life-of-boids.dir/main.cpp.i"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-par/main.cpp > CMakeFiles/life-of-boids.dir/main.cpp.i

src/life-of-boids-par/CMakeFiles/life-of-boids.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/life-of-boids.dir/main.cpp.s"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-par/main.cpp -o CMakeFiles/life-of-boids.dir/main.cpp.s

src/life-of-boids-par/CMakeFiles/life-of-boids.dir/coupled.cpp.o: src/life-of-boids-par/CMakeFiles/life-of-boids.dir/flags.make
src/life-of-boids-par/CMakeFiles/life-of-boids.dir/coupled.cpp.o: ../src/life-of-boids-par/coupled.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/life-of-boids-par/CMakeFiles/life-of-boids.dir/coupled.cpp.o"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par && /softs/gcc/7.3.0/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/life-of-boids.dir/coupled.cpp.o -c /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-par/coupled.cpp

src/life-of-boids-par/CMakeFiles/life-of-boids.dir/coupled.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/life-of-boids.dir/coupled.cpp.i"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-par/coupled.cpp > CMakeFiles/life-of-boids.dir/coupled.cpp.i

src/life-of-boids-par/CMakeFiles/life-of-boids.dir/coupled.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/life-of-boids.dir/coupled.cpp.s"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-par/coupled.cpp -o CMakeFiles/life-of-boids.dir/coupled.cpp.s

# Object files for target life-of-boids
life__of__boids_OBJECTS = \
"CMakeFiles/life-of-boids.dir/main.cpp.o" \
"CMakeFiles/life-of-boids.dir/coupled.cpp.o"

# External object files for target life-of-boids
life__of__boids_EXTERNAL_OBJECTS =

src/life-of-boids-par/life-of-boids: src/life-of-boids-par/CMakeFiles/life-of-boids.dir/main.cpp.o
src/life-of-boids-par/life-of-boids: src/life-of-boids-par/CMakeFiles/life-of-boids.dir/coupled.cpp.o
src/life-of-boids-par/life-of-boids: src/life-of-boids-par/CMakeFiles/life-of-boids.dir/build.make
src/life-of-boids-par/life-of-boids: src/life-of-boids-par/movement/libmovement.a
src/life-of-boids-par/life-of-boids: src/life-of-boids-par/config/libconfig.a
src/life-of-boids-par/life-of-boids: /softs/boost/1.63.0/lib/libboost_thread.a
src/life-of-boids-par/life-of-boids: /softs/boost/1.63.0/lib/libboost_serialization.a
src/life-of-boids-par/life-of-boids: /softs/boost/1.63.0/lib/libboost_chrono.a
src/life-of-boids-par/life-of-boids: /softs/boost/1.63.0/lib/libboost_system.a
src/life-of-boids-par/life-of-boids: /softs/boost/1.63.0/lib/libboost_program_options.a
src/life-of-boids-par/life-of-boids: /softs/boost/1.63.0/lib/libboost_regex.a
src/life-of-boids-par/life-of-boids: /softs/boost/1.63.0/lib/libboost_filesystem.a
src/life-of-boids-par/life-of-boids: /softs/intel/tbb/lib/intel64/gcc4.4/libtbb.so
src/life-of-boids-par/life-of-boids: /softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib/libmpi.so
src/life-of-boids-par/life-of-boids: src/life-of-boids-par/movement/obstacles/libobstacles.a
src/life-of-boids-par/life-of-boids: src/life-of-boids-par/movement/agents/libagents.a
src/life-of-boids-par/life-of-boids: src/life-of-boids-par/movement/fruits/libfruits.a
src/life-of-boids-par/life-of-boids: src/life-of-boids-par/CMakeFiles/life-of-boids.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable life-of-boids"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/life-of-boids.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/life-of-boids-par/CMakeFiles/life-of-boids.dir/build: src/life-of-boids-par/life-of-boids

.PHONY : src/life-of-boids-par/CMakeFiles/life-of-boids.dir/build

src/life-of-boids-par/CMakeFiles/life-of-boids.dir/clean:
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par && $(CMAKE_COMMAND) -P CMakeFiles/life-of-boids.dir/cmake_clean.cmake
.PHONY : src/life-of-boids-par/CMakeFiles/life-of-boids.dir/clean

src/life-of-boids-par/CMakeFiles/life-of-boids.dir/depend:
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-par /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-par/CMakeFiles/life-of-boids.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/life-of-boids-par/CMakeFiles/life-of-boids.dir/depend

