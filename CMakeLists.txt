cmake_policy(SET CMP0048 NEW)
cmake_minimum_required(VERSION 3.25)
project(TAGBASE VERSION 0.1.0 LANGUAGES C)

message("Project Name set to ${PROJECT_NAME} ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

set(BUILD_SHARED_LIBS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug
    Release
)
include(CTest)

option(BUILD_DOC "Build documentation." OFF)
option(BUILD_TESTING "Build the test suite" ON)

find_library(HAVE_PTHREAD NAMES pthread)
if( NOT HAVE_PTHREAD )
message(FATAL_ERROR "Pthread Library is required and not installed")
endif()

# find_library(HAVE_OPENSSL NAMES ssl)
# if( NOT HAVE_OPENSSL )
#   message(FATAL_ERROR "OpenSSL Library is required and not installed")
# endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Building Debug Symbols and Verbose Warnings")
  add_compile_options(-Wall -pedantic -O0 -DDEBUG)
endif()

# This is for the config.h file
include_directories(${PROJECT_BINARY_DIR}/src)
include_directories(src)
add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
