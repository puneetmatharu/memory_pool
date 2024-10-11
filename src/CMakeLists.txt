# -------------------------------------------------------------------------------------------------
# NOTE: memory_pool is header-only so we define it as an INTERFACE library and just indicate where
# the relevant headers can be found
add_library(memory_pool INTERFACE)
target_include_directories(memory_pool INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
# -------------------------------------------------------------------------------------------------
