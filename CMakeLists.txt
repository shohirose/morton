cmake_minimum_required(VERSION 3.8.2)
project(morton CXX)

option(MORTON_BUILD_TESTS "Build unit tests for motron library" ON)
option(MORTON_BUILD_BENCHMARK "Build benchmark for morton library" ON)

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
    # gcc and clang
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic>
    # msvc
    $<$<CXX_COMPILER_ID:MSVC>:/W3>
  )
target_compile_definitions(morton
  INTERFACE
    # For msvc
    # NOMINMAX          : To avoid errors related to std::min/std::max
    # _USE_MATH_DEFINES : To use M_PI etc.
    $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX _USE_MATH_DEFINES>
  )

if (MORTON_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

if (MORTON_BUILD_BENCHMARK)
  add_subdirectory(benchmark)
endif()