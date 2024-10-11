#include <algorithm>
#include <random>
#include <benchmark/benchmark.h>
#include "ExampleClasses.h"
#include "memory_pool.h"


using memory_pool::MemoryPool;


static void benchmark_point_multiple_pool_allocations_with_memory_pool(benchmark::State& state)
{
  for (auto _ : state)
  {
    const auto& pool_size = state.range(0);
    MemoryPool<Point> pool;
    for (auto i = 0; i < 1000; i++)
    {
      pool.create_pool(pool_size);
      pool.destroy_pool();
    }
  }
}


static void benchmark_point_multiple_pool_allocations_with_vector(benchmark::State& state)
{
  // NOTE: Can compare allocations with a vector-based pool for basic types but a vector won't
  // work when the template type has no default constructor
  for (auto _ : state)
  {
    const auto& pool_size = state.range(0);
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
  for (auto _ : state)
  {
    const auto& pool_size = state.range(0);
    MemoryPool<Point> pool(pool_size);
    Point* block_pt = nullptr;
    for (auto i = 0; i < pool_size; i++)
    {
      block_pt = pool.allocate_object();
      block_pt->x = i;
      block_pt->y = i + 1;
      block_pt->z = i + 2;
    }
  }
}


static void benchmark_base1_with_memory_pool(benchmark::State& state)
{
  for (auto _ : state)
  {
    const auto& pool_size = state.range(0);
    MemoryPool<Base1> pool(pool_size);
    Base1* block_pt = nullptr;
    for (auto i = 0; i < pool_size; i++)
    {
      block_pt = pool.allocate_object();
      auto v = block_pt->GetNumber();
    }
  }
}


static void benchmark_base2_with_memory_pool(benchmark::State& state)
{
  for (auto _ : state)
  {
    const auto& pool_size = state.range(0);
    MemoryPool<Base2> pool(pool_size);
    Base2* block_pt = nullptr;
    for (auto i = 0; i < pool_size; i++)
    {
      block_pt = pool.allocate_object();
      auto v = block_pt->GetNumber();
    }
  }
}


static void benchmark_derived_with_memory_pool(benchmark::State& state)
{
  for (auto _ : state)
  {
    const auto& pool_size = state.range(0);
    MemoryPool<Derived> pool(pool_size);
    Derived* block_pt = nullptr;
    for (auto i = 0; i < pool_size; i++)
    {
      block_pt = pool.allocate_object();
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
  for (auto _ : state)
  {
    const auto& pool_size = state.range(0);
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
  for (auto _ : state)
  {
    const auto& pool_size = state.range(0);
    MemoryPool<Derived> pool(pool_size);
    std::vector<Derived*> block_pointers(pool_size);

    // Allocate all blocks
    for (auto i = 0; i < pool_size; i++) block_pointers[i] = pool.allocate_object();

    // Shuffle the pointers so we deallocate/allocate in a random order
    auto rng = std::default_random_engine{};
    std::shuffle(block_pointers.begin(), block_pointers.end(), rng);

    // Complete several rounds of random allocation/deallocation
    for (auto round = 0; round < 100; round++)
    {
      for (auto i = 0; i < pool_size; i++)
      {
        pool.deallocate_object(&block_pointers[i]);
      }
      for (auto i = 0; i < pool_size; i++)
      {
        block_pointers[i] = pool.allocate_object();
      }
    }
  }
}


static void benchmark_no_default_constructor_with_memory_pool(benchmark::State& state)
{
  for (auto _ : state)
  {
    const auto& pool_size = state.range(0);
    MemoryPool<NoDefaultConstructor> pool(pool_size);
    NoDefaultConstructor* block_pt = nullptr;
    for (auto i = 0; i < pool_size; i++)
    {
      block_pt = pool.allocate_object(NoDefaultConstructor(i));
      auto v = block_pt->GetNumber();
    }
  }
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


// Run the benchmark
BENCHMARK_MAIN();
