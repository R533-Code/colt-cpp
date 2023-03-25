#ifndef HG_COLT_COMPOSABLE_ALLOC
#define HG_COLT_COMPOSABLE_ALLOC

#include "./simple_alloc.h"

namespace clt::mem
{
  template<meta::OwningAllocator Primary, meta::Allocator Fallback>
  /// @brief Allocator that tries to allocate through the Primary allocator and on failure uses the Fallback allocator
  struct FallbackAllocator
    : private Primary, private Fallback
  {
    /// @brief Allocates a MemBlock
    /// @param size The size of the allocation
    /// @return Allocated MemBlock or empty MemBlock
    MemBlock alloc(size<Byte> size) noexcept
    {
      auto blk = Primary::alloc(size);
      if (blk.is_null())
        return Fallback::alloc(size);
      return blk;
    }

    /// @brief Deallocates a MemBlock that was allocated using the current allocator
    /// @param to_free The block whose resources to free
    void dealloc(MemBlock to_free) noexcept
    {
      if (Primary::owns(to_free))
        Primary::alloc(to_free)
      else
        Fallback::alloc(to_free);
    }

    /// @brief Check if Primary or Fallback owns 'blk'
    /// @param blk The MemBlock to check
    /// @return True if 'blk' was allocated through the current allocator
    bool owns(MemBlock blk) noexcept
    {
      return Primary::owns(blk) || Fallback::owns(blk);
    }
  };

  template<size<Byte> SIZE, meta::Allocator Primary, meta::Allocator Secondary>
  /// @brief For all allocation sizes <= size, allocates through Primary, else through Secondary
  struct Segregator
    : private Primary, private Secondary
  {
    /// @brief Allocates a MemBlock
    /// @param size The size of the allocation
    /// @return Allocated MemBlock or empty MemBlock
    constexpr MemBlock alloc(size<Byte> size) noexcept
    {
      if (size.to_bytes() <= SIZE.to_bytes())
        return Primary::alloc(size);
      return Secondary::alloc(size);
    }

    /// @brief Deallocates a MemBlock that was allocated using the current allocator
    /// @param to_free The block whose resources to free
    constexpr void dealloc(MemBlock to_free) noexcept
    {
      if (to_free.size() <= SIZE.to_bytes())
        Primary::dealloc(to_free);
      else
        Secondary::dealloc(to_free);
    }

    /// @brief Check if the current allocator owns 'blk'
    /// @param blk The MemBlock to check
    /// @return True if 'blk' was allocated through the current allocator
    constexpr bool owns(MemBlock blk) noexcept
    {
      return Primary::owns(blk) || Secondary::owns(blk);
    }
  };

  template<meta::Allocator allocator, typename Prefix, typename Suffix>
  struct AffixAllocator
    : private allocator
  {
    /// @brief Allocates a MemBlock
    /// @param size The size of the allocation
    /// @return Allocated MemBlock or empty MemBlock
    constexpr MemBlock alloc(size<Byte> size) noexcept
    {
      auto blk = allocator::alloc(sizeof(Prefix) + size.to_bytes() + sizeof(Suffix));
      return { blk.ptr() + sizeof(Prefix), size.to_bytes() };
    }

    /// @brief Deallocates a MemBlock that was allocated using the current allocator
    /// @param to_free The block whose resources to free
    constexpr void dealloc(MemBlock to_free) noexcept
    {
      allocator::dealloc(to_free);
    }
  };
}

#endif //!HG_COLT_COMPOSABLE_ALLOC