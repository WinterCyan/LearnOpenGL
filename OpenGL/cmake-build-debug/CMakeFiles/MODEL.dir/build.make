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
include CMakeFiles/MODEL.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MODEL.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MODEL.dir/flags.make

CMakeFiles/MODEL.dir/model.cpp.o: CMakeFiles/MODEL.dir/flags.make
CMakeFiles/MODEL.dir/model.cpp.o: ../model.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/MODEL.dir/model.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MODEL.dir/model.cpp.o -c /home/wintercyan/code/clion/LearnOpenGL/OpenGL/model.cpp

CMakeFiles/MODEL.dir/model.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MODEL.dir/model.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wintercyan/code/clion/LearnOpenGL/OpenGL/model.cpp > CMakeFiles/MODEL.dir/model.cpp.i

CMakeFiles/MODEL.dir/model.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MODEL.dir/model.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wintercyan/code/clion/LearnOpenGL/OpenGL/model.cpp -o CMakeFiles/MODEL.dir/model.cpp.s

# Object files for target MODEL
MODEL_OBJECTS = \
"CMakeFiles/MODEL.dir/model.cpp.o"

# External object files for target MODEL
MODEL_EXTERNAL_OBJECTS =

MODEL: CMakeFiles/MODEL.dir/model.cpp.o
MODEL: CMakeFiles/MODEL.dir/build.make
MODEL: CMakeFiles/MODEL.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable MODEL"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MODEL.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MODEL.dir/build: MODEL

.PHONY : CMakeFiles/MODEL.dir/build

CMakeFiles/MODEL.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MODEL.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MODEL.dir/clean

CMakeFiles/MODEL.dir/depend:
	cd /home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wintercyan/code/clion/LearnOpenGL/OpenGL /home/wintercyan/code/clion/LearnOpenGL/OpenGL /home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug /home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug /home/wintercyan/code/clion/LearnOpenGL/OpenGL/cmake-build-debug/CMakeFiles/MODEL.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MODEL.dir/depend

