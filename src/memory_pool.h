#ifndef MEMORY_POOL_MEMORY_POOL_HEADER
#define MEMORY_POOL_MEMORY_POOL_HEADER

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>

namespace memory_pool
{
  /****************************************************************************************
   * @brief Type aliases
   *
   ****************************************************************************************/
  using Byte = std::byte;
  using SizeT = uint64_t;

  /****************************************************************************************
   * @brief Maximum number of objects in any pool
   *
   ****************************************************************************************/
  const SizeT g_MaxNumberOfObjectsInPool = 1000;

  /****************************************************************************************
   * @brief Memory pool class
   *
   * @tparam T:
   ****************************************************************************************/
  class PoolAllocationTracker
  {
  public:
    PoolAllocationTracker() : Blocks() {}
    PoolAllocationTracker(const SizeT& max_object) : PoolAllocationTracker() {}
    ~PoolAllocationTracker() { Blocks = {}; }

    void setup(const SizeT& max_object)
    {
      for (SizeT i = 0; i < max_object; i++) Blocks.push(i);
    }
    void clear() { Blocks = {}; }
    inline SizeT size() { return Blocks.size(); }
    inline void push(SizeT pos) { Blocks.push(std::move(pos)); }
    inline SizeT pop()
    {
      SizeT index = Blocks.top();
      Blocks.pop();
      return index;
    }

  private:
    std::stack<SizeT> Blocks;
  };

  /****************************************************************************************
   * @brief Memory pool class
   *
   * @tparam T:
   ****************************************************************************************/
  template<class T>
  class MemoryPool
  {
  public:
    MemoryPool() : Pool_pt(nullptr), Max_objects(0), Free_blocks_tracker() {}
    MemoryPool(const SizeT& max_object) : MemoryPool() { allocate(max_object); }
    ~MemoryPool() { clear(); }

    void allocate(const SizeT& max_object = g_MaxNumberOfObjectsInPool);
    void clear();

    T* new_block_pt();
    T* new_block_pt(T&& obj);
    void delete_block_pt(T*& obj_pt);

    inline SizeT size() { return Max_objects; }
    inline SizeT available_capacity() { return Free_blocks_tracker.size(); }

  private:
    bool is_pool_member(const T* const obj_pt);

    inline SizeT size_in_bytes() { return Max_objects * sizeof(T); }
    inline Byte* start() { return Pool_pt; }
    inline Byte* end() { return Pool_pt + this->size_in_bytes(); }

    void check_pool_has_available_space();

    Byte* Pool_pt;
    SizeT Max_objects;
    PoolAllocationTracker Free_blocks_tracker;
  };

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   * @param max_object:
   ****************************************************************************************/
  template<class T>
  void MemoryPool<T>::allocate(const SizeT& max_object)
  {
    if (max_object > g_MaxNumberOfObjectsInPool) throw std::bad_alloc();
    assert(Pool_pt == nullptr);
    assert(Free_blocks_tracker.size() == 0);
    Max_objects = max_object;
    Pool_pt = new Byte[this->size_in_bytes()];
    Free_blocks_tracker.setup(Max_objects);
  }

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   ****************************************************************************************/
  template<class T>
  void MemoryPool<T>::clear()
  {
    delete[] Pool_pt;
    Pool_pt = nullptr;
    Max_objects = 0;
    Free_blocks_tracker.clear();
  }

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   * @return T*:
   ****************************************************************************************/
  template<class T>
  T* MemoryPool<T>::new_block_pt()
  {
    check_pool_has_available_space();
    const auto index = Free_blocks_tracker.pop();
    T* block_pt = static_cast<T*>((void*)this->start()) + index;
    return block_pt;
  }

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   * @param obj:
   * @return T*:
   ****************************************************************************************/
  template<class T>
  T* MemoryPool<T>::new_block_pt(T&& obj)
  {
    check_pool_has_available_space();
    T* block_pt = new_block_pt();
    *block_pt = std::move(obj);
    return block_pt;
  }

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   * @param obj_pt:
   ****************************************************************************************/
  template<class T>
  void MemoryPool<T>::delete_block_pt(T*& obj_pt)
  {
    assert(is_pool_member(*obj_pt));
    auto byte_pt = static_cast<const Byte* const>((void*)obj_pt);
    SizeT pos = (byte_pt - this->start()) / sizeof(T);
    Free_blocks_tracker.push(pos);
    obj_pt = nullptr;
  }

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   * @param obj_pt:
   * @return true:
   * @return false:
   ****************************************************************************************/
  template<class T>
  bool MemoryPool<T>::is_pool_member(const T* const obj_pt)
  {
    std::ptrdiff_t offset = static_cast<const Byte* const>(obj_pt) - this->start();
    if (offset < 0) return false;
    if (offset > ((Max_objects - 1) * sizeof(T))) return false;
    if (offset % sizeof(T) != 0) return false;
    return true;
  }

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   ****************************************************************************************/
  template<class T>
  void MemoryPool<T>::check_pool_has_available_space()
  {
    if (Free_blocks_tracker.size() > 0) return;
    throw std::out_of_range("No more space available; all " + std::to_string(Max_objects) +
                            " blocks allocated!");
  }
} // namespace memory_pool

#endif // MEMORY_POOL_MEMORY_POOL_HEADER