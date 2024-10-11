#include <benchmark/benchmark.h>
#include "ExampleClasses.h"
#include "memory_pool.h"


using memory_pool::MemoryPool;


static void benchmark_multiple_point_pool_allocations_with_memory_pool(benchmark::State& state)
{
  for (auto _ : state)
  {
    MemoryPool<Point> pool;
    for (auto i = 0; i < 1000; i++)
    {
      pool.create_pool(memory_pool::g_MaxNumberOfObjectsInPool);
      pool.destroy_pool();
    }
  }
}


static void benchmark_multiple_point_pool_allocations_with_vector(benchmark::State& state)
{
  // NOTE: Can compare allocations with a vector-based pool for basic types but a vector won't
  // work when the template type has no default constructor
  for (auto _ : state)
  {
    std::vector<Point> pool;
    for (auto i = 0; i < 1000; i++)
    {
      pool.resize(memory_pool::g_MaxNumberOfObjectsInPool);
      pool.clear();
    }
  }
}


static void benchmark_point_with_memory_pool(benchmark::State& state)
{
  for (auto _ : state)
  {
    MemoryPool<Point> pool(memory_pool::g_MaxNumberOfObjectsInPool);
    Point* block_pt = nullptr;
    for (auto i = 0; i < memory_pool::g_MaxNumberOfObjectsInPool; i++)
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
    MemoryPool<Base1> pool(memory_pool::g_MaxNumberOfObjectsInPool);
    Base1* block_pt = nullptr;
    for (auto i = 0; i < memory_pool::g_MaxNumberOfObjectsInPool; i++)
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
    MemoryPool<Base2> pool(memory_pool::g_MaxNumberOfObjectsInPool);
    Base2* block_pt = nullptr;
    for (auto i = 0; i < memory_pool::g_MaxNumberOfObjectsInPool; i++)
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
    MemoryPool<Derived> pool(memory_pool::g_MaxNumberOfObjectsInPool);
    Derived* block_pt = nullptr;
    for (auto i = 0; i < memory_pool::g_MaxNumberOfObjectsInPool; i++)
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

static void benchmark_no_default_constructor_with_memory_pool(benchmark::State& state)
{
  for (auto _ : state)
  {
    MemoryPool<NoDefaultConstructor> pool(memory_pool::g_MaxNumberOfObjectsInPool);
    NoDefaultConstructor* block_pt = nullptr;
    for (auto i = 0; i < memory_pool::g_MaxNumberOfObjectsInPool; i++)
    {
      block_pt = pool.allocate_object(NoDefaultConstructor(i));
      auto v = block_pt->GetNumber();
    }
  }
}


static void benchmark_derived_with_vector(benchmark::State& state)
{
  for (auto _ : state)
  {
    std::vector<Derived> pool;
    Derived* block_pt = nullptr;
    for (auto i = 0; i < memory_pool::g_MaxNumberOfObjectsInPool; i++)
    {
      pool.emplace_back(Derived());
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

// Register the function as a benchmark
BENCHMARK(benchmark_multiple_point_pool_allocations_with_memory_pool);
BENCHMARK(benchmark_multiple_point_pool_allocations_with_vector);
BENCHMARK(benchmark_point_with_memory_pool);
BENCHMARK(benchmark_base1_with_memory_pool);
BENCHMARK(benchmark_base2_with_memory_pool);
BENCHMARK(benchmark_derived_with_memory_pool);
BENCHMARK(benchmark_no_default_constructor_with_memory_pool);
BENCHMARK(benchmark_derived_with_vector);

// Run the benchmark
BENCHMARK_MAIN();
