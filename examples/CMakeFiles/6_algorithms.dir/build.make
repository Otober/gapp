# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kimdoyoung/Desktop/work/gapp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kimdoyoung/Desktop/work/gapp

# Include any dependencies generated for this target.
include examples/CMakeFiles/6_algorithms.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include examples/CMakeFiles/6_algorithms.dir/compiler_depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/6_algorithms.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/6_algorithms.dir/flags.make

examples/CMakeFiles/6_algorithms.dir/6_algorithms.cpp.o: examples/CMakeFiles/6_algorithms.dir/flags.make
examples/CMakeFiles/6_algorithms.dir/6_algorithms.cpp.o: examples/6_algorithms.cpp
examples/CMakeFiles/6_algorithms.dir/6_algorithms.cpp.o: examples/CMakeFiles/6_algorithms.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/kimdoyoung/Desktop/work/gapp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/CMakeFiles/6_algorithms.dir/6_algorithms.cpp.o"
	cd /home/kimdoyoung/Desktop/work/gapp/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT examples/CMakeFiles/6_algorithms.dir/6_algorithms.cpp.o -MF CMakeFiles/6_algorithms.dir/6_algorithms.cpp.o.d -o CMakeFiles/6_algorithms.dir/6_algorithms.cpp.o -c /home/kimdoyoung/Desktop/work/gapp/examples/6_algorithms.cpp

examples/CMakeFiles/6_algorithms.dir/6_algorithms.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/6_algorithms.dir/6_algorithms.cpp.i"
	cd /home/kimdoyoung/Desktop/work/gapp/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kimdoyoung/Desktop/work/gapp/examples/6_algorithms.cpp > CMakeFiles/6_algorithms.dir/6_algorithms.cpp.i

examples/CMakeFiles/6_algorithms.dir/6_algorithms.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/6_algorithms.dir/6_algorithms.cpp.s"
	cd /home/kimdoyoung/Desktop/work/gapp/examples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kimdoyoung/Desktop/work/gapp/examples/6_algorithms.cpp -o CMakeFiles/6_algorithms.dir/6_algorithms.cpp.s

# Object files for target 6_algorithms
6_algorithms_OBJECTS = \
"CMakeFiles/6_algorithms.dir/6_algorithms.cpp.o"

# External object files for target 6_algorithms
6_algorithms_EXTERNAL_OBJECTS =

build/6_algorithms: examples/CMakeFiles/6_algorithms.dir/6_algorithms.cpp.o
build/6_algorithms: examples/CMakeFiles/6_algorithms.dir/build.make
build/6_algorithms: libgapp.a
build/6_algorithms: examples/CMakeFiles/6_algorithms.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/kimdoyoung/Desktop/work/gapp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../build/6_algorithms"
	cd /home/kimdoyoung/Desktop/work/gapp/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/6_algorithms.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/6_algorithms.dir/build: build/6_algorithms
.PHONY : examples/CMakeFiles/6_algorithms.dir/build

examples/CMakeFiles/6_algorithms.dir/clean:
	cd /home/kimdoyoung/Desktop/work/gapp/examples && $(CMAKE_COMMAND) -P CMakeFiles/6_algorithms.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/6_algorithms.dir/clean

examples/CMakeFiles/6_algorithms.dir/depend:
	cd /home/kimdoyoung/Desktop/work/gapp && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kimdoyoung/Desktop/work/gapp /home/kimdoyoung/Desktop/work/gapp/examples /home/kimdoyoung/Desktop/work/gapp /home/kimdoyoung/Desktop/work/gapp/examples /home/kimdoyoung/Desktop/work/gapp/examples/CMakeFiles/6_algorithms.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : examples/CMakeFiles/6_algorithms.dir/depend

