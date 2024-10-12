#include <iostream>
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
  const unsigned mid = unsigned(desired_pool_size / 2);

  // Create pool of size 'desired_pool_size' templated by your container
  MemoryPool<MyStruct> pool(desired_pool_size);

  // Option 1: Assign to pointer after allocation
  for (unsigned i = 0; i < mid; i++)
  {
    auto* obj_pt = pool.new_block_pt();
    *obj_pt = MyStruct{1, 0.0, 42.0, -9};
  }

  // Option 2: Assign data by moving rvalue during allocation
  for (unsigned i = mid; i < desired_pool_size; i++)
  {
    auto* obj_pt = pool.new_block_pt(MyStruct{1, 0.0, 42.0, -9});
  }

  // Do not try to allocate space than you have. Following line will lead to out of range error
  std::cout << "\nAttempting to allocate memory when the pool is already full."
            << "\nExpect an std::out_of_range error.\n"
            << std::endl;
  pool.new_block_pt(MyStruct{1, 0.0, 42.0, -9});
}