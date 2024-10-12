#include "memory_pool.h"
#include "ExampleClasses.h"
#include <iostream>

using memory_pool::MemoryPool;

void run_point()
{
  // Create
  MemoryPool<Point> pool(100);
  std::cout << "\nPoint pool size: " << pool.size() << std::endl;
}

void run_base1()
{
  MemoryPool<Base1> pool(memory_pool::g_MaxNumberOfObjectsInPool);
  std::cout << "\nBase1 allocated pool size: " << pool.size() << std::endl;
}

void run_base2()
{
  MemoryPool<Base2> pool;
  std::cout << "\nBase2 unallocated pool size: " << pool.size() << std::endl;
  pool.allocate(10);
  std::cout << "Base2 allocated pool size: " << pool.size() << std::endl;
}

void run_derived()
{
  std::cout << "\nDerived object size: " << sizeof(Derived) << std::endl;

  MemoryPool<Derived> pool;

  std::cout << "Derived unallocated pool size: " << pool.size() << std::endl;

  pool.allocate(memory_pool::g_MaxNumberOfObjectsInPool);

  std::cout << "Derived allocated pool size: " << pool.size() << std::endl;

  Derived* block_pt = pool.new_block_pt(Derived());

  std::cout << "Derived allocated block addr: " << block_pt << std::endl;
}

void run_no_default_constructor()
{
  std::cout << "\nNoDefaultConstructor object size: " << sizeof(NoDefaultConstructor) << std::endl;

  MemoryPool<NoDefaultConstructor> pool(2);

  NoDefaultConstructor* block1_pt = pool.new_block_pt(NoDefaultConstructor(5));
  std::cout << "NoDefaultConstructor block1_pt addr: " << block1_pt << std::endl;
  std::cout << "NoDefaultConstructor block #1 value: " << block1_pt->GetNumber() << " == 5?"
            << std::endl;

  NoDefaultConstructor* block2_pt = pool.new_block_pt();
  *block2_pt = NoDefaultConstructor(19);
  std::cout << "NoDefaultConstructor block2_pt addr: " << block2_pt << std::endl;
  std::cout << "NoDefaultConstructor block #2 value: " << block2_pt->GetNumber() << " == 19?"
            << std::endl;
}


int main()
{
  run_point();
  // run_base1(); // Will throw an assert error
  run_base2();
  run_derived();
  run_no_default_constructor();
}