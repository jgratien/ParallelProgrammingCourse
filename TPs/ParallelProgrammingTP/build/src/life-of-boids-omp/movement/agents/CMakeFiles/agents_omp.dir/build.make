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
include src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/depend.make

# Include the progress variables for this target.
include src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/progress.make

# Include the compile flags for this target's objects.
include src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/flags.make

src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/agent.cpp.o: src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/flags.make
src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/agent.cpp.o: ../src/life-of-boids-omp/movement/agents/agent.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/agent.cpp.o"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents && /softs/gcc/7.3.0/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/agents_omp.dir/agent.cpp.o -c /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-omp/movement/agents/agent.cpp

src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/agent.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/agents_omp.dir/agent.cpp.i"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-omp/movement/agents/agent.cpp > CMakeFiles/agents_omp.dir/agent.cpp.i

src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/agent.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/agents_omp.dir/agent.cpp.s"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-omp/movement/agents/agent.cpp -o CMakeFiles/agents_omp.dir/agent.cpp.s

src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/bird.cpp.o: src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/flags.make
src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/bird.cpp.o: ../src/life-of-boids-omp/movement/agents/bird.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/bird.cpp.o"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents && /softs/gcc/7.3.0/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/agents_omp.dir/bird.cpp.o -c /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-omp/movement/agents/bird.cpp

src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/bird.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/agents_omp.dir/bird.cpp.i"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-omp/movement/agents/bird.cpp > CMakeFiles/agents_omp.dir/bird.cpp.i

src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/bird.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/agents_omp.dir/bird.cpp.s"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-omp/movement/agents/bird.cpp -o CMakeFiles/agents_omp.dir/bird.cpp.s

src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/predator.cpp.o: src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/flags.make
src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/predator.cpp.o: ../src/life-of-boids-omp/movement/agents/predator.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/predator.cpp.o"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents && /softs/gcc/7.3.0/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/agents_omp.dir/predator.cpp.o -c /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-omp/movement/agents/predator.cpp

src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/predator.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/agents_omp.dir/predator.cpp.i"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-omp/movement/agents/predator.cpp > CMakeFiles/agents_omp.dir/predator.cpp.i

src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/predator.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/agents_omp.dir/predator.cpp.s"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-omp/movement/agents/predator.cpp -o CMakeFiles/agents_omp.dir/predator.cpp.s

# Object files for target agents_omp
agents_omp_OBJECTS = \
"CMakeFiles/agents_omp.dir/agent.cpp.o" \
"CMakeFiles/agents_omp.dir/bird.cpp.o" \
"CMakeFiles/agents_omp.dir/predator.cpp.o"

# External object files for target agents_omp
agents_omp_EXTERNAL_OBJECTS =

src/life-of-boids-omp/movement/agents/libagents_omp.a: src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/agent.cpp.o
src/life-of-boids-omp/movement/agents/libagents_omp.a: src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/bird.cpp.o
src/life-of-boids-omp/movement/agents/libagents_omp.a: src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/predator.cpp.o
src/life-of-boids-omp/movement/agents/libagents_omp.a: src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/build.make
src/life-of-boids-omp/movement/agents/libagents_omp.a: src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library libagents_omp.a"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents && $(CMAKE_COMMAND) -P CMakeFiles/agents_omp.dir/cmake_clean_target.cmake
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/agents_omp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/build: src/life-of-boids-omp/movement/agents/libagents_omp.a

.PHONY : src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/build

src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/clean:
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents && $(CMAKE_COMMAND) -P CMakeFiles/agents_omp.dir/cmake_clean.cmake
.PHONY : src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/clean

src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/depend:
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/life-of-boids-omp/movement/agents /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/life-of-boids-omp/movement/agents/CMakeFiles/agents_omp.dir/depend

