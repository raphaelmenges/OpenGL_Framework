# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lochmann/projects/OpenGL_Framework/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lochmann/projects/OpenGL_Framework/BUILD_Sublime

# Utility rule file for SHADERS.

# Include the progress variables for this target.
include shaders/CMakeFiles/SHADERS.dir/progress.make

shaders/CMakeFiles/SHADERS:

SHADERS: shaders/CMakeFiles/SHADERS
SHADERS: shaders/CMakeFiles/SHADERS.dir/build.make
.PHONY : SHADERS

# Rule to build all files generated by this target.
shaders/CMakeFiles/SHADERS.dir/build: SHADERS
.PHONY : shaders/CMakeFiles/SHADERS.dir/build

shaders/CMakeFiles/SHADERS.dir/clean:
	cd /home/lochmann/projects/OpenGL_Framework/BUILD_Sublime/shaders && $(CMAKE_COMMAND) -P CMakeFiles/SHADERS.dir/cmake_clean.cmake
.PHONY : shaders/CMakeFiles/SHADERS.dir/clean

shaders/CMakeFiles/SHADERS.dir/depend:
	cd /home/lochmann/projects/OpenGL_Framework/BUILD_Sublime && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lochmann/projects/OpenGL_Framework/src /home/lochmann/projects/OpenGL_Framework/src/shaders /home/lochmann/projects/OpenGL_Framework/BUILD_Sublime /home/lochmann/projects/OpenGL_Framework/BUILD_Sublime/shaders /home/lochmann/projects/OpenGL_Framework/BUILD_Sublime/shaders/CMakeFiles/SHADERS.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : shaders/CMakeFiles/SHADERS.dir/depend

