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
include apps/CMakeFiles/gm.dir/depend.make

# Include the progress variables for this target.
include apps/CMakeFiles/gm.dir/progress.make

# Include the compile flags for this target's objects.
include apps/CMakeFiles/gm.dir/flags.make

apps/CMakeFiles/gm.dir/graphmatch.cpp.o: apps/CMakeFiles/gm.dir/flags.make
apps/CMakeFiles/gm.dir/graphmatch.cpp.o: ../apps/graphmatch.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/hzchen/GMiner-Playround/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object apps/CMakeFiles/gm.dir/graphmatch.cpp.o"
	cd /data/hzchen/GMiner-Playround/build/apps && /data/opt/brew/bin/mpic++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gm.dir/graphmatch.cpp.o -c /data/hzchen/GMiner-Playround/apps/graphmatch.cpp

apps/CMakeFiles/gm.dir/graphmatch.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gm.dir/graphmatch.cpp.i"
	cd /data/hzchen/GMiner-Playround/build/apps && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/hzchen/GMiner-Playround/apps/graphmatch.cpp > CMakeFiles/gm.dir/graphmatch.cpp.i

apps/CMakeFiles/gm.dir/graphmatch.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gm.dir/graphmatch.cpp.s"
	cd /data/hzchen/GMiner-Playround/build/apps && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/hzchen/GMiner-Playround/apps/graphmatch.cpp -o CMakeFiles/gm.dir/graphmatch.cpp.s

apps/CMakeFiles/gm.dir/graphmatch.cpp.o.requires:

.PHONY : apps/CMakeFiles/gm.dir/graphmatch.cpp.o.requires

apps/CMakeFiles/gm.dir/graphmatch.cpp.o.provides: apps/CMakeFiles/gm.dir/graphmatch.cpp.o.requires
	$(MAKE) -f apps/CMakeFiles/gm.dir/build.make apps/CMakeFiles/gm.dir/graphmatch.cpp.o.provides.build
.PHONY : apps/CMakeFiles/gm.dir/graphmatch.cpp.o.provides

apps/CMakeFiles/gm.dir/graphmatch.cpp.o.provides.build: apps/CMakeFiles/gm.dir/graphmatch.cpp.o


# Object files for target gm
gm_OBJECTS = \
"CMakeFiles/gm.dir/graphmatch.cpp.o"

# External object files for target gm
gm_EXTERNAL_OBJECTS =

../release/gm: apps/CMakeFiles/gm.dir/graphmatch.cpp.o
../release/gm: apps/CMakeFiles/gm.dir/build.make
../release/gm: util/libgminer-util.a
../release/gm: apps/CMakeFiles/gm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data/hzchen/GMiner-Playround/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../release/gm"
	cd /data/hzchen/GMiner-Playround/build/apps && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
apps/CMakeFiles/gm.dir/build: ../release/gm

.PHONY : apps/CMakeFiles/gm.dir/build

apps/CMakeFiles/gm.dir/requires: apps/CMakeFiles/gm.dir/graphmatch.cpp.o.requires

.PHONY : apps/CMakeFiles/gm.dir/requires

apps/CMakeFiles/gm.dir/clean:
	cd /data/hzchen/GMiner-Playround/build/apps && $(CMAKE_COMMAND) -P CMakeFiles/gm.dir/cmake_clean.cmake
.PHONY : apps/CMakeFiles/gm.dir/clean

apps/CMakeFiles/gm.dir/depend:
	cd /data/hzchen/GMiner-Playround/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/hzchen/GMiner-Playround /data/hzchen/GMiner-Playround/apps /data/hzchen/GMiner-Playround/build /data/hzchen/GMiner-Playround/build/apps /data/hzchen/GMiner-Playround/build/apps/CMakeFiles/gm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : apps/CMakeFiles/gm.dir/depend

