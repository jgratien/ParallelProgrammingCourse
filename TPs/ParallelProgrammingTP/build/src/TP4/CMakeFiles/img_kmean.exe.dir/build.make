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
CMAKE_SOURCE_DIR = /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build

# Include any dependencies generated for this target.
include src/TP4/CMakeFiles/img_kmean.exe.dir/depend.make

# Include the progress variables for this target.
include src/TP4/CMakeFiles/img_kmean.exe.dir/progress.make

# Include the compile flags for this target's objects.
include src/TP4/CMakeFiles/img_kmean.exe.dir/flags.make

src/TP4/CMakeFiles/img_kmean.exe.dir/img_kmean.cpp.o: src/TP4/CMakeFiles/img_kmean.exe.dir/flags.make
src/TP4/CMakeFiles/img_kmean.exe.dir/img_kmean.cpp.o: ../src/TP4/img_kmean.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/TP4/CMakeFiles/img_kmean.exe.dir/img_kmean.cpp.o"
	cd /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4 && /softs/gcc/7.3.0/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/img_kmean.exe.dir/img_kmean.cpp.o -c /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/TP4/img_kmean.cpp

src/TP4/CMakeFiles/img_kmean.exe.dir/img_kmean.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/img_kmean.exe.dir/img_kmean.cpp.i"
	cd /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4 && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/TP4/img_kmean.cpp > CMakeFiles/img_kmean.exe.dir/img_kmean.cpp.i

src/TP4/CMakeFiles/img_kmean.exe.dir/img_kmean.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/img_kmean.exe.dir/img_kmean.cpp.s"
	cd /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4 && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/TP4/img_kmean.cpp -o CMakeFiles/img_kmean.exe.dir/img_kmean.cpp.s

# Object files for target img_kmean.exe
img_kmean_exe_OBJECTS = \
"CMakeFiles/img_kmean.exe.dir/img_kmean.cpp.o"

# External object files for target img_kmean.exe
img_kmean_exe_EXTERNAL_OBJECTS =

src/TP4/img_kmean.exe: src/TP4/CMakeFiles/img_kmean.exe.dir/img_kmean.cpp.o
src/TP4/img_kmean.exe: src/TP4/CMakeFiles/img_kmean.exe.dir/build.make
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_thread.a
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_serialization.a
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_chrono.a
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_system.a
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_program_options.a
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_regex.a
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_filesystem.a
src/TP4/img_kmean.exe: /softs/intel/tbb/lib/intel64/gcc4.4/libtbb.so
src/TP4/img_kmean.exe: /softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib/libmpi.so
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_thread.a
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_serialization.a
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_chrono.a
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_system.a
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_program_options.a
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_regex.a
src/TP4/img_kmean.exe: /softs/boost/1.63.0/lib/libboost_filesystem.a
src/TP4/img_kmean.exe: /softs/intel/tbb/lib/intel64/gcc4.4/libtbb.so
src/TP4/img_kmean.exe: /softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib/libmpi.so
src/TP4/img_kmean.exe: src/TP4/CMakeFiles/img_kmean.exe.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable img_kmean.exe"
	cd /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/img_kmean.exe.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/TP4/CMakeFiles/img_kmean.exe.dir/build: src/TP4/img_kmean.exe

.PHONY : src/TP4/CMakeFiles/img_kmean.exe.dir/build

src/TP4/CMakeFiles/img_kmean.exe.dir/clean:
	cd /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4 && $(CMAKE_COMMAND) -P CMakeFiles/img_kmean.exe.dir/cmake_clean.cmake
.PHONY : src/TP4/CMakeFiles/img_kmean.exe.dir/clean

src/TP4/CMakeFiles/img_kmean.exe.dir/depend:
	cd /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/TP4 /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4 /gext/dimitrios.tsitsos/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4/CMakeFiles/img_kmean.exe.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/TP4/CMakeFiles/img_kmean.exe.dir/depend

