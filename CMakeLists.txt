cmake_minimum_required(VERSION 3.10)

# Extract version from configure.ac.
set(VERSION_REGEX "^AC_INIT\\(\\[libconfig\\],[ \t]*\\[([0-9.]+)\\],.*")
file(STRINGS "configure.ac"
  VERSION_STRING REGEX ${VERSION_REGEX})
string(REGEX REPLACE ${VERSION_REGEX} "\\1" VERSION_STRING "${VERSION_STRING}")

project(libconfig LANGUAGES C VERSION ${VERSION_STRING})
option(BUILD_EXAMPLES "Enable examples" ON)
option(BUILD_SHARED_LIBS  "Enable shared library" ON)
option(BUILD_TESTS "Enable tests" ON)
option(BUILD_FUZZERS "Enable fuzzers" OFF)
option(BUILD_CXX "Build the C++ library in addition to the C library" ON)

if(BUILD_CXX)
	enable_language(CXX)
endif()

if(NOT PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	# Git auto-ignore out-of-source build directory
	file(GENERATE OUTPUT .gitignore CONTENT "*")
endif()

set_property(GLOBAL	PROPERTY USE_FOLDERS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

include(GNUInstallDirs)
include(CheckSymbolExists)
add_subdirectory(lib)

if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tinytest)
	add_subdirectory(tests)
endif()

if (BUILD_FUZZERS AND DEFINED ENV{LIB_FUZZING_ENGINE})
	add_subdirectory(fuzz)
endif()
