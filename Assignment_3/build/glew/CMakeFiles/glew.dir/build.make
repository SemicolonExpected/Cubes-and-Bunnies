# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_SOURCE_DIR = /root/CS-GY-6533/Assignment_3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/CS-GY-6533/Assignment_3/build

# Include any dependencies generated for this target.
include glew/CMakeFiles/glew.dir/depend.make

# Include the progress variables for this target.
include glew/CMakeFiles/glew.dir/progress.make

# Include the compile flags for this target's objects.
include glew/CMakeFiles/glew.dir/flags.make

glew/CMakeFiles/glew.dir/src/glew.c.o: glew/CMakeFiles/glew.dir/flags.make
glew/CMakeFiles/glew.dir/src/glew.c.o: /root/CS-GY-6533/ext/glew/src/glew.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/CS-GY-6533/Assignment_3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object glew/CMakeFiles/glew.dir/src/glew.c.o"
	cd /root/CS-GY-6533/Assignment_3/build/glew && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/glew.dir/src/glew.c.o   -c /root/CS-GY-6533/ext/glew/src/glew.c

glew/CMakeFiles/glew.dir/src/glew.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/glew.dir/src/glew.c.i"
	cd /root/CS-GY-6533/Assignment_3/build/glew && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/CS-GY-6533/ext/glew/src/glew.c > CMakeFiles/glew.dir/src/glew.c.i

glew/CMakeFiles/glew.dir/src/glew.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/glew.dir/src/glew.c.s"
	cd /root/CS-GY-6533/Assignment_3/build/glew && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/CS-GY-6533/ext/glew/src/glew.c -o CMakeFiles/glew.dir/src/glew.c.s

# Object files for target glew
glew_OBJECTS = \
"CMakeFiles/glew.dir/src/glew.c.o"

# External object files for target glew
glew_EXTERNAL_OBJECTS =

libglew.a: glew/CMakeFiles/glew.dir/src/glew.c.o
libglew.a: glew/CMakeFiles/glew.dir/build.make
libglew.a: glew/CMakeFiles/glew.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/CS-GY-6533/Assignment_3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library ../libglew.a"
	cd /root/CS-GY-6533/Assignment_3/build/glew && $(CMAKE_COMMAND) -P CMakeFiles/glew.dir/cmake_clean_target.cmake
	cd /root/CS-GY-6533/Assignment_3/build/glew && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/glew.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
glew/CMakeFiles/glew.dir/build: libglew.a

.PHONY : glew/CMakeFiles/glew.dir/build

glew/CMakeFiles/glew.dir/clean:
	cd /root/CS-GY-6533/Assignment_3/build/glew && $(CMAKE_COMMAND) -P CMakeFiles/glew.dir/cmake_clean.cmake
.PHONY : glew/CMakeFiles/glew.dir/clean

glew/CMakeFiles/glew.dir/depend:
	cd /root/CS-GY-6533/Assignment_3/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/CS-GY-6533/Assignment_3 /root/CS-GY-6533/ext/glew /root/CS-GY-6533/Assignment_3/build /root/CS-GY-6533/Assignment_3/build/glew /root/CS-GY-6533/Assignment_3/build/glew/CMakeFiles/glew.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : glew/CMakeFiles/glew.dir/depend

