# -------------------------------------------------------------------------------------------------
# Fetch benchmark at configure-time
set(BENCHMARK_ENABLE_TESTING OFF)
include(FetchContent)
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark
  GIT_TAG v1.9.0)
FetchContent_MakeAvailable(benchmark)

# Define test_memory_pool executable and link to the required libraries
add_executable(benchmark_memory_pool benchmark_memory_pool.cpp)
target_link_libraries(benchmark_memory_pool PRIVATE memory_pool::memory_pool benchmark::benchmark)
# -------------------------------------------------------------------------------------------------
