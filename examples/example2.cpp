#include "memory_pool.h"

using memory_pool::MemoryPool;

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

  // Create pool of size 'desired_pool_size'
  MemoryPool<MyStruct> pool(desired_pool_size);

  // Option 1: Assigned to pointer after allocation
  for (unsigned i = 0; i < mid; i++)
  {
    auto* obj_pt = pool.allocate_object();
    *obj_pt = MyStruct{1, 0.0, 42.0, -9};
  }

  // Option 2: Assign by passing rvalue to allocate_object() function
  for (unsigned i = mid; i < desired_pool_size; i++)
  {
    auto* obj_pt = pool.allocate_object(MyStruct{1, 0.0, 42.0, -9});
  }

  // Do not try to allocate space than you have. Following line will lead to out of range error
  pool.allocate_object(MyStruct{1, 0.0, 42.0, -9});
}