# -------------------------------------------------------------------------------------------------
# NOTE: Remember to link to the 'memory_pool::memory_pool' (header-only) library to help locate the
# memory_pool.h header.

# Define 'example_classes' target to be created from 'example_classes.cpp'
add_executable(example_classes example_classes.cpp)
target_link_libraries(example_classes PRIVATE memory_pool::memory_pool)

# Define 'my_struct' target to be created from 'my_struct.cpp'
add_executable(my_struct my_struct.cpp)
target_link_libraries(my_struct PRIVATE memory_pool::memory_pool)
# -------------------------------------------------------------------------------------------------
