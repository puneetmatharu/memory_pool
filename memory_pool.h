#ifndef ARM_CPP_TEST_MEMORY_POOL_HEADER
#define ARM_CPP_TEST_MEMORY_POOL_HEADER

#include <cstdint>
#include <memory>
#include <queue>

namespace memory_pool
{
  using Byte = std::byte;
  using SizeT = uint64_t;

  const SizeT g_MaxNumberOfObjectsInPool = 1000;

  template<class T>
  class MemoryPool
  {
  public:
    MemoryPool() : Pool_pt(nullptr), Max_objects(0), Free_blocks() {}
    MemoryPool(const SizeT& max_object) : MemoryPool() { create_pool(max_object); }
    ~MemoryPool() { destroy_pool(); }

    void create_pool(const SizeT& max_object = g_MaxNumberOfObjectsInPool);
    void destroy_pool();

    T* allocate_object();
    T* allocate_object(T&& obj);
    void deallocate_object(T& obj) { deallocate_object(&obj); }
    void deallocate_object(const T* const obj_pt);

    inline SizeT size() { return Max_objects * sizeof(T); }

  private:
    inline Byte* start() { return Pool_pt; }
    inline Byte* end() { return Pool_pt + size(); }

    bool in_pool(const T& obj) { return in_pool(&obj); }
    bool in_pool(const T* const obj_pt);

    Byte* get_address_of_nth_block(const SizeT& block_index);
    SizeT get_position_in_pool(const T& obj) { return get_position_in_pool(&obj); }
    SizeT get_position_in_pool(const T* const obj_pt);

    void check_pool_has_available_space();

    Byte* Pool_pt;
    SizeT Max_objects;
    std::queue<SizeT> Free_blocks;
  };

} // namespace memory_pool

#include "memory_pool.tpp"

#endif // ARM_CPP_TEST_MEMORY_POOL_HEADER