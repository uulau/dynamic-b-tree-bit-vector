add_executable("benchmarks" "benchmark.cpp")
target_link_libraries("benchmarks" "benchmark")
target_include_directories("benchmarks" PUBLIC ${benchmark_SOURCE_DIR}/include/benchmark)