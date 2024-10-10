#include "memory_pool.h"
#include "ExampleClasses.h"
#include <iostream>

using memory_pool::MemoryPool;

// TODO:
// 1. Add GitHub tests for all platforms; make sure compile-error-free
// 2. Add 'verbose' mode? To print pool statistics/info
// 2. Make appropriate functions private in memory_pool
// 3. Separate into header and .cpp file
// 4. Add macros to disable safety checks
// 5. CHECK MOVE IN ALLOCATE_BLOCK WORKS
// 6. Compare speed to general memory allocation?
// 7. Add option to zero out the blocks where we deallocate?
// 8. Use RAII to make thread-safe? NOT NECESSARY

void test_point()
{
  // Create
  MemoryPool<Point> pool(100);
  std::cout << "\nPoint pool size: " << pool.size() << std::endl;
}

void test_base1()
{
  // Uncomment and run to produce an error
  MemoryPool<Base1> pool(memory_pool::g_MaxNumberOfObjectsInPool + 1);
}

void test_base2()
{
  MemoryPool<Base2> pool;
  std::cout << "\nBase2 unallocated pool size: " << pool.size() << std::endl;
  pool.create_pool(10);
  std::cout << "Base2 allocated pool size: " << pool.size() << std::endl;
}

void test_derived()
{
  std::cout << "\nDerived object size: " << sizeof(Derived) << std::endl;

  MemoryPool<Derived> pool;

  std::cout << "Derived unallocated pool size: " << pool.size() << std::endl;

  pool.create_pool(memory_pool::g_MaxNumberOfObjectsInPool);

  std::cout << "Derived allocated pool size: " << pool.size() << std::endl;

  Derived* block_pt = pool.allocate_object(Derived());

  std::cout << "Derived allocated block addr: " << block_pt << std::endl;
}

void test_no_default_constructor()
{
  std::cout << "\nNoDefaultConstructor object size: " << sizeof(NoDefaultConstructor) << std::endl;

  MemoryPool<NoDefaultConstructor> pool(2);

  NoDefaultConstructor* block1_pt = pool.allocate_object(NoDefaultConstructor(5));
  std::cout << "NoDefaultConstructor block1_pt addr: " << block1_pt << std::endl;
  std::cout << "NoDefaultConstructor block #1 value: " << block1_pt->GetNumber() << " == 5?"
            << std::endl;

  NoDefaultConstructor* block2_pt = pool.allocate_object();
  *block2_pt = NoDefaultConstructor(19);
  std::cout << "NoDefaultConstructor block2_pt addr: " << block2_pt << std::endl;
  std::cout << "NoDefaultConstructor block #2 value: " << block2_pt->GetNumber() << " == 19?"
            << std::endl;
}


int main()
{
  test_point();
  // test_base1(); // Will throw an assert error
  test_base2();
  test_derived();
  test_no_default_constructor();
}