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
CMAKE_SOURCE_DIR = /gext/maxime.naillon/Project/CPU

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /gext/maxime.naillon/Project/CPU/build

# Include any dependencies generated for this target.
include src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/depend.make

# Include the progress variables for this target.
include src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/progress.make

# Include the compile flags for this target's objects.
include src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/flags.make

src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/agent.cpp.o: src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/flags.make
src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/agent.cpp.o: ../src/LiveOfBoids_OpenMP/movement/agents/agent.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/maxime.naillon/Project/CPU/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/agent.cpp.o"
	cd /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents && /softs/gcc/7.3.0/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/agents.dir/agent.cpp.o -c /gext/maxime.naillon/Project/CPU/src/LiveOfBoids_OpenMP/movement/agents/agent.cpp

src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/agent.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/agents.dir/agent.cpp.i"
	cd /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/maxime.naillon/Project/CPU/src/LiveOfBoids_OpenMP/movement/agents/agent.cpp > CMakeFiles/agents.dir/agent.cpp.i

src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/agent.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/agents.dir/agent.cpp.s"
	cd /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/maxime.naillon/Project/CPU/src/LiveOfBoids_OpenMP/movement/agents/agent.cpp -o CMakeFiles/agents.dir/agent.cpp.s

src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/bird.cpp.o: src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/flags.make
src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/bird.cpp.o: ../src/LiveOfBoids_OpenMP/movement/agents/bird.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/maxime.naillon/Project/CPU/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/bird.cpp.o"
	cd /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents && /softs/gcc/7.3.0/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/agents.dir/bird.cpp.o -c /gext/maxime.naillon/Project/CPU/src/LiveOfBoids_OpenMP/movement/agents/bird.cpp

src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/bird.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/agents.dir/bird.cpp.i"
	cd /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/maxime.naillon/Project/CPU/src/LiveOfBoids_OpenMP/movement/agents/bird.cpp > CMakeFiles/agents.dir/bird.cpp.i

src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/bird.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/agents.dir/bird.cpp.s"
	cd /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/maxime.naillon/Project/CPU/src/LiveOfBoids_OpenMP/movement/agents/bird.cpp -o CMakeFiles/agents.dir/bird.cpp.s

src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/predator.cpp.o: src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/flags.make
src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/predator.cpp.o: ../src/LiveOfBoids_OpenMP/movement/agents/predator.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/maxime.naillon/Project/CPU/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/predator.cpp.o"
	cd /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents && /softs/gcc/7.3.0/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/agents.dir/predator.cpp.o -c /gext/maxime.naillon/Project/CPU/src/LiveOfBoids_OpenMP/movement/agents/predator.cpp

src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/predator.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/agents.dir/predator.cpp.i"
	cd /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/maxime.naillon/Project/CPU/src/LiveOfBoids_OpenMP/movement/agents/predator.cpp > CMakeFiles/agents.dir/predator.cpp.i

src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/predator.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/agents.dir/predator.cpp.s"
	cd /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/maxime.naillon/Project/CPU/src/LiveOfBoids_OpenMP/movement/agents/predator.cpp -o CMakeFiles/agents.dir/predator.cpp.s

# Object files for target agents
agents_OBJECTS = \
"CMakeFiles/agents.dir/agent.cpp.o" \
"CMakeFiles/agents.dir/bird.cpp.o" \
"CMakeFiles/agents.dir/predator.cpp.o"

# External object files for target agents
agents_EXTERNAL_OBJECTS =

src/LiveOfBoids_OpenMP/movement/agents/libagents.a: src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/agent.cpp.o
src/LiveOfBoids_OpenMP/movement/agents/libagents.a: src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/bird.cpp.o
src/LiveOfBoids_OpenMP/movement/agents/libagents.a: src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/predator.cpp.o
src/LiveOfBoids_OpenMP/movement/agents/libagents.a: src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/build.make
src/LiveOfBoids_OpenMP/movement/agents/libagents.a: src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/gext/maxime.naillon/Project/CPU/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library libagents.a"
	cd /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents && $(CMAKE_COMMAND) -P CMakeFiles/agents.dir/cmake_clean_target.cmake
	cd /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/agents.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/build: src/LiveOfBoids_OpenMP/movement/agents/libagents.a

.PHONY : src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/build

src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/clean:
	cd /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents && $(CMAKE_COMMAND) -P CMakeFiles/agents.dir/cmake_clean.cmake
.PHONY : src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/clean

src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/depend:
	cd /gext/maxime.naillon/Project/CPU/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /gext/maxime.naillon/Project/CPU /gext/maxime.naillon/Project/CPU/src/LiveOfBoids_OpenMP/movement/agents /gext/maxime.naillon/Project/CPU/build /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents /gext/maxime.naillon/Project/CPU/build/src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/LiveOfBoids_OpenMP/movement/agents/CMakeFiles/agents.dir/depend

