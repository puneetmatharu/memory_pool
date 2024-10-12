<div align="center">
  <img alt="memory_pool" src="./assets/logo.png" width="50%" max-width="300px">
</div>

<h2>memory_pool</h2>

A C++17 header-only library for a generic memory pool allocator that provides quick memory allocation/deallocation for objects of a given type.

<!-- Use <h2> tags to omit heading from table of contents -->
<h2>Table of contents</h2>

- [Usage](#usage)
- [Options](#options)
- [`MemoryPool`](#memorypool)
- [Creating your own example](#creating-your-own-example)
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

- `allocate(const SizeT& max_object)`: Creates a pool large enough for '`max_object`' objects of type `T`
- `clear()`: Destroys the pool
- `new_block_pt()`: Allocates a block of memory to the user from the pool
- `delete_block_pt(T*& obj_pt)`: Deallocates an object from this pool and sets the input pointer to `nullptr`

The interface to the class looks like below:

```cpp
using SizeT = uint64_t;

template<class T>
class MemoryPool
{
public:
  MemoryPool();
  MemoryPool(const SizeT& max_object);
  ~MemoryPool();

  void allocate(const SizeT& max_object = g_MaxNumberOfObjectsInPool);
  void clear();

  T* new_block_pt();
  T* new_block_pt(T&& obj);
  void delete_block_pt(T*& obj_pt);

  SizeT size();
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