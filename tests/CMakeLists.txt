# -------------------------------------------------------------------------------------------------
# Fetch doctest at configure-time
include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest
  GIT_TAG 6660e199adb32ef45ca9cdf36d06133842f60caf)
FetchContent_MakeAvailable(doctest)

# Define test_memory_pool executable and link to the required libraries
add_executable(test_memory_pool test_memory_pool.cpp)
target_link_libraries(test_memory_pool PRIVATE memory_pool doctest)

# Define the test target to be run when 'ctest' is invoked
add_test(NAME test_memory_pool COMMAND test_memory_pool)
# -------------------------------------------------------------------------------------------------
