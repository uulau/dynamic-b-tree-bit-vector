cmake_minimum_required (VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)

project ("DynamicBTreeBitVector" VERSION 1.0 DESCRIPTION "Benchmarks different B-tree implementations for backing a dynamic bit vector." LANGUAGES CXX)

include(FetchContent)
include(CTest)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.1
)

FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.5.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate( googletest)
  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(benchmark)
if(NOT benchmark_POPULATED)
  FetchContent_Populate(benchmark)
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Enable testing of the benchmark library." FORCE)
  add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

if(MSVC)
  add_compile_options(/W4 /arch:AVX2 /Qvec-report:2 -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize -mavx2 -march=native)
else()
  add_compile_options(-Wall -Wextra -pedantic -mavx2 -fopt-info-vec-missed)
endif()

include_directories ("include" "include/sdsl")

add_subdirectory ("test-case-generator")
add_subdirectory ("benchmark")
add_subdirectory ("profiler")
add_subdirectory ("tests")
