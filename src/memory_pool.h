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
   * @brief Tracks the blocks in the pool that can be allocated to.
   *
   ****************************************************************************************/
  class BlockTracker
  {
  public:
    BlockTracker() : Blocks() {}
    BlockTracker(const SizeT& num_blocks) : BlockTracker() {}
    ~BlockTracker() { Blocks = {}; }

    void setup(const SizeT& num_blocks);
    void clear();
    inline SizeT size() { return Blocks.size(); }
    inline void push(SizeT block_index);
    inline SizeT pop();

  private:
    SizeT Num_blocks;
    std::stack<SizeT> Blocks;
  };

  /****************************************************************************************
   * @brief Allocates an index for each of the blocks.
   *
   * @param num_blocks: The number of blocks to track.
   ****************************************************************************************/
  void BlockTracker::setup(const SizeT& num_blocks)
  {
    Num_blocks = num_blocks;
    for (SizeT i = 0; i < Num_blocks; i++) Blocks.push(i);
  }

  /****************************************************************************************
   * @brief Clears up any data used for the block tracking.
   *
   ****************************************************************************************/
  void BlockTracker::clear()
  {
    Blocks = {};
    Num_blocks = 0;
  }

  /****************************************************************************************
   * @brief Adds a block back to the set of values to be tracked.
   *
   * @param block_index: The index of the block to be stored.
   ****************************************************************************************/
  void BlockTracker::push(SizeT block_index)
  {
    assert(block_index < Num_blocks);
    Blocks.push(std::move(block_index));
  }

  /****************************************************************************************
   * @brief Returns the index of a block from the set of blocks to be tracked.
   *
   ****************************************************************************************/
  SizeT BlockTracker::pop()
  {
    SizeT index = Blocks.top();
    Blocks.pop();
    return index;
  }

  /****************************************************************************************
   * @brief The MemoryPool class. A generic memory pool that provides quick memory
   *        allocation/deallocation for objects of a given type.
   *
   * @tparam T: The type of the objects to be allocated for in the memory pool.
   ****************************************************************************************/
  template<class T>
  class MemoryPool
  {
  public:
    // Default constructor. Initialises an empty pool. You must call allocate() separately
    // to create the pool
    MemoryPool() : Pool_pt(nullptr), Pool_size(0), Free_blocks_tracker() {}

    // Immediately creates a pool for 'num_blocks' objects of type T. The argument must
    // not exceed the value of 'g_MaxNumberOfObjectsInPool' in the 'memory_pool' namespace
    MemoryPool(const SizeT& num_blocks) : MemoryPool() { allocate(num_blocks); }

    // Destructor. Handles the clean-up
    ~MemoryPool() { clear(); }

    // Allocate space for 'num_blocks' objects of type T
    void allocate(const SizeT& num_blocks = g_MaxNumberOfObjectsInPool);

    // Clean up
    void clear();

    // Returns a pointer to an available block in the memory pool
    T* new_block_pt();

    // Returns a pointer to an available block in the memory pool and assigns 'obj' to the
    // location addressed by the pointer
    T* new_block_pt(T&& obj);

    // "Deletes" the data pointed to by 'obj_pt' and nullifies the input pointer. Do not try
    // to access obj_pt after this function has been called
    void delete_block_pt(T*& obj_pt);

    // The total number of objects this pool can hold
    inline SizeT size() { return Pool_size; }

    // The remaining number of objects this pool can hold
    inline SizeT available_capacity() { return Free_blocks_tracker.size(); }

  private:
    // Returns true if obj_pt points to an object of type T in the pool. Returns false otherwise
    bool is_pool_member(const T* const obj_pt);

    // Computes the number of bytes allocated in the pool for the objects of type T
    inline SizeT size_in_bytes() { return Pool_size * sizeof(T); }

    // Returns a pointer to the start of the memory block used for the memory pool
    inline Byte* start() { return Pool_pt; }

    // Returns a (off-the-end) pointer that points to the first byte after the memory block
    // used for the memory pool
    inline Byte* end() { return Pool_pt + this->size_in_bytes(); }

    // Checks if there is any more available space in the pool and throws if there is no
    // more space available. Does nothing otherwise
    void throw_if_pool_has_no_more_available_space();

    // A pointer to the underlying block of memory used for the memory pool
    Byte* Pool_pt;

    // The number of objects this pool can hold
    SizeT Pool_size;

    // Tracks the blocks in the pool that can be allocated to
    BlockTracker Free_blocks_tracker;
  };

  /****************************************************************************************
   * @brief Allocate space for 'num_blocks' objects of type T.
   *
   * @param num_blocks: A positive integer indicating the number of objects the pool should
   *                    be capable of holding; must not exceed 'g_MaxNumberOfObjectsInPool'
   ****************************************************************************************/
  template<class T>
  void MemoryPool<T>::allocate(const SizeT& num_blocks)
  {
    if (num_blocks > g_MaxNumberOfObjectsInPool) throw std::bad_alloc();
    assert(Pool_pt == nullptr);
    assert(Free_blocks_tracker.size() == 0);
    Pool_size = num_blocks;
    Pool_pt = new Byte[this->size_in_bytes()];
    Free_blocks_tracker.setup(Pool_size);
  }

  /****************************************************************************************
   * @brief Cleans up any memory used for the memory pool.
   *
   ****************************************************************************************/
  template<class T>
  void MemoryPool<T>::clear()
  {
    delete[] Pool_pt;
    Pool_pt = nullptr;
    Pool_size = 0;
    Free_blocks_tracker.clear();
  }

  /****************************************************************************************
   * @brief Returns a pointer to an available block in the memory pool.
   *
   * @return T*: A pointer to a new object in the memory pool.
   ****************************************************************************************/
  template<class T>
  T* MemoryPool<T>::new_block_pt()
  {
    throw_if_pool_has_no_more_available_space();
    const auto index = Free_blocks_tracker.pop();
    T* block_pt = static_cast<T*>((void*)this->start()) + index;
    return block_pt;
  }


  /****************************************************************************************
   * @brief Returns a pointer to an available block in the memory pool and assigns 'obj' to
   *        the location addressed by the pointer.
   *
   * @param obj: The input to assign to the new memory block. If the input is an lvalue, it
   *             will be copied to the new memory block. If an rvalue object then it will
   *             be moved to the new memory block.
   * @return T*: A pointer to a new object in the memory pool.
   ****************************************************************************************/
  template<class T>
  T* MemoryPool<T>::new_block_pt(T&& obj)
  {
    throw_if_pool_has_no_more_available_space();
    T* block_pt = new_block_pt();
    *block_pt = std::move(obj);
    return block_pt;
  }

  /****************************************************************************************
   * @brief "Deletes" the data pointed to by 'obj_pt' and nullifies the input pointer. Do
   *        not try to access obj_pt after this function has been called.
   *
   *        NOTE: The underlying block in memory pointed to 'obj_pt' is not wiped. It is
   *        simply added back to the memory pool for use by another object.
   *
   * @param obj_pt: A reference to the pointer to the underlying block in the memory pool.
   *                Will be set to 'nullptr' after the underlying data has been deallocated.
   ****************************************************************************************/
  template<class T>
  void MemoryPool<T>::delete_block_pt(T*& obj_pt)
  {
    assert(is_pool_member(obj_pt));
    auto byte_pt = static_cast<const Byte* const>((void*)obj_pt);
    SizeT pos = (byte_pt - this->start()) / sizeof(T);
    Free_blocks_tracker.push(pos);
    obj_pt = nullptr;
  }

  /****************************************************************************************
   * @brief Returns true if 'obj_pt' points to an object of type T in the pool. Returns
   *        false otherwise.
   *
   * @param obj_pt: A pointer to an object of type T.
   * @return true: If 'obj_pt' points to an object of type T in the pool.
   * @return false: If 'obj_pt' does not point to an object of type T in the pool.
   ****************************************************************************************/
  template<class T>
  bool MemoryPool<T>::is_pool_member(const T* const obj_pt)
  {
    std::ptrdiff_t offset = static_cast<const Byte* const>(obj_pt) - this->start();
    if (offset < 0) return false;
    if (offset > ((Pool_size - 1) * sizeof(T))) return false;
    if (offset % sizeof(T) != 0) return false;
    return true;
  }

  /****************************************************************************************
   * @brief Computes the number of bytes allocated in the pool for the objects of type T
   *
   ****************************************************************************************/
  template<class T>
  void MemoryPool<T>::throw_if_pool_has_no_more_available_space()
  {
    if (Free_blocks_tracker.size() > 0) return;
    throw std::out_of_range("No more space available; all " + std::to_string(Pool_size) +
                            " blocks allocated!");
  }
} // namespace memory_pool

#endif // MEMORY_POOL_MEMORY_POOL_HEADER