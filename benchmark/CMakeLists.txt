find_package(benchmark REQUIRED)

function(add_benchmark name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name}
    PRIVATE
      benchmark::benchmark
      benchmark::benchmark_main
      morton
    )
endfunction()

add_benchmark(morton2d_benchmark)
add_benchmark(morton3d_benchmark)