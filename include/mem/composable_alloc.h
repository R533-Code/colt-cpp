#ifndef HG_COLT_COMPOSABLE_ALLOC
#define HG_COLT_COMPOSABLE_ALLOC

#include "./simple_alloc.h"
#include "../meta/traits.h"

namespace clt::mem
{
  template<meta::OwningAllocator Primary, meta::Allocator Fallback>
  /// @brief Allocator that tries to allocate through the Primary allocator and on failure uses the Fallback allocator
  struct FallbackAllocator
    : private Primary, private Fallback
  {
    /// @brief Alignment of returned MemBlock (min alignment of both allocator)
    static constexpr u64 alignment = (Primary::alignment < Fallback::alignment) ? Primary::alignment : Fallback::alignment;

    /// @brief Allocates a MemBlock
    /// @param size The size of the allocation
    /// @return Allocated MemBlock or empty MemBlock
    constexpr MemBlock alloc(size<Byte> size) noexcept
    {
      auto blk = Primary::alloc(size);
      if (blk.is_null())
        return Fallback::alloc(size);
      return blk;
    }

    /// @brief Deallocates a MemBlock that was allocated using the current allocator
    /// @param to_free The block whose resources to free
    constexpr void dealloc(MemBlock to_free) noexcept
    {
      if (Primary::owns(to_free))
        Primary::dealloc(to_free);
      else
        Fallback::dealloc(to_free);
    }

    /// @brief Check if Primary or Fallback owns 'blk'
    /// @param blk The MemBlock to check
    /// @return True if 'blk' was allocated through the current allocator
    constexpr bool owns(MemBlock blk) const noexcept
    {
      return Primary::owns(blk) || Fallback::owns(blk);
    }

    /// @brief Reallocates a MemBlock
    /// @param blk The block to reallocate
    /// @param n The new size of the block
    /// @return True if reallocation was successful
    constexpr bool realloc(MemBlock& blk, size<Byte> n) noexcept
    {
      if (Primary::owns(blk))
        Primary::realloc(blk);
      else
        Fallback::realloc(blk);
    }

    /// @brief Expands a block in place if possible
    /// @param blk The block to expand
    /// @param delta The new size
    /// @return True if expansion was done successfully
    constexpr bool expand(MemBlock& blk, size<Byte> delta) noexcept
    {
      if (Primary::owns(blk))
        Primary::expand(blk);
      else
        Fallback::expand(blk);
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
      if (to_free.size() <= SIZE)
        Primary::dealloc(to_free);
      else
        Secondary::dealloc(to_free);
    }

    /// @brief Check if the current allocator owns 'blk'
    /// @param blk The MemBlock to check
    /// @return True if 'blk' was allocated through the current allocator
    constexpr bool owns(MemBlock blk) const noexcept
    {
      if (blk.size() <= SIZE)
        return Primary::owns(blk);
      return Secondary::owns(blk);
    }
  };

  template<meta::Allocator allocator, typename Prefix, typename Suffix>
  struct AffixAllocator
    : private allocator
  {
    /// @brief Aligned size of prefix in bytes
    static constexpr u64 prefix_size = meta::sizeof_or_zero_v<Prefix>;
    /// @brief Aligned size of suffix in bytes
    static constexpr u64 suffix_size = meta::sizeof_or_zero_v<Suffix>;

  private:
    using helper_suffix_t = std::conditional_t<std::is_same_v<Suffix, void>, u8, Suffix>;
    using helper_prefix_t = std::conditional_t<std::is_same_v<Prefix, void>, u8, Prefix>;

  public:

    /// @brief Allocates a MemBlock
    /// @param size The size of the allocation
    /// @return Allocated MemBlock or empty MemBlock
    constexpr MemBlock alloc(size<Byte> size) noexcept
    {
      auto blk = allocator::alloc(prefix_size + size.to_bytes() + suffix_size);
      return { static_cast<u8*>(blk.ptr()) + prefix_size, size.to_bytes() };
    }

    /// @brief Deallocates a MemBlock that was allocated using the current allocator
    /// @param to_free The block whose resources to free
    constexpr void dealloc(MemBlock to_free) noexcept
    {
      allocator::dealloc(
        MemBlock{ static_cast<u8*>(to_free.ptr()) - prefix_size,
        to_free.size().to_bytes() + prefix_size + suffix_size }
      );
    }

    /// @brief Check if the current allocator owns 'blk'
    /// @param blk The MemBlock to check
    /// @return True if 'blk' was allocated through the current allocator
    constexpr bool owns(MemBlock blk) const noexcept
    {
      return allocator::owns(
        MemBlock{ blk.ptr() - prefix_size,
        blk.size().to_bytes() + prefix_size + suffix_size }
      );
    }

    template<typename... Args> requires (!std::same_as<void, Prefix>)
    constexpr helper_prefix_t& create_prefix(MemBlock blk, Args&&... args) const noexcept
    {
      return *static_cast<Prefix*>(new(static_cast<u8*>(blk.ptr()) - prefix_size) Prefix(std::forward<Args>(args)...));
    }

    template<typename... Args> requires (!std::same_as<void, Suffix>)
    constexpr helper_suffix_t& create_suffix(MemBlock blk, Args&&... args) const noexcept
    {
      return *static_cast<Suffix*>(new(static_cast<u8*>(blk.ptr()) + blk.size().to_bytes()) Suffix(std::forward<Args>(args)...));
    }

    constexpr void destroy_prefix(MemBlock blk) const noexcept
    {
      static_assert(!std::same_as<void, Prefix>, "AffixAllocator does not have a prefix!");
      static_cast<Prefix*>(static_cast<u8*>(blk.ptr()) - prefix_size)->~Prefix();
    }

    constexpr void destroy_suffix(MemBlock blk) const noexcept
    {
      static_assert(!std::same_as<void, Suffix>, "AffixAllocator does not have a suffix!");
      static_cast<Suffix*>(static_cast<u8*>(blk.ptr()) + blk.size().to_bytes())->~Suffix();
    }

    constexpr helper_prefix_t& get_prefix(MemBlock blk) const noexcept
    {
      static_assert(!std::same_as<void, Prefix>, "AffixAllocator does not have a prefix!");
      return *static_cast<Prefix*>(static_cast<u8*>(blk.ptr()) - prefix_size);
    }

    constexpr helper_suffix_t& get_suffix(MemBlock blk) const noexcept
    {
      static_assert(!std::same_as<void, Suffix>, "AffixAllocator does not have a suffix!");
      return *static_cast<Suffix*>(static_cast<u8*>(blk.ptr()) + blk.size().to_bytes());
    }

    /// @brief Reallocates a MemBlock
    /// @param blk The block to reallocate
    /// @param n The new size of the block
    /// @return True if reallocation was successful
    constexpr bool realloc(MemBlock& blk, size<Byte> n) noexcept
    {
      
    }

    /// @brief Expands a block in place if possible
    /// @param blk The block to expand
    /// @param delta The new size
    /// @return True if expansion was done successfully
    constexpr bool expand(MemBlock& blk, size<Byte> delta) noexcept
    {

    }
  };
}

#endif //!HG_COLT_COMPOSABLE_ALLOC