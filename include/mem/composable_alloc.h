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
    /// @brief Alignment of returned MemBlock (min alignment of both allocator)
    static constexpr u64 alignment = (Primary::alignment < Secondary::alignment) ? Primary::alignment : Secondary::alignment;
    
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

    /// @brief Reallocates a MemBlock
    /// @param blk The block to reallocate
    /// @param n The new size of the block
    /// @return True if reallocation was successful
    constexpr bool realloc(MemBlock& blk, size<Byte> n) noexcept
    {
      if (blk.size() == n)
        return true;
      if (blk.is_null())
      {
        blk = alloc(n);
        return !blk.is_null();
      }
      if (n == 0_B)
      {
        dealloc(blk);
        return true;
      }

      if (blk.size() <= SIZE)
      {
        if (n <= SIZE)
          return Primary::realloc(blk, n);
        return details::realloc_with_copy(*this, static_cast<Secondary&>(*this), blk, n);
      }

      if (n <= SIZE)
        return details::realloc_with_copy(*this, static_cast<Primary&>(*this), blk, n);
      return Secondary::realloc(blk, n);
    }

    /// @brief Expands a block in place if possible
    /// @param blk The block to expand
    /// @param delta The new size
    /// @return True if expansion was done successfully
    constexpr bool expand(MemBlock& blk, size<Byte> delta) noexcept
    {
      //If expansion will make block size go over SIZE,
      //then expansion is not possible.
      if (blk.size() <= SIZE && blk.size().to_bytes() + delta > SIZE.to_bytes())
        return false;
      
      if (blk.size() <= SIZE)
        return Primary::expand(blk, delta);
      return Secondary::expand(blk, delta);
    }
  };

  template<meta::Allocator allocator, typename Prefix, typename Suffix>
    requires (std::is_trivially_copyable_v<Prefix> || std::is_void_v<Prefix>) && (std::is_trivially_copyable_v<Suffix> || std::is_void_v<Suffix>)
  struct AffixAllocator
    : private allocator
  {
    /// @brief Alignment of returned MemBlock
    static constexpr u64 alignment = allocator::alignment;
    
    /// @brief Aligned size of prefix in bytes
    static constexpr u64 prefix_size = round_to_alignment<alignment>(meta::sizeof_or_zero_v<Prefix>);
    /// @brief Aligned size of suffix in bytes
    static constexpr u64 suffix_size = round_to_alignment<alignment>(meta::sizeof_or_zero_v<Suffix>);
  
  private:

    /// @brief This helper avoids compile time errors due to 'void&' being invalid
    using helper_suffix_t = std::conditional_t<std::is_void_v<Suffix>, u8, Suffix>;
    /// @brief This helper avoids compile time errors due to 'void&' being invalid
    using helper_prefix_t = std::conditional_t<std::is_void_v<Prefix>, u8, Prefix>;

    /// @brief Converts a block from returned form to the its prefix form
    /// @param blk The block to convert
    /// @return The converted block
    static constexpr MemBlock from_ret_to_prefix(MemBlock blk) const noexcept
    {
      return MemBlock{ static_cast<u8*>(blk.ptr()) - prefix_size,
        blk.size().to_bytes() + prefix_size + suffix_size }
    }

    /// @brief Converts a block from its prefix form to the returned form
    /// @param blk The block to convert
    /// @return The converted block
    static constexpr MemBlock from_prefix_to_ret(MemBlock blk) const noexcept
    {
      return MemBlock{ static_cast<u8*>(blk.ptr()) + prefix_size,
        blk.size().to_bytes() - prefix_size - suffix_size }
    }

  public:

    /// @brief Allocates a MemBlock
    /// @param size The size of the allocation
    /// @return Allocated MemBlock or empty MemBlock
    constexpr MemBlock alloc(size<Byte> size) noexcept
    {
      if (auto blk = allocator::alloc(prefix_size + size.to_bytes() + suffix_size))
        return from_prefix_to_ret(blk);
      return nullblk;
    }

    /// @brief Deallocates a MemBlock that was allocated using the current allocator
    /// @param to_free The block whose resources to free
    constexpr void dealloc(MemBlock to_free) noexcept
    {
      allocator::dealloc(from_ret_to_prefix(to_free));
    }

    /// @brief Check if the current allocator owns 'blk'
    /// @param blk The MemBlock to check
    /// @return True if 'blk' was allocated through the current allocator
    constexpr bool owns(MemBlock blk) const noexcept
    {
      return allocator::owns(from_ret_to_prefix(blk));
    }

    template<typename... Args> requires (!std::same_as<void, Prefix>)
    /// @brief Constructs the prefix
    /// @tparam ...Args The types of the arguments to forward
    /// @param blk The block whose prefix to modify
    /// @param ...args The parameter pack to forward to the constructor
    /// @return Reference to the constructed prefix
    constexpr helper_prefix_t& create_prefix(MemBlock blk, Args&&... args) const noexcept(std::is_nothrow_constructible_v<Prefix, Args...>)
    {
      return *static_cast<Prefix*>(new(static_cast<u8*>(blk.ptr()) - prefix_size) Prefix(std::forward<Args>(args)...));
    }

    template<typename... Args> requires (!std::same_as<void, Suffix>)
    /// @brief Constructs the suffix
    /// @tparam ...Args The types of the arguments to forward
    /// @param blk The block whose suffix to modify
    /// @param ...args The parameter pack to forward to the constructor
    /// @return Reference to the constructed suffix
    constexpr helper_suffix_t& create_suffix(MemBlock blk, Args&&... args) const noexcept(std::is_nothrow_constructible_v<Suffix, Args...>)
    {
      return *static_cast<Suffix*>(new(static_cast<u8*>(blk.ptr()) + blk.size().to_bytes()) Suffix(std::forward<Args>(args)...));
    }

    /// @brief Destroy the prefix. Be sure to have constructed the prefix!
    /// @param blk The block whose prefix to destroy
    constexpr void destroy_prefix(MemBlock blk) const noexcept(std::is_nothrow_destructible_v<Prefix>)
    {
      static_assert(!std::same_as<void, Prefix>, "AffixAllocator does not have a prefix!");
      static_cast<Prefix*>(static_cast<u8*>(blk.ptr()) - prefix_size)->~Prefix();
    }

    /// @brief Destroy the suffix. Be sure to have constructed the suffix!
    /// @param blk The block whose suffix to destroy
    constexpr void destroy_suffix(MemBlock blk) const noexcept(std::is_nothrow_destructible_v<Suffix>)
    {
      static_assert(!std::same_as<void, Suffix>, "AffixAllocator does not have a suffix!");
      static_cast<Suffix*>(static_cast<u8*>(blk.ptr()) + blk.size().to_bytes())->~Suffix();
    }

    /// @brief Returns the prefix. Be sure to have constructed the prefix!
    /// @param blk The block whose prefix to return
    /// @return reference to the prefix
    constexpr helper_prefix_t& get_prefix(MemBlock blk) const noexcept
    {
      static_assert(!std::same_as<void, Prefix>, "AffixAllocator does not have a prefix!");
      return *static_cast<Prefix*>(static_cast<void*>(static_cast<u8*>(blk.ptr()) - prefix_size));
    }

    /// @brief Returns the suffix. Be sure to have constructed the suffix!
    /// @param blk The block whose suffix to return
    /// @return reference to the suffix
    constexpr helper_suffix_t& get_suffix(MemBlock blk) const noexcept
    {
      static_assert(!std::same_as<void, Suffix>, "AffixAllocator does not have a suffix!");
      return *static_cast<Suffix*>(static_cast<void*>(static_cast<u8*>(blk.ptr()) + blk.size().to_bytes()));
    }

    /// @brief Reallocates a MemBlock
    /// @param blk The block to reallocate
    /// @param n The new size of the block
    /// @return True if reallocation was successful
    constexpr bool realloc(MemBlock& blk, size<Byte> n) noexcept
    {
      if (blk.size() == n)
        return true;
      if (blk.is_null())
      {
        blk = alloc(n);
        return !blk.is_null();
      }
      if (n == 0_B)
      {
        dealloc(blk);
        return true;
      }
      if constexpr (!std::is_void_v<Suffix>) //if suffix
        Suffix suffix = get_suffix(blk); //copy suffix
      
      if (MemBlock cpy = from_ret_to_prefix(blk);
        allocator::realloc(cpy, n))
      {
        blk = from_prefix_to_ret(cpy);
        if constexpr (!std::is_void_v<Suffix>) //if suffix
          get_suffix(blk) = suffix;
        return true;
      }
      return false;
    }

    /// @brief Expands a block in place if possible
    /// @param blk The block to expand
    /// @param delta The new size
    /// @return True if expansion was done successfully
    constexpr bool expand(MemBlock& blk, size<Byte> delta) noexcept
    {
      if (delta == 0_B)
        return true;

      if (blk.is_null())
      {
        blk = alloc(delta);
        return !blk.is_null();
      }
      if constexpr (!std::is_void_v<Suffix>) //if suffix
        Suffix suffix = get_suffix(blk); //copy suffix

      if (MemBlock cpy = from_ret_to_prefix(blk);
        allocator::expand(cpy, delta))
      {
        blk = from_prefix_to_ret(cpy);
        if constexpr (!std::is_void_v<Suffix>) //if suffix
          get_suffix(blk) = suffix;
        return true;
      }
      return false;
    }
  };

  template<meta::Allocator allocator, size<Byte> BUFFER_SIZE, u8 PATTERN = 0xFD>
  /// @brief Memory corruption detector, detects if memory around a block was corrupted
  class MemCorruptDetector
    : private AffixAllocator<allocator, std::array<u8, BUFFER_SIZE.to_bytes()>, std::array<u8, BUFFER_SIZE.to_bytes()>>
  {
    /// @brief Inherited allocator
    using Allocator = AffixAllocator<allocator, std::array<u8, BUFFER_SIZE.to_bytes()>, std::array<u8, BUFFER_SIZE.to_bytes()>>;
    /// @brief Type of suffix and prefix
    using Array = std::array<u8, BUFFER_SIZE.to_bytes()>;

    /// @brief Returns the array filled with PATTERN.
    /// This allows us to use 'memcmp' to check for corruption.
    /// @return Array of size BUFFER_SIZE filled with PATTERN
    static consteval Array get_pattern() noexcept
    {
      Array arr;
      arr.fill(PATTERN);
      return arr;
    }

    /// @brief The Array pattern
    static constexpr auto pattern = get_pattern();    

  public:
    /// @brief Alignment of returned MemBlock
    static constexpr u64 alignment = Allocator::alignment;

    /// @brief Check if prefix of block is corrupted
    /// @param blk The block to check for
    /// @return True if corrupted
    constexpr bool is_prefix_corrupted(MemBlock blk) const noexcept
    {
      return std::memcmp(&Allocator::get_prefix(blk), pattern.data(), BUFFER_SIZE.to_bytes()) == 0;
    }

    /// @brief Check if suffix of block is corrupted
    /// @param blk The block to check for
    /// @return True if corrupted
    constexpr bool is_suffix_corrupted(MemBlock blk) const noexcept
    {
      return std::memcmp(&Allocator::get_suffix(blk), pattern.data(), BUFFER_SIZE.to_bytes()) == 0;
    }

    /// @brief Allocates a MemBlock
    /// @param size The size of the allocation
    /// @return Allocated MemBlock or empty MemBlock
    constexpr MemBlock alloc(size<Byte> size) noexcept
    {
      if (auto blk = Allocator::alloc(size))
      {
        Allocator::get_prefix(blk).fill(PATTERN);
        Allocator::get_suffix(blk).fill(PATTERN);
        return blk;
      }
      return nullblk;
    }

    /// @brief Deallocates a MemBlock that was allocated using the current allocator
    /// @param to_free The block whose resources to free
    constexpr void dealloc(MemBlock to_free) noexcept
    {
      assert_true(!is_prefix_corrupted(), !is_suffix_corrupted());
      Allocator::dealloc(to_free);
    }

    /// @brief Check if the current allocator owns 'blk'
    /// @param blk The MemBlock to check
    /// @return True if 'blk' was allocated through the current allocator
    constexpr bool owns(MemBlock blk) const noexcept
    {
      return Allocator::owns(blk);
    }

    /// @brief Reallocates a MemBlock
    /// @param blk The block to reallocate
    /// @param n The new size of the block
    /// @return True if reallocation was successful
    constexpr bool realloc(MemBlock& blk, size<Byte> n) noexcept
    {
      return Allocator::realloc(blk, n);
    }

    /// @brief Expands a block in place if possible
    /// @param blk The block to expand
    /// @param delta The new size
    /// @return True if expansion was done successfully
    constexpr bool expand(MemBlock& blk, size<Byte> delta) noexcept
    {
      return Allocator::expand(blk, delta);
    }
  };  
}

#endif //!HG_COLT_COMPOSABLE_ALLOC