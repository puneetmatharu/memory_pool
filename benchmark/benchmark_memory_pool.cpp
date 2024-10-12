#include <algorithm>
#include <random>
#include <benchmark/benchmark.h>
#include "ExampleClasses.h"
#include "memory_pool.h"

using memory_pool::MemoryPool;


static void benchmark_point_multiple_pool_allocations_with_memory_pool(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    MemoryPool<Point> pool;
    for (auto i = 0; i < 1000; i++)
    {
      pool.allocate(pool_size);
      pool.clear();
    }
  }
}


static void benchmark_point_multiple_pool_allocations_with_vector(benchmark::State& state)
{
  // NOTE: Can compare allocations with a vector-based pool for basic types but a vector won't
  // work when the template type has no default constructor
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    std::vector<Point> pool;
    for (auto i = 0; i < 1000; i++)
    {
      pool.resize(pool_size);
      pool.clear();
    }
  }
}


static void benchmark_point_with_memory_pool(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    MemoryPool<Point> pool(pool_size);
    Point* block_pt = nullptr;
    for (auto i = 0; i < pool_size; i++)
    {
      block_pt = pool.new_block_pt();
      block_pt->x = i;
      block_pt->y = i + 1;
      block_pt->z = i + 2;
    }
  }
}


static void benchmark_base1_with_memory_pool(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    MemoryPool<Base1> pool(pool_size);
    Base1* block_pt = nullptr;
    for (auto i = 0; i < pool_size; i++)
    {
      block_pt = pool.new_block_pt();
      auto v = block_pt->GetNumber();
    }
  }
}


static void benchmark_base2_with_memory_pool(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    MemoryPool<Base2> pool(pool_size);
    Base2* block_pt = nullptr;
    for (auto i = 0; i < pool_size; i++)
    {
      block_pt = pool.new_block_pt();
      auto v = block_pt->GetNumber();
    }
  }
}


static void benchmark_derived_with_memory_pool(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    MemoryPool<Derived> pool(pool_size);
    Derived* block_pt = nullptr;
    for (auto i = 0; i < pool_size; i++)
    {
      block_pt = pool.new_block_pt();
      auto v1 = block_pt->GetNumber1();
      auto v2 = block_pt->GetNumber2();
      auto v3 = block_pt->GetNumber3();
      block_pt->p.x = i;
      block_pt->p.y = i + 1;
      block_pt->p.z = i + 2;
    }
  }
}


static void benchmark_derived_with_vector(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    std::vector<Derived> pool(pool_size);
    Derived* block_pt = nullptr;
    for (auto i = 0; i < pool_size; i++)
    {
      // pool.emplace_back(Derived());
      block_pt = &pool[i];
      auto v1 = block_pt->GetNumber1();
      auto v2 = block_pt->GetNumber2();
      auto v3 = block_pt->GetNumber3();
      block_pt->p.x = i;
      block_pt->p.y = i + 1;
      block_pt->p.z = i + 2;
    }
  }
}


static void benchmark_derived_random_allocations_and_deallocations_with_memory_pool(
  benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    MemoryPool<Derived> pool(pool_size);
    std::vector<Derived*> block_pointers(pool_size);

    // Allocate all blocks
    for (auto i = 0; i < pool_size; i++) block_pointers[i] = pool.new_block_pt();

    // Shuffle the pointers so we deallocate/allocate in a random order
    auto rng = std::default_random_engine{};
    std::shuffle(block_pointers.begin(), block_pointers.end(), rng);

    // Complete several rounds of random allocation/deallocation
    for (auto round = 0; round < 100; round++)
    {
      for (auto i = 0; i < pool_size; i++)
      {
        pool.delete_block_pt(block_pointers[i]);
      }
      for (auto i = 0; i < pool_size; i++)
      {
        block_pointers[i] = pool.new_block_pt();
      }
    }
  }
}


static void benchmark_no_default_constructor_with_memory_pool(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    MemoryPool<NoDefaultConstructor> pool(pool_size);
    NoDefaultConstructor* block_pt = nullptr;
    for (auto i = 0; i < pool_size; i++)
    {
      block_pt = pool.new_block_pt(NoDefaultConstructor(i));
      auto v = block_pt->GetNumber();
    }
  }
}


static void benchmark_table_pool_creation(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    MemoryPool<Derived> pool(pool_size);
  }
  state.SetComplexityN(state.range(0));
}


static void benchmark_table_pool_destruction(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    MemoryPool<Derived> pool(pool_size);
    state.ResumeTiming();
    pool.clear();
  }
  state.SetComplexityN(state.range(0));
}


static void benchmark_table_pool_block_allocation(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    MemoryPool<Derived> pool(pool_size);
    state.ResumeTiming();
    for (auto i = 0; i < pool_size; i++)
    {
      pool.new_block_pt();
    }
  }
  state.SetComplexityN(state.range(0));
}

static void benchmark_table_pool_block_deallocation(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();
    MemoryPool<Derived> pool(pool_size);
    std::vector<Derived*> block_pointers(pool_size);
    for (auto i = 0; i < pool_size; i++)
    {
      block_pointers[i] = pool.new_block_pt();
    }
    state.ResumeTiming();

    for (auto i = 0; i < pool_size; i++)
    {
      pool.delete_block_pt(block_pointers[i]);
    }
  }
  state.SetComplexityN(state.range(0));
}

static void benchmark_table_pool_random_block_allocations(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();

    MemoryPool<Derived> pool(pool_size);
    std::vector<Derived*> block_pointers(pool_size);

    // Allocate all blocks
    for (auto i = 0; i < pool_size; i++) block_pointers[i] = pool.new_block_pt();

    // Shuffle the pointers so we deallocate/allocate in a random order
    auto rng = std::default_random_engine{};
    std::shuffle(block_pointers.begin(), block_pointers.end(), rng);

    state.ResumeTiming();

    // Complete several rounds of random allocation/deallocation
    for (auto round = 0; round < 100; round++)
    {
      state.PauseTiming();
      for (auto i = 0; i < pool_size; i++)
      {
        pool.delete_block_pt(block_pointers[i]);
      }
      state.ResumeTiming();

      for (auto i = 0; i < pool_size; i++)
      {
        block_pointers[i] = pool.new_block_pt();
      }
    }
  }
  state.SetComplexityN(state.range(0));
}

static void benchmark_table_pool_random_block_deallocations(benchmark::State& state)
{
  const auto& pool_size = state.range(0);
  for (auto _ : state)
  {
    state.PauseTiming();

    MemoryPool<Derived> pool(pool_size);
    std::vector<Derived*> block_pointers(pool_size);

    // Allocate all blocks
    for (auto i = 0; i < pool_size; i++) block_pointers[i] = pool.new_block_pt();

    // Shuffle the pointers so we deallocate/allocate in a random order
    auto rng = std::default_random_engine{};
    std::shuffle(block_pointers.begin(), block_pointers.end(), rng);

    state.ResumeTiming();

    // Complete several rounds of random allocation/deallocation
    for (auto round = 0; round < 100; round++)
    {
      for (auto i = 0; i < pool_size; i++)
      {
        pool.delete_block_pt(block_pointers[i]);
      }

      state.PauseTiming();
      for (auto i = 0; i < pool_size; i++)
      {
        block_pointers[i] = pool.new_block_pt();
      }
      state.ResumeTiming();
    }
  }
  state.SetComplexityN(state.range(0));
}


// Register the benchmarking functions as a benchmark. The Arg(...) arguments are passed in the
// benchmark::Start object
BENCHMARK(benchmark_point_multiple_pool_allocations_with_memory_pool)
  ->Arg(8)
  ->Arg(32)
  ->Arg(128)
  ->Arg(512);
BENCHMARK(benchmark_point_multiple_pool_allocations_with_vector)
  ->Arg(8)
  ->Arg(32)
  ->Arg(128)
  ->Arg(512);
BENCHMARK(benchmark_point_with_memory_pool)->Arg(8)->Arg(32)->Arg(128)->Arg(512);
BENCHMARK(benchmark_base1_with_memory_pool)->Arg(8)->Arg(32)->Arg(128)->Arg(512);
BENCHMARK(benchmark_base2_with_memory_pool)->Arg(8)->Arg(32)->Arg(128)->Arg(512);
BENCHMARK(benchmark_derived_with_memory_pool)->Arg(8)->Arg(32)->Arg(128)->Arg(512);
BENCHMARK(benchmark_derived_with_vector)->Arg(8)->Arg(32)->Arg(128)->Arg(512);
BENCHMARK(benchmark_derived_random_allocations_and_deallocations_with_memory_pool)
  ->Arg(8)
  ->Arg(32)
  ->Arg(128)
  ->Arg(512)
  ->Arg(1000);
BENCHMARK(benchmark_no_default_constructor_with_memory_pool)->Arg(8)->Arg(32)->Arg(128)->Arg(512);
BENCHMARK(benchmark_table_pool_creation)->Arg(8)->Arg(32)->Arg(128)->Arg(512)->Complexity();
BENCHMARK(benchmark_table_pool_destruction)->Arg(8)->Arg(32)->Arg(128)->Arg(512)->Complexity();
BENCHMARK(benchmark_table_pool_block_allocation)->Arg(8)->Arg(32)->Arg(128)->Arg(512)->Complexity();
BENCHMARK(benchmark_table_pool_block_deallocation)
  ->Arg(8)
  ->Arg(32)
  ->Arg(128)
  ->Arg(512)
  ->Complexity();
BENCHMARK(benchmark_table_pool_random_block_allocations)
  ->Arg(8)
  ->Arg(32)
  ->Arg(128)
  ->Arg(512)
  ->Complexity();
BENCHMARK(benchmark_table_pool_random_block_deallocations)
  ->Arg(8)
  ->Arg(32)
  ->Arg(128)
  ->Arg(512)
  ->Complexity();


// Run the benchmark
BENCHMARK_MAIN();
