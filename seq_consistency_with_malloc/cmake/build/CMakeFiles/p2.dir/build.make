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
CMAKE_SOURCE_DIR = /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build

# Include any dependencies generated for this target.
include CMakeFiles/p2.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/p2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/p2.dir/flags.make

dsm.pb.cc: ../../dsm.proto
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating dsm.pb.cc, dsm.pb.h, dsm.grpc.pb.cc, dsm.grpc.pb.h"
	/home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/bin/protoc-3.17.3.0 --grpc_out /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build --cpp_out /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build -I /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency --plugin=protoc-gen-grpc="/home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/bin/grpc_cpp_plugin" /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/dsm.proto

dsm.pb.h: dsm.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate dsm.pb.h

dsm.grpc.pb.cc: dsm.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate dsm.grpc.pb.cc

dsm.grpc.pb.h: dsm.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate dsm.grpc.pb.h

CMakeFiles/p2.dir/p2.cc.o: CMakeFiles/p2.dir/flags.make
CMakeFiles/p2.dir/p2.cc.o: ../../p2.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/p2.dir/p2.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/p2.dir/p2.cc.o -c /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/p2.cc

CMakeFiles/p2.dir/p2.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/p2.dir/p2.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/p2.cc > CMakeFiles/p2.dir/p2.cc.i

CMakeFiles/p2.dir/p2.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/p2.dir/p2.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/p2.cc -o CMakeFiles/p2.dir/p2.cc.s

CMakeFiles/p2.dir/dsm.pb.cc.o: CMakeFiles/p2.dir/flags.make
CMakeFiles/p2.dir/dsm.pb.cc.o: dsm.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/p2.dir/dsm.pb.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/p2.dir/dsm.pb.cc.o -c /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/dsm.pb.cc

CMakeFiles/p2.dir/dsm.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/p2.dir/dsm.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/dsm.pb.cc > CMakeFiles/p2.dir/dsm.pb.cc.i

CMakeFiles/p2.dir/dsm.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/p2.dir/dsm.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/dsm.pb.cc -o CMakeFiles/p2.dir/dsm.pb.cc.s

CMakeFiles/p2.dir/dsm.grpc.pb.cc.o: CMakeFiles/p2.dir/flags.make
CMakeFiles/p2.dir/dsm.grpc.pb.cc.o: dsm.grpc.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/p2.dir/dsm.grpc.pb.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/p2.dir/dsm.grpc.pb.cc.o -c /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/dsm.grpc.pb.cc

CMakeFiles/p2.dir/dsm.grpc.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/p2.dir/dsm.grpc.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/dsm.grpc.pb.cc > CMakeFiles/p2.dir/dsm.grpc.pb.cc.i

CMakeFiles/p2.dir/dsm.grpc.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/p2.dir/dsm.grpc.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/dsm.grpc.pb.cc -o CMakeFiles/p2.dir/dsm.grpc.pb.cc.s

# Object files for target p2
p2_OBJECTS = \
"CMakeFiles/p2.dir/p2.cc.o" \
"CMakeFiles/p2.dir/dsm.pb.cc.o" \
"CMakeFiles/p2.dir/dsm.grpc.pb.cc.o"

# External object files for target p2
p2_EXTERNAL_OBJECTS =

p2: CMakeFiles/p2.dir/p2.cc.o
p2: CMakeFiles/p2.dir/dsm.pb.cc.o
p2: CMakeFiles/p2.dir/dsm.grpc.pb.cc.o
p2: CMakeFiles/p2.dir/build.make
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libgrpc++.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libprotobuf.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libgrpc.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libz.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libcares.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libaddress_sorting.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libre2.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_hash.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_city.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_wyhash.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_raw_hash_set.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_hashtablez_sampler.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_exponential_biased.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_statusor.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_bad_variant_access.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libgpr.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libupb.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_status.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_cord.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_str_format_internal.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_synchronization.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_stacktrace.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_symbolize.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_debugging_internal.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_demangle_internal.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_graphcycles_internal.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_malloc_internal.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_time.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_strings.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_throw_delegate.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_strings_internal.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_base.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_spinlock_wait.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_int128.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_civil_time.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_time_zone.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_bad_optional_access.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_raw_logging_internal.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_log_severity.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libssl.a
p2: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libcrypto.a
p2: CMakeFiles/p2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable p2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/p2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/p2.dir/build: p2

.PHONY : CMakeFiles/p2.dir/build

CMakeFiles/p2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/p2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/p2.dir/clean

CMakeFiles/p2.dir/depend: dsm.pb.cc
CMakeFiles/p2.dir/depend: dsm.pb.h
CMakeFiles/p2.dir/depend: dsm.grpc.pb.cc
CMakeFiles/p2.dir/depend: dsm.grpc.pb.h
	cd /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/CMakeFiles/p2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/p2.dir/depend

