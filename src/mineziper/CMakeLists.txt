# Minimum CMake required
cmake_minimum_required(VERSION 3.11)

# Project
project(mineziper LANGUAGES C VERSION 1.0.0)

# Opions
option(BUILD_TESTS "Build test programs" OFF)

# Config
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Add cmake folder for 
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# The yarattd library
add_subdirectory(libmineziper)

# The mineziper executable program
add_subdirectory(mineziper)

# Tests
if(BUILD_TESTS)
    add_subdirectory(tests)
endif(BUILD_TESTS)

# Summary
message(STATUS "Configuration summary")
message(STATUS "Project name                 : ${PROJECT_NAME}")
message(STATUS "Project version              : ${PROJECT_VERSION}")
message(STATUS "Build Tests                  : ${BUILD_TESTS}")
