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
include src/movement/fruits/CMakeFiles/fruits.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/movement/fruits/CMakeFiles/fruits.dir/compiler_depend.make

# Include the progress variables for this target.
include src/movement/fruits/CMakeFiles/fruits.dir/progress.make

# Include the compile flags for this target's objects.
include src/movement/fruits/CMakeFiles/fruits.dir/flags.make

src/movement/fruits/CMakeFiles/fruits.dir/eco.cpp.o: src/movement/fruits/CMakeFiles/fruits.dir/flags.make
src/movement/fruits/CMakeFiles/fruits.dir/eco.cpp.o: /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits/eco.cpp
src/movement/fruits/CMakeFiles/fruits.dir/eco.cpp.o: src/movement/fruits/CMakeFiles/fruits.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/movement/fruits/CMakeFiles/fruits.dir/eco.cpp.o"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/movement/fruits/CMakeFiles/fruits.dir/eco.cpp.o -MF CMakeFiles/fruits.dir/eco.cpp.o.d -o CMakeFiles/fruits.dir/eco.cpp.o -c /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits/eco.cpp

src/movement/fruits/CMakeFiles/fruits.dir/eco.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fruits.dir/eco.cpp.i"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits/eco.cpp > CMakeFiles/fruits.dir/eco.cpp.i

src/movement/fruits/CMakeFiles/fruits.dir/eco.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fruits.dir/eco.cpp.s"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits/eco.cpp -o CMakeFiles/fruits.dir/eco.cpp.s

src/movement/fruits/CMakeFiles/fruits.dir/fruit.cpp.o: src/movement/fruits/CMakeFiles/fruits.dir/flags.make
src/movement/fruits/CMakeFiles/fruits.dir/fruit.cpp.o: /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits/fruit.cpp
src/movement/fruits/CMakeFiles/fruits.dir/fruit.cpp.o: src/movement/fruits/CMakeFiles/fruits.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/movement/fruits/CMakeFiles/fruits.dir/fruit.cpp.o"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/movement/fruits/CMakeFiles/fruits.dir/fruit.cpp.o -MF CMakeFiles/fruits.dir/fruit.cpp.o.d -o CMakeFiles/fruits.dir/fruit.cpp.o -c /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits/fruit.cpp

src/movement/fruits/CMakeFiles/fruits.dir/fruit.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fruits.dir/fruit.cpp.i"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits/fruit.cpp > CMakeFiles/fruits.dir/fruit.cpp.i

src/movement/fruits/CMakeFiles/fruits.dir/fruit.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fruits.dir/fruit.cpp.s"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits/fruit.cpp -o CMakeFiles/fruits.dir/fruit.cpp.s

src/movement/fruits/CMakeFiles/fruits.dir/tree.cpp.o: src/movement/fruits/CMakeFiles/fruits.dir/flags.make
src/movement/fruits/CMakeFiles/fruits.dir/tree.cpp.o: /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits/tree.cpp
src/movement/fruits/CMakeFiles/fruits.dir/tree.cpp.o: src/movement/fruits/CMakeFiles/fruits.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/movement/fruits/CMakeFiles/fruits.dir/tree.cpp.o"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/movement/fruits/CMakeFiles/fruits.dir/tree.cpp.o -MF CMakeFiles/fruits.dir/tree.cpp.o.d -o CMakeFiles/fruits.dir/tree.cpp.o -c /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits/tree.cpp

src/movement/fruits/CMakeFiles/fruits.dir/tree.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fruits.dir/tree.cpp.i"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits/tree.cpp > CMakeFiles/fruits.dir/tree.cpp.i

src/movement/fruits/CMakeFiles/fruits.dir/tree.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fruits.dir/tree.cpp.s"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits/tree.cpp -o CMakeFiles/fruits.dir/tree.cpp.s

# Object files for target fruits
fruits_OBJECTS = \
"CMakeFiles/fruits.dir/eco.cpp.o" \
"CMakeFiles/fruits.dir/fruit.cpp.o" \
"CMakeFiles/fruits.dir/tree.cpp.o"

# External object files for target fruits
fruits_EXTERNAL_OBJECTS =

src/movement/fruits/libfruits.a: src/movement/fruits/CMakeFiles/fruits.dir/eco.cpp.o
src/movement/fruits/libfruits.a: src/movement/fruits/CMakeFiles/fruits.dir/fruit.cpp.o
src/movement/fruits/libfruits.a: src/movement/fruits/CMakeFiles/fruits.dir/tree.cpp.o
src/movement/fruits/libfruits.a: src/movement/fruits/CMakeFiles/fruits.dir/build.make
src/movement/fruits/libfruits.a: src/movement/fruits/CMakeFiles/fruits.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library libfruits.a"
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits && $(CMAKE_COMMAND) -P CMakeFiles/fruits.dir/cmake_clean_target.cmake
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fruits.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/movement/fruits/CMakeFiles/fruits.dir/build: src/movement/fruits/libfruits.a
.PHONY : src/movement/fruits/CMakeFiles/fruits.dir/build

src/movement/fruits/CMakeFiles/fruits.dir/clean:
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits && $(CMAKE_COMMAND) -P CMakeFiles/fruits.dir/cmake_clean.cmake
.PHONY : src/movement/fruits/CMakeFiles/fruits.dir/clean

src/movement/fruits/CMakeFiles/fruits.dir/depend:
	cd /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/src/movement/fruits /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits /gext/abhishek.purandare/ue3/ParallelProgrammingCourse/Projects/life-of-boids-par/build/src/movement/fruits/CMakeFiles/fruits.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/movement/fruits/CMakeFiles/fruits.dir/depend

