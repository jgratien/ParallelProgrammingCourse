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
CMAKE_SOURCE_DIR = /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build

# Include any dependencies generated for this target.
include src/TP4/CMakeFiles/img_tbb.exe.dir/depend.make

# Include the progress variables for this target.
include src/TP4/CMakeFiles/img_tbb.exe.dir/progress.make

# Include the compile flags for this target's objects.
include src/TP4/CMakeFiles/img_tbb.exe.dir/flags.make

src/TP4/CMakeFiles/img_tbb.exe.dir/img_tbb.cpp.o: src/TP4/CMakeFiles/img_tbb.exe.dir/flags.make
src/TP4/CMakeFiles/img_tbb.exe.dir/img_tbb.cpp.o: ../src/TP4/img_tbb.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/TP4/CMakeFiles/img_tbb.exe.dir/img_tbb.cpp.o"
	cd /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4 && /softs/gcc/7.3.0/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/img_tbb.exe.dir/img_tbb.cpp.o -c /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/TP4/img_tbb.cpp

src/TP4/CMakeFiles/img_tbb.exe.dir/img_tbb.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/img_tbb.exe.dir/img_tbb.cpp.i"
	cd /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4 && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/TP4/img_tbb.cpp > CMakeFiles/img_tbb.exe.dir/img_tbb.cpp.i

src/TP4/CMakeFiles/img_tbb.exe.dir/img_tbb.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/img_tbb.exe.dir/img_tbb.cpp.s"
	cd /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4 && /softs/gcc/7.3.0/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/TP4/img_tbb.cpp -o CMakeFiles/img_tbb.exe.dir/img_tbb.cpp.s

# Object files for target img_tbb.exe
img_tbb_exe_OBJECTS = \
"CMakeFiles/img_tbb.exe.dir/img_tbb.cpp.o"

# External object files for target img_tbb.exe
img_tbb_exe_EXTERNAL_OBJECTS =

src/TP4/img_tbb.exe: src/TP4/CMakeFiles/img_tbb.exe.dir/img_tbb.cpp.o
src/TP4/img_tbb.exe: src/TP4/CMakeFiles/img_tbb.exe.dir/build.make
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_thread.a
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_serialization.a
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_chrono.a
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_system.a
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_program_options.a
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_regex.a
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_filesystem.a
src/TP4/img_tbb.exe: /softs/intel/tbb/lib/intel64/gcc4.4/libtbb.so
src/TP4/img_tbb.exe: /softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib/libmpi.so
src/TP4/img_tbb.exe: /softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib/libmpi_cxx.so
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_thread.a
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_serialization.a
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_chrono.a
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_system.a
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_program_options.a
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_regex.a
src/TP4/img_tbb.exe: /softs/boost/1.63.0/lib/libboost_filesystem.a
src/TP4/img_tbb.exe: /softs/intel/tbb/lib/intel64/gcc4.4/libtbb.so
src/TP4/img_tbb.exe: /gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64/libopencv_highgui.so.3.4.7
src/TP4/img_tbb.exe: /softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib/libmpi.so
src/TP4/img_tbb.exe: /softs/mpi/openmpi-1.10.7/gcc-7.3.0/lib/libmpi_cxx.so
src/TP4/img_tbb.exe: /gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64/libopencv_videoio.so.3.4.7
src/TP4/img_tbb.exe: /gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64/libopencv_imgcodecs.so.3.4.7
src/TP4/img_tbb.exe: /gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64/libopencv_imgproc.so.3.4.7
src/TP4/img_tbb.exe: /gext/jean-marc.gratien/ModuleHPC2020/local/opencv/lib64/libopencv_core.so.3.4.7
src/TP4/img_tbb.exe: src/TP4/CMakeFiles/img_tbb.exe.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable img_tbb.exe"
	cd /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/img_tbb.exe.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/TP4/CMakeFiles/img_tbb.exe.dir/build: src/TP4/img_tbb.exe

.PHONY : src/TP4/CMakeFiles/img_tbb.exe.dir/build

src/TP4/CMakeFiles/img_tbb.exe.dir/clean:
	cd /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4 && $(CMAKE_COMMAND) -P CMakeFiles/img_tbb.exe.dir/cmake_clean.cmake
.PHONY : src/TP4/CMakeFiles/img_tbb.exe.dir/clean

src/TP4/CMakeFiles/img_tbb.exe.dir/depend:
	cd /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/src/TP4 /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4 /gext/fadi.bechara/build/ParallelProgrammingCourse/TPs/ParallelProgrammingTP/build/src/TP4/CMakeFiles/img_tbb.exe.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/TP4/CMakeFiles/img_tbb.exe.dir/depend

