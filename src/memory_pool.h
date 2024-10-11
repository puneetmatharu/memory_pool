#ifndef MEMORY_POOL_MEMORY_POOL_HEADER
#define MEMORY_POOL_MEMORY_POOL_HEADER

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <queue>
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
    void deallocate_object(T** const obj_pt);

    inline SizeT size() { return Max_objects; }
    inline SizeT size_in_bytes() { return Max_objects * sizeof(T); }
    inline SizeT available_capacity() { return Free_blocks.size(); }

    bool in_pool(const T* const obj_pt);

  private:
    inline Byte* start() { return Pool_pt; }
    inline Byte* end() { return Pool_pt + size_in_bytes(); }

    Byte* get_address_of_nth_block(const SizeT& block_index);
    SizeT get_position_in_pool(const T& obj) { return get_position_in_pool(&obj); }
    SizeT get_position_in_pool(const T* const obj_pt);

    void check_pool_has_available_space();

    Byte* Pool_pt;
    SizeT Max_objects;
    std::queue<SizeT> Free_blocks;
  };

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   * @param max_object:
   ****************************************************************************************/
  template<class T>
  void MemoryPool<T>::create_pool(const SizeT& max_object)
  {
    if (max_object > g_MaxNumberOfObjectsInPool) throw std::bad_alloc();
    assert(Pool_pt == nullptr);
    assert(Free_blocks.size() == 0);

    Max_objects = max_object;
    SizeT pool_size_in_bytes = Max_objects * sizeof(T);
    Pool_pt = new Byte[pool_size_in_bytes];
    for (SizeT i = 0; i < Max_objects; i++)
    {
      Free_blocks.push(i);
    }
  }

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   ****************************************************************************************/
  template<class T>
  void MemoryPool<T>::destroy_pool()
  {
    delete[] Pool_pt;
    Pool_pt = nullptr;
    Max_objects = 0;
    Free_blocks = std::queue<SizeT>();
  }

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   * @return T*:
   ****************************************************************************************/
  template<class T>
  T* MemoryPool<T>::allocate_object()
  {
    check_pool_has_available_space();
    const auto index = Free_blocks.front();
    Free_blocks.pop();
    T* block_pt = static_cast<T*>((void*)get_address_of_nth_block(index));
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
  T* MemoryPool<T>::allocate_object(T&& obj)
  {
    check_pool_has_available_space();
    T* block_pt = allocate_object();
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
  void MemoryPool<T>::deallocate_object(T** const obj_pt)
  {
    assert(in_pool(*obj_pt));
    auto pos = get_position_in_pool(*obj_pt);
    Free_blocks.push(pos);
    *obj_pt = nullptr;
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
  bool MemoryPool<T>::in_pool(const T* const obj_pt)
  {
    auto byte_pt = reinterpret_cast<const Byte* const>(obj_pt);
    if ((byte_pt >= this->start()) or (byte_pt < this->end())) return true;
    return false;
  }

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   * @param block_index:
   * @return Byte*:
   ****************************************************************************************/
  template<class T>
  Byte* MemoryPool<T>::get_address_of_nth_block(const SizeT& block_index)
  {
    assert(block_index <= Max_objects);
    auto offset = block_index * sizeof(T);
    return this->start() + offset;
  }

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   * @param obj_pt:
   * @return SizeT:
   ****************************************************************************************/
  template<class T>
  SizeT MemoryPool<T>::get_position_in_pool(const T* const obj_pt)
  {
    assert(in_pool(obj_pt));
    auto byte_pt = reinterpret_cast<const Byte* const>(obj_pt);
    return (byte_pt - Pool_pt) / sizeof(T);
  }

  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   ****************************************************************************************/
  template<class T>
  void MemoryPool<T>::check_pool_has_available_space()
  {
    if (Free_blocks.size() > 0) return;
    throw std::out_of_range("No more space available; all " + std::to_string(Max_objects) +
                            " blocks allocated!");
  }
} // namespace memory_pool

#endif // MEMORY_POOL_MEMORY_POOL_HEADER