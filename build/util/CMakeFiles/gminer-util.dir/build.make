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
include util/CMakeFiles/gminer-util.dir/depend.make

# Include the progress variables for this target.
include util/CMakeFiles/gminer-util.dir/progress.make

# Include the compile flags for this target's objects.
include util/CMakeFiles/gminer-util.dir/flags.make

# Object files for target gminer-util
gminer__util_OBJECTS =

# External object files for target gminer-util
gminer__util_EXTERNAL_OBJECTS = \
"/data/hzchen/GMiner-Playround/build/util/CMakeFiles/util-objs.dir/global.cpp.o" \
"/data/hzchen/GMiner-Playround/build/util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o" \
"/data/hzchen/GMiner-Playround/build/util/CMakeFiles/util-objs.dir/minhash.cpp.o" \
"/data/hzchen/GMiner-Playround/build/util/CMakeFiles/util-objs.dir/threadpool.cpp.o" \
"/data/hzchen/GMiner-Playround/build/util/CMakeFiles/util-objs.dir/timer.cpp.o" \
"/data/hzchen/GMiner-Playround/build/util/iniparser/CMakeFiles/iniparser-objs.dir/dictionary.c.o" \
"/data/hzchen/GMiner-Playround/build/util/iniparser/CMakeFiles/iniparser-objs.dir/iniparser.c.o"

util/libgminer-util.a: util/CMakeFiles/util-objs.dir/global.cpp.o
util/libgminer-util.a: util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o
util/libgminer-util.a: util/CMakeFiles/util-objs.dir/minhash.cpp.o
util/libgminer-util.a: util/CMakeFiles/util-objs.dir/threadpool.cpp.o
util/libgminer-util.a: util/CMakeFiles/util-objs.dir/timer.cpp.o
util/libgminer-util.a: util/iniparser/CMakeFiles/iniparser-objs.dir/dictionary.c.o
util/libgminer-util.a: util/iniparser/CMakeFiles/iniparser-objs.dir/iniparser.c.o
util/libgminer-util.a: util/CMakeFiles/gminer-util.dir/build.make
util/libgminer-util.a: util/CMakeFiles/gminer-util.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data/hzchen/GMiner-Playround/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking CXX static library libgminer-util.a"
	cd /data/hzchen/GMiner-Playround/build/util && $(CMAKE_COMMAND) -P CMakeFiles/gminer-util.dir/cmake_clean_target.cmake
	cd /data/hzchen/GMiner-Playround/build/util && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gminer-util.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
util/CMakeFiles/gminer-util.dir/build: util/libgminer-util.a

.PHONY : util/CMakeFiles/gminer-util.dir/build

util/CMakeFiles/gminer-util.dir/requires:

.PHONY : util/CMakeFiles/gminer-util.dir/requires

util/CMakeFiles/gminer-util.dir/clean:
	cd /data/hzchen/GMiner-Playround/build/util && $(CMAKE_COMMAND) -P CMakeFiles/gminer-util.dir/cmake_clean.cmake
.PHONY : util/CMakeFiles/gminer-util.dir/clean

util/CMakeFiles/gminer-util.dir/depend:
	cd /data/hzchen/GMiner-Playround/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/hzchen/GMiner-Playround /data/hzchen/GMiner-Playround/util /data/hzchen/GMiner-Playround/build /data/hzchen/GMiner-Playround/build/util /data/hzchen/GMiner-Playround/build/util/CMakeFiles/gminer-util.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : util/CMakeFiles/gminer-util.dir/depend
