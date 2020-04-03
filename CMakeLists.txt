cmake_minimum_required(VERSION 3.8)
project(morton CXX)

option(MORTON_BUILD_TESTS "Build unit tests for motron library" ON)

add_library(morton INTERFACE)
target_include_directories(morton
  INTERFACE
    include
  )
target_compile_features(morton
  INTERFACE
    cxx_std_11
  )
target_compile_options(morton
  INTERFACE
    -Wall
    -Wextra
    -Wpedantic
  )

if (MORTON_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()