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
include util/CMakeFiles/util-objs.dir/depend.make

# Include the progress variables for this target.
include util/CMakeFiles/util-objs.dir/progress.make

# Include the compile flags for this target's objects.
include util/CMakeFiles/util-objs.dir/flags.make

util/CMakeFiles/util-objs.dir/global.cpp.o: util/CMakeFiles/util-objs.dir/flags.make
util/CMakeFiles/util-objs.dir/global.cpp.o: ../util/global.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/hzchen/GMiner-Playround/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object util/CMakeFiles/util-objs.dir/global.cpp.o"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/util-objs.dir/global.cpp.o -c /data/hzchen/GMiner-Playround/util/global.cpp

util/CMakeFiles/util-objs.dir/global.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/util-objs.dir/global.cpp.i"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/hzchen/GMiner-Playround/util/global.cpp > CMakeFiles/util-objs.dir/global.cpp.i

util/CMakeFiles/util-objs.dir/global.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/util-objs.dir/global.cpp.s"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/hzchen/GMiner-Playround/util/global.cpp -o CMakeFiles/util-objs.dir/global.cpp.s

util/CMakeFiles/util-objs.dir/global.cpp.o.requires:

.PHONY : util/CMakeFiles/util-objs.dir/global.cpp.o.requires

util/CMakeFiles/util-objs.dir/global.cpp.o.provides: util/CMakeFiles/util-objs.dir/global.cpp.o.requires
	$(MAKE) -f util/CMakeFiles/util-objs.dir/build.make util/CMakeFiles/util-objs.dir/global.cpp.o.provides.build
.PHONY : util/CMakeFiles/util-objs.dir/global.cpp.o.provides

util/CMakeFiles/util-objs.dir/global.cpp.o.provides.build: util/CMakeFiles/util-objs.dir/global.cpp.o


util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o: util/CMakeFiles/util-objs.dir/flags.make
util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o: ../util/hdfs_core.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/hzchen/GMiner-Playround/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/util-objs.dir/hdfs_core.cpp.o -c /data/hzchen/GMiner-Playround/util/hdfs_core.cpp

util/CMakeFiles/util-objs.dir/hdfs_core.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/util-objs.dir/hdfs_core.cpp.i"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/hzchen/GMiner-Playround/util/hdfs_core.cpp > CMakeFiles/util-objs.dir/hdfs_core.cpp.i

util/CMakeFiles/util-objs.dir/hdfs_core.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/util-objs.dir/hdfs_core.cpp.s"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/hzchen/GMiner-Playround/util/hdfs_core.cpp -o CMakeFiles/util-objs.dir/hdfs_core.cpp.s

util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o.requires:

.PHONY : util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o.requires

util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o.provides: util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o.requires
	$(MAKE) -f util/CMakeFiles/util-objs.dir/build.make util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o.provides.build
.PHONY : util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o.provides

util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o.provides.build: util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o


util/CMakeFiles/util-objs.dir/minhash.cpp.o: util/CMakeFiles/util-objs.dir/flags.make
util/CMakeFiles/util-objs.dir/minhash.cpp.o: ../util/minhash.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/hzchen/GMiner-Playround/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object util/CMakeFiles/util-objs.dir/minhash.cpp.o"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/util-objs.dir/minhash.cpp.o -c /data/hzchen/GMiner-Playround/util/minhash.cpp

util/CMakeFiles/util-objs.dir/minhash.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/util-objs.dir/minhash.cpp.i"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/hzchen/GMiner-Playround/util/minhash.cpp > CMakeFiles/util-objs.dir/minhash.cpp.i

util/CMakeFiles/util-objs.dir/minhash.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/util-objs.dir/minhash.cpp.s"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/hzchen/GMiner-Playround/util/minhash.cpp -o CMakeFiles/util-objs.dir/minhash.cpp.s

util/CMakeFiles/util-objs.dir/minhash.cpp.o.requires:

.PHONY : util/CMakeFiles/util-objs.dir/minhash.cpp.o.requires

util/CMakeFiles/util-objs.dir/minhash.cpp.o.provides: util/CMakeFiles/util-objs.dir/minhash.cpp.o.requires
	$(MAKE) -f util/CMakeFiles/util-objs.dir/build.make util/CMakeFiles/util-objs.dir/minhash.cpp.o.provides.build
.PHONY : util/CMakeFiles/util-objs.dir/minhash.cpp.o.provides

util/CMakeFiles/util-objs.dir/minhash.cpp.o.provides.build: util/CMakeFiles/util-objs.dir/minhash.cpp.o


util/CMakeFiles/util-objs.dir/threadpool.cpp.o: util/CMakeFiles/util-objs.dir/flags.make
util/CMakeFiles/util-objs.dir/threadpool.cpp.o: ../util/threadpool.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/hzchen/GMiner-Playround/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object util/CMakeFiles/util-objs.dir/threadpool.cpp.o"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/util-objs.dir/threadpool.cpp.o -c /data/hzchen/GMiner-Playround/util/threadpool.cpp

util/CMakeFiles/util-objs.dir/threadpool.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/util-objs.dir/threadpool.cpp.i"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/hzchen/GMiner-Playround/util/threadpool.cpp > CMakeFiles/util-objs.dir/threadpool.cpp.i

util/CMakeFiles/util-objs.dir/threadpool.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/util-objs.dir/threadpool.cpp.s"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/hzchen/GMiner-Playround/util/threadpool.cpp -o CMakeFiles/util-objs.dir/threadpool.cpp.s

util/CMakeFiles/util-objs.dir/threadpool.cpp.o.requires:

.PHONY : util/CMakeFiles/util-objs.dir/threadpool.cpp.o.requires

util/CMakeFiles/util-objs.dir/threadpool.cpp.o.provides: util/CMakeFiles/util-objs.dir/threadpool.cpp.o.requires
	$(MAKE) -f util/CMakeFiles/util-objs.dir/build.make util/CMakeFiles/util-objs.dir/threadpool.cpp.o.provides.build
.PHONY : util/CMakeFiles/util-objs.dir/threadpool.cpp.o.provides

util/CMakeFiles/util-objs.dir/threadpool.cpp.o.provides.build: util/CMakeFiles/util-objs.dir/threadpool.cpp.o


util/CMakeFiles/util-objs.dir/timer.cpp.o: util/CMakeFiles/util-objs.dir/flags.make
util/CMakeFiles/util-objs.dir/timer.cpp.o: ../util/timer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/hzchen/GMiner-Playround/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object util/CMakeFiles/util-objs.dir/timer.cpp.o"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/util-objs.dir/timer.cpp.o -c /data/hzchen/GMiner-Playround/util/timer.cpp

util/CMakeFiles/util-objs.dir/timer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/util-objs.dir/timer.cpp.i"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/hzchen/GMiner-Playround/util/timer.cpp > CMakeFiles/util-objs.dir/timer.cpp.i

util/CMakeFiles/util-objs.dir/timer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/util-objs.dir/timer.cpp.s"
	cd /data/hzchen/GMiner-Playround/build/util && /data/opt/brew/bin/mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/hzchen/GMiner-Playround/util/timer.cpp -o CMakeFiles/util-objs.dir/timer.cpp.s

util/CMakeFiles/util-objs.dir/timer.cpp.o.requires:

.PHONY : util/CMakeFiles/util-objs.dir/timer.cpp.o.requires

util/CMakeFiles/util-objs.dir/timer.cpp.o.provides: util/CMakeFiles/util-objs.dir/timer.cpp.o.requires
	$(MAKE) -f util/CMakeFiles/util-objs.dir/build.make util/CMakeFiles/util-objs.dir/timer.cpp.o.provides.build
.PHONY : util/CMakeFiles/util-objs.dir/timer.cpp.o.provides

util/CMakeFiles/util-objs.dir/timer.cpp.o.provides.build: util/CMakeFiles/util-objs.dir/timer.cpp.o


util-objs: util/CMakeFiles/util-objs.dir/global.cpp.o
util-objs: util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o
util-objs: util/CMakeFiles/util-objs.dir/minhash.cpp.o
util-objs: util/CMakeFiles/util-objs.dir/threadpool.cpp.o
util-objs: util/CMakeFiles/util-objs.dir/timer.cpp.o
util-objs: util/CMakeFiles/util-objs.dir/build.make

.PHONY : util-objs

# Rule to build all files generated by this target.
util/CMakeFiles/util-objs.dir/build: util-objs

.PHONY : util/CMakeFiles/util-objs.dir/build

util/CMakeFiles/util-objs.dir/requires: util/CMakeFiles/util-objs.dir/global.cpp.o.requires
util/CMakeFiles/util-objs.dir/requires: util/CMakeFiles/util-objs.dir/hdfs_core.cpp.o.requires
util/CMakeFiles/util-objs.dir/requires: util/CMakeFiles/util-objs.dir/minhash.cpp.o.requires
util/CMakeFiles/util-objs.dir/requires: util/CMakeFiles/util-objs.dir/threadpool.cpp.o.requires
util/CMakeFiles/util-objs.dir/requires: util/CMakeFiles/util-objs.dir/timer.cpp.o.requires

.PHONY : util/CMakeFiles/util-objs.dir/requires

util/CMakeFiles/util-objs.dir/clean:
	cd /data/hzchen/GMiner-Playround/build/util && $(CMAKE_COMMAND) -P CMakeFiles/util-objs.dir/cmake_clean.cmake
.PHONY : util/CMakeFiles/util-objs.dir/clean

util/CMakeFiles/util-objs.dir/depend:
	cd /data/hzchen/GMiner-Playround/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/hzchen/GMiner-Playround /data/hzchen/GMiner-Playround/util /data/hzchen/GMiner-Playround/build /data/hzchen/GMiner-Playround/build/util /data/hzchen/GMiner-Playround/build/util/CMakeFiles/util-objs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : util/CMakeFiles/util-objs.dir/depend

