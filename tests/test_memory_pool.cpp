#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ExampleClasses.h"
#include "memory_pool.h"


using memory_pool::MemoryPool;


TEST_CASE("Point")
{
  MemoryPool<Point> pool(100);
  REQUIRE(pool.size() == 100);
}


TEST_CASE("Base1")
{
  SUBCASE("Cannot allocate more than 'g_MaxNumberOfObjectsInPool' objects")
  {
    CHECK_THROWS_AS(MemoryPool<Base1>(memory_pool::g_MaxNumberOfObjectsInPool + 1), std::bad_alloc);
  }
}


TEST_CASE("Base2")
{
  MemoryPool<Base2> pool;
  pool.allocate(10);
  REQUIRE(pool.size() == 10);
}


TEST_CASE("Derived")
{
  const auto& num_objects = memory_pool::g_MaxNumberOfObjectsInPool;
  MemoryPool<Derived> pool(num_objects);
  REQUIRE(pool.size() == num_objects);

  auto* block_pt = pool.new_block_pt(Derived());

  SUBCASE("Allocating a block reduces the available capacity")
  {
    REQUIRE(pool.available_capacity() == num_objects - 1);
    REQUIRE(pool.size() == num_objects);
  }

  SUBCASE("After deallocating a block from a pointer, the pointer is nulled out")
  {
    pool.delete_block_pt(block_pt);
    CHECK(block_pt == nullptr);
    REQUIRE(pool.available_capacity() == num_objects);
    REQUIRE(pool.size() == num_objects);
  }
}


TEST_CASE("NoDefaultConstructor")
{
  MemoryPool<NoDefaultConstructor> pool(2);
  REQUIRE(pool.size() == 2);
  REQUIRE(pool.available_capacity() == 2);

  NoDefaultConstructor* block1_pt = pool.new_block_pt(NoDefaultConstructor(5));

  SUBCASE("Check object assigned during block allocation is valid")
  {
    CHECK(block1_pt->GetNumber() == 5);
    CHECK(pool.available_capacity() == 1);
  }

  NoDefaultConstructor* block2_pt = pool.new_block_pt();
  *block2_pt = NoDefaultConstructor(19);

  SUBCASE("Check object assigned after block allocation is valid")
  {
    CHECK(pool.available_capacity() == 0);
    CHECK(block2_pt->GetNumber() == 19);
  }

  SUBCASE("Cannot allocate more than is available")
  {
    REQUIRE(pool.available_capacity() == 0);
    CHECK_THROWS_AS(pool.new_block_pt(NoDefaultConstructor(3)), std::out_of_range);
  }
}
