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

# Utility rule file for opencv_java_jar.

# Include the progress variables for this target.
include modules/java/jar/CMakeFiles/opencv_java_jar.dir/progress.make

modules/java/jar/CMakeFiles/opencv_java_jar: CMakeFiles/dephelper/opencv_java_jar


CMakeFiles/dephelper/opencv_java_jar: CMakeFiles/dephelper/opencv_java_jar_source_copy
CMakeFiles/dephelper/opencv_java_jar: modules/java/jar/opencv/build.xml
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating opencv-347.jar"
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/java/jar/opencv && /bin/ant -noinput -k jar
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/java/jar/opencv && /work/irlin355_1/gratienj/local/eb_new/centos_7/software/Compiler/GCCcore/7.3.0/CMake/3.11.4/bin/cmake -E touch /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles/dephelper/opencv_java_jar

CMakeFiles/dephelper/opencv_java_jar_source_copy: ../cmake/copy_files.cmake
CMakeFiles/dephelper/opencv_java_jar_source_copy: CMakeFiles/dephelper/gen_opencv_java_source
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Copy Java(JAR) source files"
	/work/irlin355_1/gratienj/local/eb_new/centos_7/software/Compiler/GCCcore/7.3.0/CMake/3.11.4/bin/cmake -DCONFIG_FILE:PATH=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/java/jar/copyfiles-JAVA_SRC_COPY.cmake -DCOPYLIST_VAR:STRING=JAVA_SRC_COPY -DDEPHELPER=/work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/CMakeFiles/dephelper/opencv_java_jar_source_copy -P /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/cmake/copy_files.cmake

opencv_java_jar: modules/java/jar/CMakeFiles/opencv_java_jar
opencv_java_jar: CMakeFiles/dephelper/opencv_java_jar
opencv_java_jar: CMakeFiles/dephelper/opencv_java_jar_source_copy
opencv_java_jar: modules/java/jar/CMakeFiles/opencv_java_jar.dir/build.make

.PHONY : opencv_java_jar

# Rule to build all files generated by this target.
modules/java/jar/CMakeFiles/opencv_java_jar.dir/build: opencv_java_jar

.PHONY : modules/java/jar/CMakeFiles/opencv_java_jar.dir/build

modules/java/jar/CMakeFiles/opencv_java_jar.dir/clean:
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/java/jar && $(CMAKE_COMMAND) -P CMakeFiles/opencv_java_jar.dir/cmake_clean.cmake
.PHONY : modules/java/jar/CMakeFiles/opencv_java_jar.dir/clean

modules/java/jar/CMakeFiles/opencv_java_jar.dir/depend:
	cd /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7 /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/modules/java/jar /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/java/jar /work/irlin355_1/gratienj/ParallelProgrammingCourse/ParallelProgrammingTP/contrib/opencv-3.4.7/build/modules/java/jar/CMakeFiles/opencv_java_jar.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/java/jar/CMakeFiles/opencv_java_jar.dir/depend
