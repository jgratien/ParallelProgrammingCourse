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
include src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/depend.make

# Include the progress variables for this target.
include src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/progress.make

# Include the compile flags for this target's objects.
include src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/flags.make

src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/helloworld_stdthread.cpp.o: src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/flags.make
src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/helloworld_stdthread.cpp.o: ../src/TP1/helloworld_stdthread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/maxime.naillon/Project/CPU/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/helloworld_stdthread.cpp.o"
	cd /gext/maxime.naillon/Project/CPU/build/src/TP1 && /softs/gcc/7.3.0/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/helloworld_stdthread.exe.dir/helloworld_stdthread.cpp.o -c /gext/maxime.naillon/Project/CPU/src/TP1/helloworld_stdthread.cpp

src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/helloworld_stdthread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/helloworld_stdthread.exe.dir/helloworld_stdthread.cpp.i"
	cd /gext/maxime.naillon/Project/CPU/build/src/TP1 && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/maxime.naillon/Project/CPU/src/TP1/helloworld_stdthread.cpp > CMakeFiles/helloworld_stdthread.exe.dir/helloworld_stdthread.cpp.i

src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/helloworld_stdthread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/helloworld_stdthread.exe.dir/helloworld_stdthread.cpp.s"
	cd /gext/maxime.naillon/Project/CPU/build/src/TP1 && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/maxime.naillon/Project/CPU/src/TP1/helloworld_stdthread.cpp -o CMakeFiles/helloworld_stdthread.exe.dir/helloworld_stdthread.cpp.s

# Object files for target helloworld_stdthread.exe
helloworld_stdthread_exe_OBJECTS = \
"CMakeFiles/helloworld_stdthread.exe.dir/helloworld_stdthread.cpp.o"

# External object files for target helloworld_stdthread.exe
helloworld_stdthread_exe_EXTERNAL_OBJECTS =

src/TP1/helloworld_stdthread.exe: src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/helloworld_stdthread.cpp.o
src/TP1/helloworld_stdthread.exe: src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/build.make
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_thread.a
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_serialization.a
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_chrono.a
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_system.a
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_program_options.a
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_regex.a
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_filesystem.a
src/TP1/helloworld_stdthread.exe: /softs/intel/tbb/lib/intel64/gcc4.4/libtbb.so
src/TP1/helloworld_stdthread.exe: /softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib/libmpi.so
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_thread.a
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_serialization.a
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_chrono.a
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_system.a
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_program_options.a
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_regex.a
src/TP1/helloworld_stdthread.exe: /softs/boost/1.63.0/lib/libboost_filesystem.a
src/TP1/helloworld_stdthread.exe: /softs/intel/tbb/lib/intel64/gcc4.4/libtbb.so
src/TP1/helloworld_stdthread.exe: /softs/opencv/4.6.0/lib64/libopencv_highgui.so.4.6.0
src/TP1/helloworld_stdthread.exe: /softs/mpi/openmpi-4.0.1/gcc-7.3.0/lib/libmpi.so
src/TP1/helloworld_stdthread.exe: /softs/opencv/4.6.0/lib64/libopencv_videoio.so.4.6.0
src/TP1/helloworld_stdthread.exe: /softs/opencv/4.6.0/lib64/libopencv_imgcodecs.so.4.6.0
src/TP1/helloworld_stdthread.exe: /softs/opencv/4.6.0/lib64/libopencv_imgproc.so.4.6.0
src/TP1/helloworld_stdthread.exe: /softs/opencv/4.6.0/lib64/libopencv_core.so.4.6.0
src/TP1/helloworld_stdthread.exe: src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/gext/maxime.naillon/Project/CPU/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable helloworld_stdthread.exe"
	cd /gext/maxime.naillon/Project/CPU/build/src/TP1 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/helloworld_stdthread.exe.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/build: src/TP1/helloworld_stdthread.exe

.PHONY : src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/build

src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/clean:
	cd /gext/maxime.naillon/Project/CPU/build/src/TP1 && $(CMAKE_COMMAND) -P CMakeFiles/helloworld_stdthread.exe.dir/cmake_clean.cmake
.PHONY : src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/clean

src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/depend:
	cd /gext/maxime.naillon/Project/CPU/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /gext/maxime.naillon/Project/CPU /gext/maxime.naillon/Project/CPU/src/TP1 /gext/maxime.naillon/Project/CPU/build /gext/maxime.naillon/Project/CPU/build/src/TP1 /gext/maxime.naillon/Project/CPU/build/src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/TP1/CMakeFiles/helloworld_stdthread.exe.dir/depend

