#ifndef HG_COLT_GLOBAL_ALLOC
#define HG_COLT_GLOBAL_ALLOC

#include "./composable_alloc.h"

namespace clt::mem
{
  /// @brief The global allocator
  inline AbortOnNULLAllocator<
    Segregator<1_KiB,
    ThreadSafeAllocator<Segregator<256, FreeList<StackAllocator<8_KiB, 16>, 16_B, 256_B, 32>, FreeList<Mallocator, 256_B, 1_KiB, 32>>>,
    ThreadSafeAllocator<FreeList<Mallocator, 1_KiB, 4_KiB, 32>>
    >> GlobalAllocator;

  /// @brief Allocate a block of memory through the global allocator
  /// @param sz The size of the block
  /// @return A MemBlock that is NEVER null
  static MemBlock alloc(size<Byte> sz) noexcept
  {
    return GlobalAllocator.alloc(sz);
  }

  /// @brief Deallocate a block of memory through the global allocator.
  /// This function does not accept a 'nullblk' as 'alloc' never returns a 'nullblk'.
  /// @param blk The block to deallocate
  static void dealloc(meta::for_debug_for_release_t<MemBlock&, MemBlock> blk) noexcept
    COLT_PRE(!blk.is_null())
  {
    GlobalAllocator.dealloc(blk);
    if constexpr (is_debug())
      blk = nullblk; //to avoid reuse of freed block
  }
  COLT_POST()
}

#endif //!HG_COLT_GLOBAL_ALLOC