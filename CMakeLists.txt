cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(
  arm_cpp_test
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Handles a linker error that arises with XCode 15+
if(APPLE)
  include(CheckLinkerFlag)
  check_linker_flag(CXX "-Wl,-ld_classic" CAN_USE_LD_CLASSIC_LINKER_FLAG)
  if(CAN_USE_LD_CLASSIC_LINKER_FLAG)
    list(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,-ld_classic")
  endif()
endif()

add_executable(main main.cpp memory_pool.h)
