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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/wintercyan/code/clion/LearnOpenGL/OpenGL

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/BRDF.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/BRDF.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/BRDF.dir/flags.make

CMakeFiles/BRDF.dir/BRDF.cpp.o: CMakeFiles/BRDF.dir/flags.make
CMakeFiles/BRDF.dir/BRDF.cpp.o: ../BRDF.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/BRDF.dir/BRDF.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BRDF.dir/BRDF.cpp.o -c /home/wintercyan/code/clion/LearnOpenGL/OpenGL/BRDF.cpp

CMakeFiles/BRDF.dir/BRDF.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BRDF.dir/BRDF.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wintercyan/code/clion/LearnOpenGL/OpenGL/BRDF.cpp > CMakeFiles/BRDF.dir/BRDF.cpp.i

CMakeFiles/BRDF.dir/BRDF.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BRDF.dir/BRDF.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wintercyan/code/clion/LearnOpenGL/OpenGL/BRDF.cpp -o CMakeFiles/BRDF.dir/BRDF.cpp.s

# Object files for target BRDF
BRDF_OBJECTS = \
"CMakeFiles/BRDF.dir/BRDF.cpp.o"

# External object files for target BRDF
BRDF_EXTERNAL_OBJECTS =

BRDF: CMakeFiles/BRDF.dir/BRDF.cpp.o
BRDF: CMakeFiles/BRDF.dir/build.make
BRDF: CMakeFiles/BRDF.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable BRDF"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/BRDF.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/BRDF.dir/build: BRDF

.PHONY : CMakeFiles/BRDF.dir/build

CMakeFiles/BRDF.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/BRDF.dir/cmake_clean.cmake
.PHONY : CMakeFiles/BRDF.dir/clean

CMakeFiles/BRDF.dir/depend:
	cd /home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wintercyan/code/clion/LearnOpenGL/OpenGL /home/wintercyan/code/clion/LearnOpenGL/OpenGL /home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug /home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug /home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug/CMakeFiles/BRDF.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/BRDF.dir/depend

