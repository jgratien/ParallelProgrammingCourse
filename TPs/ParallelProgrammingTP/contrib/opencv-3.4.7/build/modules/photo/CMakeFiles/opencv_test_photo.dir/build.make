# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

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
CMAKE_COMMAND = /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Compiler/GCCcore/7.3.0/CMake/3.11.4/bin/cmake

# The command to remove a file.
RM = /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Compiler/GCCcore/7.3.0/CMake/3.11.4/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build

# Include any dependencies generated for this target.
include modules/photo/CMakeFiles/opencv_test_photo.dir/depend.make

# Include the progress variables for this target.
include modules/photo/CMakeFiles/opencv_test_photo.dir/progress.make

# Include the compile flags for this target's objects.
include modules/photo/CMakeFiles/opencv_test_photo.dir/flags.make

modules/photo/CMakeFiles/opencv_test_photo.dir/test/ocl/test_denoising.cpp.o: modules/photo/CMakeFiles/opencv_test_photo.dir/flags.make
modules/photo/CMakeFiles/opencv_test_photo.dir/test/ocl/test_denoising.cpp.o: ../modules/photo/test/ocl/test_denoising.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object modules/photo/CMakeFiles/opencv_test_photo.dir/test/ocl/test_denoising.cpp.o"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -o CMakeFiles/opencv_test_photo.dir/test/ocl/test_denoising.cpp.o -c /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/ocl/test_denoising.cpp

modules/photo/CMakeFiles/opencv_test_photo.dir/test/ocl/test_denoising.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_photo.dir/test/ocl/test_denoising.cpp.i"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -E /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/ocl/test_denoising.cpp > CMakeFiles/opencv_test_photo.dir/test/ocl/test_denoising.cpp.i

modules/photo/CMakeFiles/opencv_test_photo.dir/test/ocl/test_denoising.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_photo.dir/test/ocl/test_denoising.cpp.s"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -S /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/ocl/test_denoising.cpp -o CMakeFiles/opencv_test_photo.dir/test/ocl/test_denoising.cpp.s

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_cloning.cpp.o: modules/photo/CMakeFiles/opencv_test_photo.dir/flags.make
modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_cloning.cpp.o: ../modules/photo/test/test_cloning.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_cloning.cpp.o"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -o CMakeFiles/opencv_test_photo.dir/test/test_cloning.cpp.o -c /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_cloning.cpp

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_cloning.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_photo.dir/test/test_cloning.cpp.i"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -E /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_cloning.cpp > CMakeFiles/opencv_test_photo.dir/test/test_cloning.cpp.i

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_cloning.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_photo.dir/test/test_cloning.cpp.s"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -S /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_cloning.cpp -o CMakeFiles/opencv_test_photo.dir/test/test_cloning.cpp.s

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_decolor.cpp.o: modules/photo/CMakeFiles/opencv_test_photo.dir/flags.make
modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_decolor.cpp.o: ../modules/photo/test/test_decolor.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_decolor.cpp.o"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -o CMakeFiles/opencv_test_photo.dir/test/test_decolor.cpp.o -c /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_decolor.cpp

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_decolor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_photo.dir/test/test_decolor.cpp.i"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -E /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_decolor.cpp > CMakeFiles/opencv_test_photo.dir/test/test_decolor.cpp.i

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_decolor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_photo.dir/test/test_decolor.cpp.s"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -S /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_decolor.cpp -o CMakeFiles/opencv_test_photo.dir/test/test_decolor.cpp.s

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoise_tvl1.cpp.o: modules/photo/CMakeFiles/opencv_test_photo.dir/flags.make
modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoise_tvl1.cpp.o: ../modules/photo/test/test_denoise_tvl1.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoise_tvl1.cpp.o"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -o CMakeFiles/opencv_test_photo.dir/test/test_denoise_tvl1.cpp.o -c /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_denoise_tvl1.cpp

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoise_tvl1.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_photo.dir/test/test_denoise_tvl1.cpp.i"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -E /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_denoise_tvl1.cpp > CMakeFiles/opencv_test_photo.dir/test/test_denoise_tvl1.cpp.i

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoise_tvl1.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_photo.dir/test/test_denoise_tvl1.cpp.s"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -S /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_denoise_tvl1.cpp -o CMakeFiles/opencv_test_photo.dir/test/test_denoise_tvl1.cpp.s

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoising.cpp.o: modules/photo/CMakeFiles/opencv_test_photo.dir/flags.make
modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoising.cpp.o: ../modules/photo/test/test_denoising.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoising.cpp.o"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -o CMakeFiles/opencv_test_photo.dir/test/test_denoising.cpp.o -c /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_denoising.cpp

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoising.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_photo.dir/test/test_denoising.cpp.i"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -E /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_denoising.cpp > CMakeFiles/opencv_test_photo.dir/test/test_denoising.cpp.i

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoising.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_photo.dir/test/test_denoising.cpp.s"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -S /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_denoising.cpp -o CMakeFiles/opencv_test_photo.dir/test/test_denoising.cpp.s

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoising.cuda.cpp.o: modules/photo/CMakeFiles/opencv_test_photo.dir/flags.make
modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoising.cuda.cpp.o: ../modules/photo/test/test_denoising.cuda.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoising.cuda.cpp.o"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -o CMakeFiles/opencv_test_photo.dir/test/test_denoising.cuda.cpp.o -c /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_denoising.cuda.cpp

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoising.cuda.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_photo.dir/test/test_denoising.cuda.cpp.i"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -E /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_denoising.cuda.cpp > CMakeFiles/opencv_test_photo.dir/test/test_denoising.cuda.cpp.i

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoising.cuda.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_photo.dir/test/test_denoising.cuda.cpp.s"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -S /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_denoising.cuda.cpp -o CMakeFiles/opencv_test_photo.dir/test/test_denoising.cuda.cpp.s

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_hdr.cpp.o: modules/photo/CMakeFiles/opencv_test_photo.dir/flags.make
modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_hdr.cpp.o: ../modules/photo/test/test_hdr.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_hdr.cpp.o"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -o CMakeFiles/opencv_test_photo.dir/test/test_hdr.cpp.o -c /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_hdr.cpp

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_hdr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_photo.dir/test/test_hdr.cpp.i"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -E /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_hdr.cpp > CMakeFiles/opencv_test_photo.dir/test/test_hdr.cpp.i

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_hdr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_photo.dir/test/test_hdr.cpp.s"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -S /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_hdr.cpp -o CMakeFiles/opencv_test_photo.dir/test/test_hdr.cpp.s

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_inpaint.cpp.o: modules/photo/CMakeFiles/opencv_test_photo.dir/flags.make
modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_inpaint.cpp.o: ../modules/photo/test/test_inpaint.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_inpaint.cpp.o"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -o CMakeFiles/opencv_test_photo.dir/test/test_inpaint.cpp.o -c /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_inpaint.cpp

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_inpaint.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_photo.dir/test/test_inpaint.cpp.i"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -E /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_inpaint.cpp > CMakeFiles/opencv_test_photo.dir/test/test_inpaint.cpp.i

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_inpaint.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_photo.dir/test/test_inpaint.cpp.s"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -S /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_inpaint.cpp -o CMakeFiles/opencv_test_photo.dir/test/test_inpaint.cpp.s

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_main.cpp.o: modules/photo/CMakeFiles/opencv_test_photo.dir/flags.make
modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_main.cpp.o: ../modules/photo/test/test_main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_main.cpp.o"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -o CMakeFiles/opencv_test_photo.dir/test/test_main.cpp.o -c /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_main.cpp

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_photo.dir/test/test_main.cpp.i"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -E /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_main.cpp > CMakeFiles/opencv_test_photo.dir/test/test_main.cpp.i

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_photo.dir/test/test_main.cpp.s"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -S /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_main.cpp -o CMakeFiles/opencv_test_photo.dir/test/test_main.cpp.s

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_npr.cpp.o: modules/photo/CMakeFiles/opencv_test_photo.dir/flags.make
modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_npr.cpp.o: ../modules/photo/test/test_npr.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_npr.cpp.o"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -o CMakeFiles/opencv_test_photo.dir/test/test_npr.cpp.o -c /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_npr.cpp

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_npr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/opencv_test_photo.dir/test/test_npr.cpp.i"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -E /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_npr.cpp > CMakeFiles/opencv_test_photo.dir/test/test_npr.cpp.i

modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_npr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/opencv_test_photo.dir/test/test_npr.cpp.s"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Core/GCCcore/7.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS)  -Winvalid-pch  -include "/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/test_precomp.hpp" -S /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo/test/test_npr.cpp -o CMakeFiles/opencv_test_photo.dir/test/test_npr.cpp.s

# Object files for target opencv_test_photo
opencv_test_photo_OBJECTS = \
"CMakeFiles/opencv_test_photo.dir/test/ocl/test_denoising.cpp.o" \
"CMakeFiles/opencv_test_photo.dir/test/test_cloning.cpp.o" \
"CMakeFiles/opencv_test_photo.dir/test/test_decolor.cpp.o" \
"CMakeFiles/opencv_test_photo.dir/test/test_denoise_tvl1.cpp.o" \
"CMakeFiles/opencv_test_photo.dir/test/test_denoising.cpp.o" \
"CMakeFiles/opencv_test_photo.dir/test/test_denoising.cuda.cpp.o" \
"CMakeFiles/opencv_test_photo.dir/test/test_hdr.cpp.o" \
"CMakeFiles/opencv_test_photo.dir/test/test_inpaint.cpp.o" \
"CMakeFiles/opencv_test_photo.dir/test/test_main.cpp.o" \
"CMakeFiles/opencv_test_photo.dir/test/test_npr.cpp.o"

# External object files for target opencv_test_photo
opencv_test_photo_EXTERNAL_OBJECTS =

bin/opencv_test_photo: modules/photo/CMakeFiles/opencv_test_photo.dir/test/ocl/test_denoising.cpp.o
bin/opencv_test_photo: modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_cloning.cpp.o
bin/opencv_test_photo: modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_decolor.cpp.o
bin/opencv_test_photo: modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoise_tvl1.cpp.o
bin/opencv_test_photo: modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoising.cpp.o
bin/opencv_test_photo: modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_denoising.cuda.cpp.o
bin/opencv_test_photo: modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_hdr.cpp.o
bin/opencv_test_photo: modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_inpaint.cpp.o
bin/opencv_test_photo: modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_main.cpp.o
bin/opencv_test_photo: modules/photo/CMakeFiles/opencv_test_photo.dir/test/test_npr.cpp.o
bin/opencv_test_photo: modules/photo/CMakeFiles/opencv_test_photo.dir/build.make
bin/opencv_test_photo: lib/libopencv_ts.a
bin/opencv_test_photo: lib/libopencv_photo.so.3.4.7
bin/opencv_test_photo: lib/libopencv_highgui.so.3.4.7
bin/opencv_test_photo: lib/libopencv_videoio.so.3.4.7
bin/opencv_test_photo: lib/libopencv_imgcodecs.so.3.4.7
bin/opencv_test_photo: lib/libopencv_imgproc.so.3.4.7
bin/opencv_test_photo: lib/libopencv_core.so.3.4.7
bin/opencv_test_photo: modules/photo/CMakeFiles/opencv_test_photo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking CXX executable ../../bin/opencv_test_photo"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/opencv_test_photo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
modules/photo/CMakeFiles/opencv_test_photo.dir/build: bin/opencv_test_photo

.PHONY : modules/photo/CMakeFiles/opencv_test_photo.dir/build

modules/photo/CMakeFiles/opencv_test_photo.dir/clean:
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo && $(CMAKE_COMMAND) -P CMakeFiles/opencv_test_photo.dir/cmake_clean.cmake
.PHONY : modules/photo/CMakeFiles/opencv_test_photo.dir/clean

modules/photo/CMakeFiles/opencv_test_photo.dir/depend:
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7 /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/photo /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/photo/CMakeFiles/opencv_test_photo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/photo/CMakeFiles/opencv_test_photo.dir/depend

