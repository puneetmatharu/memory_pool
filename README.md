# memory_pool

## Usage

```bash
# Option 1: Configure with Makefile generator
cmake -B build

# Option 2: Configure with Ninja generator
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

| Name                              | Description       | Default         |
| --------------------------------- | ----------------- | --------------- |
| `MEMORY_POOL_ENABLE_TESTING`      | Enable tests      | `BUILD_TESTING` |
| `MEMORY_POOL_ENABLE_BENCHMARKING` | Enable benchmarks | `ON`            |

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

## Creating your own example

```cpp
#include "memory_pool.h"

using memory_pool::MemoryPool;

// Define your container
struct MyStruct
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
  const unsigned mid = desired_pool_size / 2;

  // Create pool of size 'desired_pool_size' templated by your container
  MemoryPool<MyStruct> pool(desired_pool_size);

  // Option 1: Assign to pointer after allocation
  for (unsigned i = 0; i < mid; i++)
  {
    auto* obj_pt = pool.allocate_object();
    *obj_pt = MyStruct{1, 0.0, 42.0, -9};
  }

  // Option 2: Assign data by moving rvalue during allocation
  for (unsigned i = mid; i < desired_pool_size; i++)
  {
    auto* obj_pt = pool.allocate_object(MyStruct{1, 0.0, 42.0, -9});
  }

  // Do not try to allocate space than you have. Following line will lead to out of range error
  pool.allocate_object(MyStruct{1, 0.0, 42.0, -9});
}
```
