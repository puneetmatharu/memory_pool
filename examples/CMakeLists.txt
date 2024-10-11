# -------------------------------------------------------------------------------------------------
add_executable(example example.cpp)
target_link_libraries(example PRIVATE memory_pool::memory_pool)

add_executable(example2 example2.cpp)
target_link_libraries(example2 PRIVATE memory_pool::memory_pool)
# -------------------------------------------------------------------------------------------------
