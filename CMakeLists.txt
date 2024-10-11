# -------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(
  memory_pool
  VERSION 0.0.1
  LANGUAGES CXX)
include(CTest)

if(NOT CMAKE_BUILD_TYPE)
  set(MEMORY_POOL_DEFAULT_BUILD_TYPE "Release")
  message(STATUS "Setting build type to '${MEMORY_POOL_DEFAULT_BUILD_TYPE}'.")
  set(CMAKE_BUILD_TYPE
      "${MEMORY_POOL_DEFAULT_BUILD_TYPE}"
      CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Enable/disable testing with -DMEMORY_POOL_ENABLE_TESTING=ON/OFF
option(MEMORY_POOL_ENABLE_TESTING "Enable project tests." ${BUILD_TESTING})

# Enable/disable benchmarking with -DMEMORY_POOL_ENABLE_BENCHMARKING=ON/OFF
option(MEMORY_POOL_ENABLE_BENCHMARKING "Enable project benchmarks." ON)

# Handles a linker error that arises with XCode 15+
if(APPLE)
  include(CheckLinkerFlag)
  check_linker_flag(CXX "-Wl,-ld_classic" CAN_USE_LD_CLASSIC_LINKER_FLAG)
  if(CAN_USE_LD_CLASSIC_LINKER_FLAG)
    list(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,-ld_classic")
  endif()
endif()

add_subdirectory(src)
add_subdirectory(examples)
if(MEMORY_POOL_ENABLE_TESTING)
  add_subdirectory(tests)
endif()
if(MEMORY_POOL_ENABLE_BENCHMARKING)
  add_subdirectory(benchmark)
endif()

# -------------------------------------------------------------------------------------------------
