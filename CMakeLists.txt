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

# SQLite is used in the historical logging module as one of the plugin types
include(FindSQLite3)
if(NOT SQLite3_FOUND)
  message(STATUS "SQLite Libraries not found")
else()
  add_compile_definitions(HAVE_SQLITE)
endif()


find_program(HAVE_ASCIIDOC_PDF NAMES asciidoctor-pdf)
find_program(HAVE_ASCIIDOC NAMES asciidoctor)
if(HAVE_ASCIIDOC_PDF)
    message("Found Asciidoctor PDF" ${HAVE_ASCIIDOC_PDF})
endif()
if(HAVE_ASCIIDOC)
    message("Found Asciidoctor" ${HAVE_ASCIIDOC_PDF})
endif()

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

if(BUILD_DOC)
  message("Building Documentation")
  add_subdirectory(doc)
endif()
