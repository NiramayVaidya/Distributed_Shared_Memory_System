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
include CMakeFiles/psu_dsm.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/psu_dsm.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/psu_dsm.dir/flags.make

dsm.pb.cc: ../../dsm.proto
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating dsm.pb.cc, dsm.pb.h, dsm.grpc.pb.cc, dsm.grpc.pb.h"
	/home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/bin/protoc-3.17.3.0 --grpc_out /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build --cpp_out /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build -I /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency --plugin=protoc-gen-grpc="/home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/bin/grpc_cpp_plugin" /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/dsm.proto

dsm.pb.h: dsm.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate dsm.pb.h

dsm.grpc.pb.cc: dsm.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate dsm.grpc.pb.cc

dsm.grpc.pb.h: dsm.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate dsm.grpc.pb.h

CMakeFiles/psu_dsm.dir/psu_dsm.cc.o: CMakeFiles/psu_dsm.dir/flags.make
CMakeFiles/psu_dsm.dir/psu_dsm.cc.o: ../../psu_dsm.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/psu_dsm.dir/psu_dsm.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/psu_dsm.dir/psu_dsm.cc.o -c /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/psu_dsm.cc

CMakeFiles/psu_dsm.dir/psu_dsm.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/psu_dsm.dir/psu_dsm.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/psu_dsm.cc > CMakeFiles/psu_dsm.dir/psu_dsm.cc.i

CMakeFiles/psu_dsm.dir/psu_dsm.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/psu_dsm.dir/psu_dsm.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/psu_dsm.cc -o CMakeFiles/psu_dsm.dir/psu_dsm.cc.s

CMakeFiles/psu_dsm.dir/dsm.pb.cc.o: CMakeFiles/psu_dsm.dir/flags.make
CMakeFiles/psu_dsm.dir/dsm.pb.cc.o: dsm.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/psu_dsm.dir/dsm.pb.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/psu_dsm.dir/dsm.pb.cc.o -c /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/dsm.pb.cc

CMakeFiles/psu_dsm.dir/dsm.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/psu_dsm.dir/dsm.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/dsm.pb.cc > CMakeFiles/psu_dsm.dir/dsm.pb.cc.i

CMakeFiles/psu_dsm.dir/dsm.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/psu_dsm.dir/dsm.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/dsm.pb.cc -o CMakeFiles/psu_dsm.dir/dsm.pb.cc.s

CMakeFiles/psu_dsm.dir/dsm.grpc.pb.cc.o: CMakeFiles/psu_dsm.dir/flags.make
CMakeFiles/psu_dsm.dir/dsm.grpc.pb.cc.o: dsm.grpc.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/psu_dsm.dir/dsm.grpc.pb.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/psu_dsm.dir/dsm.grpc.pb.cc.o -c /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/dsm.grpc.pb.cc

CMakeFiles/psu_dsm.dir/dsm.grpc.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/psu_dsm.dir/dsm.grpc.pb.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/dsm.grpc.pb.cc > CMakeFiles/psu_dsm.dir/dsm.grpc.pb.cc.i

CMakeFiles/psu_dsm.dir/dsm.grpc.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/psu_dsm.dir/dsm.grpc.pb.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/dsm.grpc.pb.cc -o CMakeFiles/psu_dsm.dir/dsm.grpc.pb.cc.s

# Object files for target psu_dsm
psu_dsm_OBJECTS = \
"CMakeFiles/psu_dsm.dir/psu_dsm.cc.o" \
"CMakeFiles/psu_dsm.dir/dsm.pb.cc.o" \
"CMakeFiles/psu_dsm.dir/dsm.grpc.pb.cc.o"

# External object files for target psu_dsm
psu_dsm_EXTERNAL_OBJECTS =

psu_dsm: CMakeFiles/psu_dsm.dir/psu_dsm.cc.o
psu_dsm: CMakeFiles/psu_dsm.dir/dsm.pb.cc.o
psu_dsm: CMakeFiles/psu_dsm.dir/dsm.grpc.pb.cc.o
psu_dsm: CMakeFiles/psu_dsm.dir/build.make
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libgrpc++.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libprotobuf.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libgrpc.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libz.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libcares.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libaddress_sorting.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libre2.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_hash.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_city.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_wyhash.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_raw_hash_set.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_hashtablez_sampler.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_exponential_biased.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_statusor.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_bad_variant_access.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libgpr.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libupb.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_status.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_cord.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_str_format_internal.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_synchronization.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_stacktrace.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_symbolize.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_debugging_internal.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_demangle_internal.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_graphcycles_internal.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_malloc_internal.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_time.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_strings.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_throw_delegate.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_strings_internal.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_base.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_spinlock_wait.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_int128.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_civil_time.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_time_zone.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_bad_optional_access.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_raw_logging_internal.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libabsl_log_severity.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libssl.a
psu_dsm: /home/niramay/Documents/Fall_2021/CSE_511_OS/gRPC/lib/libcrypto.a
psu_dsm: CMakeFiles/psu_dsm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable psu_dsm"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/psu_dsm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/psu_dsm.dir/build: psu_dsm

.PHONY : CMakeFiles/psu_dsm.dir/build

CMakeFiles/psu_dsm.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/psu_dsm.dir/cmake_clean.cmake
.PHONY : CMakeFiles/psu_dsm.dir/clean

CMakeFiles/psu_dsm.dir/depend: dsm.pb.cc
CMakeFiles/psu_dsm.dir/depend: dsm.pb.h
CMakeFiles/psu_dsm.dir/depend: dsm.grpc.pb.cc
CMakeFiles/psu_dsm.dir/depend: dsm.grpc.pb.h
	cd /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build /home/niramay/Documents/Fall_2021/CSE_511_OS/project-2-dsm-and-map-reduce-framework-NiramayVaidya/seq-consistency/cmake/build/CMakeFiles/psu_dsm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/psu_dsm.dir/depend

