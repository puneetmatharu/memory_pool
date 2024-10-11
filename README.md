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