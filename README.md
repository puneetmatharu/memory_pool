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

## `MemoryPool`

The `MemoryPool` class, templated by type `T`, implements four key operations:

- `create_pool(const SizeT& max_object)`: Creates a pool large enough for 'max_object' objects of type `T`
- `destroy_pool()`: Destroy the pool
- `allocate_object()`: Allocate an object from this pool
- `deallocate_object(T** const obj_pt)`: Deallocate an object from this pool

The interface looks like below:

```cpp
template<class T>
class MemoryPool
{
public:
  MemoryPool();
  MemoryPool(const SizeT& max_object);
  ~MemoryPool();

  void create_pool(const SizeT& max_object = g_MaxNumberOfObjectsInPool);
  void destroy_pool();

  T* allocate_object();
  T* allocate_object(T&& obj);
  void deallocate_object(T** const obj_pt);

  inline SizeT size();
  inline SizeT available_capacity();
};
```

## Creating your own example

Enter the `examples/` folder, and create a new example called, say, `mystruct_example.cpp`.

**`mystruct_example.cpp`**:

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

then create an executable target in the `CMakeLists.txt`.

**`CMakeLists.txt`**:

```cmake
# -------------------------------------------------------------------------------------------------
add_executable(example example.cpp)
target_link_libraries(example PRIVATE memory_pool::memory_pool)

add_executable(example2 example2.cpp)
target_link_libraries(example2 PRIVATE memory_pool::memory_pool)

<!-- NEW LINES BELOW -->
add_executable(mystruct_example mystruct_example.cpp)
target_link_libraries(mystruct_example PRIVATE memory_pool::memory_pool)
<!-- NEW LINES BELOW -->
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