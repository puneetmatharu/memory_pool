#ifndef ARM_CPP_TEST_MEMORY_POOL_TEMPLATE_HEADER
#define ARM_CPP_TEST_MEMORY_POOL_TEMPLATE_HEADER

#ifndef ARM_CPP_TEST_MEMORY_POOL_HEADER
#error __FILE__ should only be included from memory_pool.h
#endif // ARM_CPP_TEST_MEMORY_POOL_HEADER

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>

#include "memory_pool.h"

namespace memory_pool
{
  /****************************************************************************************
   * @brief
   *
   * @tparam T:
   * @param max_object:
   ****************************************************************************************/
  template<class T>
  void MemoryPool<T>::create_pool(const SizeT& max_object)
  {
    assert(max_object <= g_MaxNumberOfObjectsInPool);
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
  void MemoryPool<T>::deallocate_object(const T* const obj_pt)
  {
    assert(in_pool(obj_pt));
    auto pos = get_position_in_pool(obj_pt);
    Free_blocks.push(pos);
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
    SizeT pool_size = this->size();
    if ((obj_pt >= this->start()) && (obj_pt < this->end())) return true;
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
    return (obj_pt - Pool_pt) / sizeof(T);
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

#endif // ARM_CPP_TEST_MEMORY_POOL_TEMPLATE_HEADER