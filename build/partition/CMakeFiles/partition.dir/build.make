# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /data/opt/brew/Cellar/cmake/3.10.1/bin/cmake

# The command to remove a file.
RM = /data/opt/brew/Cellar/cmake/3.10.1/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /data/hzchen/GMiner-Playround

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /data/hzchen/GMiner-Playround/build

# Include any dependencies generated for this target.
include partition/CMakeFiles/partition.dir/depend.make

# Include the progress variables for this target.
include partition/CMakeFiles/partition.dir/progress.make

# Include the compile flags for this target's objects.
include partition/CMakeFiles/partition.dir/flags.make

partition/CMakeFiles/partition.dir/partition.cpp.o: partition/CMakeFiles/partition.dir/flags.make
partition/CMakeFiles/partition.dir/partition.cpp.o: ../partition/partition.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/hzchen/GMiner-Playround/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object partition/CMakeFiles/partition.dir/partition.cpp.o"
	cd /data/hzchen/GMiner-Playround/build/partition && /data/opt/brew/bin/mpic++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/partition.dir/partition.cpp.o -c /data/hzchen/GMiner-Playround/partition/partition.cpp

partition/CMakeFiles/partition.dir/partition.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/partition.dir/partition.cpp.i"
	cd /data/hzchen/GMiner-Playround/build/partition && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/hzchen/GMiner-Playround/partition/partition.cpp > CMakeFiles/partition.dir/partition.cpp.i

partition/CMakeFiles/partition.dir/partition.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/partition.dir/partition.cpp.s"
	cd /data/hzchen/GMiner-Playround/build/partition && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/hzchen/GMiner-Playround/partition/partition.cpp -o CMakeFiles/partition.dir/partition.cpp.s

partition/CMakeFiles/partition.dir/partition.cpp.o.requires:

.PHONY : partition/CMakeFiles/partition.dir/partition.cpp.o.requires

partition/CMakeFiles/partition.dir/partition.cpp.o.provides: partition/CMakeFiles/partition.dir/partition.cpp.o.requires
	$(MAKE) -f partition/CMakeFiles/partition.dir/build.make partition/CMakeFiles/partition.dir/partition.cpp.o.provides.build
.PHONY : partition/CMakeFiles/partition.dir/partition.cpp.o.provides

partition/CMakeFiles/partition.dir/partition.cpp.o.provides.build: partition/CMakeFiles/partition.dir/partition.cpp.o


# Object files for target partition
partition_OBJECTS = \
"CMakeFiles/partition.dir/partition.cpp.o"

# External object files for target partition
partition_EXTERNAL_OBJECTS =

../release/partition: partition/CMakeFiles/partition.dir/partition.cpp.o
../release/partition: partition/CMakeFiles/partition.dir/build.make
../release/partition: partition/libgminer-partition.a
../release/partition: util/libgminer-util.a
../release/partition: partition/CMakeFiles/partition.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data/hzchen/GMiner-Playround/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../release/partition"
	cd /data/hzchen/GMiner-Playround/build/partition && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/partition.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
partition/CMakeFiles/partition.dir/build: ../release/partition

.PHONY : partition/CMakeFiles/partition.dir/build

partition/CMakeFiles/partition.dir/requires: partition/CMakeFiles/partition.dir/partition.cpp.o.requires

.PHONY : partition/CMakeFiles/partition.dir/requires

partition/CMakeFiles/partition.dir/clean:
	cd /data/hzchen/GMiner-Playround/build/partition && $(CMAKE_COMMAND) -P CMakeFiles/partition.dir/cmake_clean.cmake
.PHONY : partition/CMakeFiles/partition.dir/clean

partition/CMakeFiles/partition.dir/depend:
	cd /data/hzchen/GMiner-Playround/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/hzchen/GMiner-Playround /data/hzchen/GMiner-Playround/partition /data/hzchen/GMiner-Playround/build /data/hzchen/GMiner-Playround/build/partition /data/hzchen/GMiner-Playround/build/partition/CMakeFiles/partition.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : partition/CMakeFiles/partition.dir/depend

