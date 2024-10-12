<div align="center">
  <img alt="memory_pool" src="./assets/logo.png" width="50%" max-width="300px">
</div>

<div align="center">
  <a href="./LICENSE">
    <img alt="License: MIT" src="https://img.shields.io/badge/license-MIT-blue">
  </a>
  <a href="../../actions/workflows/linux.yaml">
    <img alt="Ubuntu tests" src="../../actions/workflows/linux.yaml/badge.svg?branch=main">
  </a>
  <a href="../../actions/workflows/macos.yaml">
    <img alt="macOS tests" src="../../actions/workflows/macos.yaml/badge.svg?branch=main">
  </a>

</div>

<h2>memory_pool</h2>

A C++17 header-only library for a generic memory pool that provides quick memory allocation/deallocation for objects of a given type.

<!-- Use <h2> tags to omit heading from table of contents -->
<h2>Table of contents</h2>

- [Usage](#usage)
- [Options](#options)
- [`MemoryPool`](#memorypool)
- [Creating your own example](#creating-your-own-example)
- [Performance](#performance)
- [Pre-commit hooks](#pre-commit-hooks)


## Usage

The fastest way to give the project a spin is to run the following steps:

```bash
# Configure. Omit the '-G Ninja' if you do not have Ninja
cmake -G Ninja -B build

# Build examples/tests/benchmarks
cmake --build build

# Enter build directory
cd build

# Run tests
ctest 

# Run benchmarks
./benchmark/benchmark_memory_pool
```

## Options

You can control the behaviour of the configure/build process with the flags in the table below.

| Name                              | Description       | Default             |
| --------------------------------- | ----------------- | ------------------- |
| `MEMORY_POOL_ENABLE_TESTING`      | Enable tests      | `BUILD_TESTING`[^1] |
| `MEMORY_POOL_ENABLE_BENCHMARKING` | Enable benchmarks | `ON`                |

To use the above configuration options, you must specify the flags at configure-time using the `-D<FLAG>=ON`/`-D<FLAG>=OFF` syntax. For example

```bash
cmake -G Ninja -B build -D MEMORY_POOL_ENABLE_TESTING=OFF -D MEMORY_POOL_ENABLE_BENCHMARKING=OFF
```

**Important:** Make sure to wipe your `build/` directory if you wish to rebuild with different settings.

[^1]: By default, the value of `BUILD_TESTING` will be `ON` for the `memory_pool` project (if built as a standalone project).

## `MemoryPool`

The `MemoryPool` class, found in the `memory_pool` namespace, is templated by class `T` and implements four key operations:

- `allocate(const SizeT& num_blocks)`: Creates a pool large enough for '`num_blocks`' objects of type `T`
- `clear()`: Destroys the pool
- `new_block_pt()`: Allocates a block of memory to the user from the pool
- `delete_block_pt(T*& obj_pt)`: Deallocates an object from this pool and sets the input pointer to `nullptr`

A simplified version of the class interface is as follows:

```cpp
using SizeT = uint64_t;

template<class T>
class MemoryPool
{
public:
  // Default constructor; must call allocate() separately to create the pool
  MemoryPool();

  // Immediately creates a pool for 'num_blocks' objects of type T. The argument must
  // not exceed the value of 'g_MaxNumberOfObjectsInPool' in the 'memory_pool' namespace
  MemoryPool(const SizeT& num_blocks);

  // Destructor. Handles the clean-up
  ~MemoryPool();

  // Allocate space for 'num_blocks' objects of type T
  void allocate(const SizeT& num_blocks = g_MaxNumberOfObjectsInPool);

  // Clean up
  void clear();

  // Returns a pointer to an available block in the memory pool
  T* new_block_pt();

  // Returns a pointer to an available block in the memory pool and assigns 'obj' to the
  // location addressed by the pointer
  T* new_block_pt(T&& obj);

  // "Deletes" the data pointed to by 'obj_pt' and nullifies the input pointer. Do not try
  // to access obj_pt after this function has been called
  void delete_block_pt(T*& obj_pt);

  // The total number of objects this pool can hold
  SizeT size();

  // The remaining number of objects this pool can hold
  SizeT available_capacity();
};
```

## Creating your own example

Enter the `examples/` folder, and create a new example called, say, `clever_struct.cpp`.

**`clever_struct.cpp`**:

```cpp
#include "memory_pool.h"

using memory_pool::MemoryPool;

// Define your container
struct CleverStruct
{
  int a;
  float b;
  double c;
  int d;
};

int main()
{
  // NOTE: Number of objects in pool cannot exceed memory_pool::g_MaxNumberOfObjectsInPool
  const unsigned desired_pool_size = 100;
  const unsigned mid = unsigned(desired_pool_size / 2);

  // Create pool of size 'desired_pool_size' templated by your container
  MemoryPool<CleverStruct> pool(desired_pool_size);

  // Option 1: Assign to pointer after allocation
  for (unsigned i = 0; i < mid; i++)
  {
    auto* obj_pt = pool.new_block_pt();
    *obj_pt = CleverStruct{1, 0.0, 42.0, -9};
  }

  // Option 2: Assign data by moving rvalue during allocation
  for (unsigned i = mid; i < desired_pool_size; i++)
  {
    auto* obj_pt = pool.new_block_pt(CleverStruct{1, 0.0, 42.0, -9});
  }

  // Do not try to allocate space than you have. The next line will lead to an out-of-range error
  pool.new_block_pt(CleverStruct{1, 0.0, 42.0, -9});
}
```

then create an executable target in the `CMakeLists.txt`.

**`CMakeLists.txt`**:

```cmake
# -------------------------------------------------------------------------------------------------
# NOTE: Remember to link to the 'memory_pool::memory_pool' (header-only) library to help 
# locate the memory_pool.h header.

# Define 'example_classes' target to be created from 'example_classes.cpp'
add_executable(example_classes example_classes.cpp)
target_link_libraries(example_classes PRIVATE memory_pool::memory_pool)

# Define 'my_struct' target to be created from 'my_struct.cpp'
add_executable(my_struct my_struct.cpp)
target_link_libraries(my_struct PRIVATE memory_pool::memory_pool)

<!-- NEW LINES -->
# Define 'clever_struct' target to be created from 'clever_struct.cpp'
add_executable(clever_struct clever_struct.cpp)
target_link_libraries(clever_struct PRIVATE memory_pool::memory_pool)
<!-- NEW LINES -->
# -------------------------------------------------------------------------------------------------
```

## Performance

Below we report timing results and the computational complexity of the four key operations (described earlier in [`MemoryPool`](#memorypool)). The results have been computed by running the `benchmark/benchmark_memory_pool.cpp` benchmark driver on a 2021 Apple M1 MacBook Pro, which uses the [`Google/benchmark`](https://github.com/google/benchmark) C++ library.

> **Note:** The value of `g_MaxNumberOfObjectsInPool` was overridden temporarily to allow these results to be computed for pool sizes greater than 1000.

**Reported system information:**

```bash
Run on (10 X 23.9998 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB
  L1 Instruction 128 KiB
  L2 Unified 4096 KiB (x10)
Load Average: 1.90, 2.32, 2.48
```

The results we present below illustrate the performance of `MemoryPool` for pools templated by the `Derived` class in [`src/ExampleClasses.h`](src/ExampleClasses.h). Each results table shows three different types of results:

- `<benchmark-name>/<pool-size>`: the results from running `<benchmark-name>` with a pool size of `<pool-size>`
- `<benchmark-name>_BigO`: the coefficient for the high-order term in the running time (Big O complexity)
- `<benchmark-name>_RMS`: the normalized root-mean square error

**Pool creation:**

The table below illustrates how the runtime cost of pool creation changes with increasing pool size. The creation process has $O(N)$ runtime complexity. It is dominated by the cost of allocating a raw array for each pool.

```bash
------------------------------------------------------------------------------------------
Benchmark                                                Time             CPU   Iterations
------------------------------------------------------------------------------------------
benchmark_table_pool_creation/8                        275 ns          275 ns      2567112
benchmark_table_pool_creation/32                       258 ns          258 ns      2875617
benchmark_table_pool_creation/128                      394 ns          394 ns      1757849
benchmark_table_pool_creation/512                      872 ns          871 ns       777596
benchmark_table_pool_creation/2048                    2756 ns         2755 ns       253687
benchmark_table_pool_creation/8192                    9480 ns         9479 ns        73973
benchmark_table_pool_creation/32768                  36018 ns        36014 ns        19342
benchmark_table_pool_creation_BigO                    1.10 N          1.10 N    
benchmark_table_pool_creation_RMS                        5 %             5 %    
```

**Pool destruction:**

The table below illustrates how the runtime cost of pool destruction changes with increasing pool size. The destruction process has $O(N)$ runtime complexity. It is dominated by the cost of deallocating a raw array for each pool.

```bash
------------------------------------------------------------------------------------------
Benchmark                                                Time             CPU   Iterations
------------------------------------------------------------------------------------------
benchmark_table_pool_destruction/8                     884 ns          886 ns       782464
benchmark_table_pool_destruction/32                    908 ns          910 ns       796477
benchmark_table_pool_destruction/128                   928 ns          930 ns       749914
benchmark_table_pool_destruction/512                  1082 ns         1085 ns       642220
benchmark_table_pool_destruction/2048                 1705 ns         1712 ns       410425
benchmark_table_pool_destruction/8192                 4032 ns         4037 ns       173259
benchmark_table_pool_destruction/32768               13312 ns        13314 ns        52459
benchmark_table_pool_destruction_BigO                 0.41 N          0.41 N    
benchmark_table_pool_destruction_RMS                    24 %            24 %   
```

Pool allocation has $O(N)$ complexity.

**Allocating a new block:**

*Sequential allocations:*

The table below illustrates how the runtime cost of allocating a full pool changes with increasing pool size. Within each benchmark run, we allocate `<pool-size>` blocks *sequentially* for the user. 

Since this process has $O(N)$ cost (see table) for a full pool of allocations, we can deduce that a single allocation has $O(1)$ cost. This is to be expected, since the allocation process just involves popping a value from a stack, creating a pointer and doing some simple pointer arithmetic.

```bash
------------------------------------------------------------------------------------------
Benchmark                                                Time             CPU   Iterations
------------------------------------------------------------------------------------------
benchmark_table_pool_allocation/8                      811 ns          813 ns       860670
benchmark_table_pool_allocation/32                     804 ns          807 ns       858727
benchmark_table_pool_allocation/128                    885 ns          888 ns       794092
benchmark_table_pool_allocation/512                   1175 ns         1178 ns       605961
benchmark_table_pool_allocation/2048                  2244 ns         2245 ns       313271
benchmark_table_pool_allocation/8192                  6015 ns         6016 ns       116065
benchmark_table_pool_allocation/32768                21382 ns        21381 ns        32693
benchmark_table_pool_allocation_BigO                  0.66 N          0.66 N    
benchmark_table_pool_allocation_RMS                     16 %            16 %    
```

*Random allocations:*

With this benchmark, we allocate `<pool-size>` blocks *in a random order* for the user. To do so, we first allocate all blocks, reshuffle their pointers and deallocate all blocks. We then allocate all of the blocks, and time this step. As a result, the blocks will be allocated in a random order. (Note: this does rely on knowing that the blocks are allocated in the reverse of the order that they were deallocated.)

Again, this process has $O(N)$ cost (see table) for a full pool of allocations, so we can deduce that a single (random) allocation has $O(1)$ cost.

```bash
------------------------------------------------------------------------------------------
Benchmark                                                Time             CPU   Iterations
------------------------------------------------------------------------------------------
benchmark_table_pool_random_allocations/8            61544 ns        61722 ns        11232
benchmark_table_pool_random_allocations/32           62768 ns        63001 ns        11179
benchmark_table_pool_random_allocations/128          73985 ns        74192 ns         9583
benchmark_table_pool_random_allocations/512         110229 ns       110492 ns         6462
benchmark_table_pool_random_allocations/2048        241296 ns       241700 ns         2892
benchmark_table_pool_random_allocations/8192        756520 ns       756558 ns          919
benchmark_table_pool_random_allocations/32768      2789072 ns      2789341 ns          249
benchmark_table_pool_random_allocations_BigO         85.70 N         85.71 N    
benchmark_table_pool_random_allocations_RMS             10 %            10 %    
```

**Deallocating a block:**

*Sequential deallocations:*

The table below illustrates how the runtime cost of deallocating a full pool changes with increasing pool size. Within each benchmark run, we allocate `<pool-size>` blocks *sequentially* for the user. To do so, we first allocate all of the blocks.

Since this process has $O(N)$ cost (see table) for a full pool of deallocations, we can deduce that a single deallocation has $O(1)$ cost. Again, this is to be expected, since the deallocation process just involves pushing a value from a stack and setting a pointer to `nullptr`. We do not need to touch the block of memory that has to be "deallocated"; it will simply be overwritten at a later time.

```bash
------------------------------------------------------------------------------------------
Benchmark                                                Time             CPU   Iterations
------------------------------------------------------------------------------------------
benchmark_table_pool_deallocation/8                    816 ns          817 ns       858538
benchmark_table_pool_deallocation/32                   831 ns          834 ns       839721
benchmark_table_pool_deallocation/128                  998 ns          997 ns       694190
benchmark_table_pool_deallocation/512                 1541 ns         1544 ns       460814
benchmark_table_pool_deallocation/2048                3565 ns         3566 ns       195199
benchmark_table_pool_deallocation/8192               11754 ns        11751 ns        57452
benchmark_table_pool_deallocation/32768              43203 ns        43179 ns        16142
benchmark_table_pool_deallocation_BigO                1.33 N          1.33 N    
benchmark_table_pool_deallocation_RMS                    9 %             9 %    
```

*Random deallocations:*

With this benchmark, we deallocate `<pool-size>` blocks *in a random order* for the user. To do so, we first allocate all blocks then reshuffle their pointers. We then deallocate all of the blocks, and time this step.

Again, this process has $O(N)$ cost (see table) for a full pool of deallocations, so we can deduce that a single (random) deallocation has $O(1)$ cost.

```bash
------------------------------------------------------------------------------------------
Benchmark                                                Time             CPU   Iterations
------------------------------------------------------------------------------------------
benchmark_table_pool_random_deallocations/8          62827 ns        62230 ns        11374
benchmark_table_pool_random_deallocations/32         62770 ns        62921 ns        11154
benchmark_table_pool_random_deallocations/128        73769 ns        73990 ns         9429
benchmark_table_pool_random_deallocations/512       111119 ns       111396 ns         6351
benchmark_table_pool_random_deallocations/2048      258532 ns       259046 ns         2693
benchmark_table_pool_random_deallocations/8192      829133 ns       829298 ns          842
benchmark_table_pool_random_deallocations/32768    3095973 ns      3095656 ns          224
benchmark_table_pool_random_deallocations_BigO       95.03 N         95.02 N    
benchmark_table_pool_random_deallocations_RMS            9 %             9 % 
```

## Pre-commit hooks

To use the pre-commit hooks to format the C++ and CMake files, you will need to install [pre-commit](https://pre-commit.com). You can do so easily with the Python package manager, `pip`:

```bash
pip install pre-commit
```

To install the pre-commit hooks, run

```bash
pre-commit install
```

When you make a commit, these hooks will run and will format the files appropriately. If the tests fail, stage the files that have been changed and attempt to make a commit again. The commit should work the second time around.