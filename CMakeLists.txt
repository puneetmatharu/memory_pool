cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(arm_cpp_test VERSION 0.0.1 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

list(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,-ld_classic")

add_executable(main main.cpp memory_pool.h)
