/*****************************************************************/ /**
 * @file   block.h
 * @brief  Contains MemBlock, the result of an allocation.
 *
 * @author RPC
 * @date   January 2024
 *********************************************************************/
#ifndef HG_COLT_BLOCK
#define HG_COLT_BLOCK

#include "fmt/format.h"
#include "colt/macro/assert.h"
#include "colt/typedefs.h"

namespace clt::mem
{
  /// @brief Result of an allocation: ptr + size.
  /// If ptr() is nullptr, then size() is guaranteed to be 0.
  /// If size() is 0, ptr() is not guaranteed to be nullptr.
  class MemBlock
  {
    /// @brief Address of the block
    void* blk_ptr = nullptr;
    /// @brief Size of the block (in bytes)
    u64 blk_sz = 0;

  public:
    constexpr MemBlock() = default;
    /// @brief Constructs a MemBlock from a pointer and a size
    /// @param blk The block
    /// @param sz The size of the block
    constexpr MemBlock(void* blk, u64 sz = 0ULL) noexcept
        : blk_ptr(blk)
        , blk_sz(sz * static_cast<u64>(blk != nullptr))
    {
    }

    /// @brief Constructs a MemBlock from two pointers
    /// @param start The beginning of the block
    /// @param end The end of the block
    constexpr MemBlock(void* start, void* end) noexcept
        : blk_ptr(start)
        , blk_sz(static_cast<u8*>(end) - static_cast<u8*>(start))
    {
      assert_true("'start' must precede 'end'!", start < end);
      blk_sz *= static_cast<u64>(start != nullptr);
    }

    /// @brief Constructs a MemBlock from a nullptr.
    /// The size of a MemBlock with ptr() == nullptr is always 0.
    constexpr MemBlock(std::nullptr_t, u64 = 0) noexcept {}

    //Copy constructor
    constexpr MemBlock(const MemBlock&) noexcept = default;
    //Move constructor
    constexpr MemBlock(MemBlock&&) noexcept = default;
    //Copy-assignment operator
    constexpr MemBlock& operator=(const MemBlock&) noexcept = default;
    //Move-assignment operator
    constexpr MemBlock& operator=(MemBlock&&) noexcept = default;

    /// @brief Sets the block to nullptr
    /// @param  nullptr
    /// @return Self
    constexpr MemBlock& operator=(std::nullptr_t) noexcept
    {
      blk_ptr = nullptr;
      blk_sz  = 0;
      return *this;
    }

    /// @brief Returns the size of a block (in bytes).
    /// The size of a block cannot be modified directly, except by assignment.
    /// @return Byte size of the block
    constexpr u64 size() const noexcept { return blk_sz; }
    /// @brief Returns the pointer to the memory block
    /// @return Pointer (can be nullptr)
    constexpr void* ptr() const noexcept { return blk_ptr; }

    /// @brief Check if 'ptr() == nullptr'
    /// @return True if the MemBlock points to no block
    constexpr bool is_null() const noexcept { return blk_ptr == nullptr; }
    /// @brief Check if 'ptr() != nullptr'
    /// @return True if the MemBlock points to a block
    explicit constexpr operator bool() const noexcept { return blk_ptr != nullptr; }

    /// @brief Check if two blocks are equal.
    /// Comparison of the pointer, then the sizes are done.
    /// @param blk The block to compare with
    /// @return True if both the block and size are equal
    constexpr bool operator==(const MemBlock&) const noexcept = default;

    /// @brief Check if a block is nullptr
    /// @param  nullptr
    /// @return True if nullptr
    constexpr bool operator==(std::nullptr_t) const noexcept
    {
      return blk_ptr == nullptr;
    }

    /// @brief Check if a block is not nullptr
    /// @param  nullptr
    /// @return True if not nullptr
    constexpr bool operator!=(std::nullptr_t) const noexcept
    {
      return blk_ptr != nullptr;
    }
  };

  /// @brief Represents an empty block
  inline constexpr MemBlock nullblk = MemBlock{nullptr, 0};

  template<u64 ALIGN>
  /// @brief Rounds 'sz' to an alignment if it is not already aligned
  /// @param sz The size to align
  /// @return The aligned size
  constexpr size_t round_to_alignment(size_t sz) noexcept
  {
    //Do no round as already rounded
    if (sz % ALIGN == 0)
      return sz;
    //Round size upward if needed
    return sz + ALIGN - (sz % ALIGN);
  }

  namespace details
  {
    template<typename Old, typename New>
    /// @brief Reallocates using new allocator, copying the memory,
    /// and deallocating the block on success.
    /// @tparam New The type of the new allocator
    /// @tparam Old The type of the old allocator
    /// @param old_a The old allocator to use for deallocation
    /// @param new_a The new allocator to use for allocation
    /// @param blk The block to "reallocate"
    /// @param n The new size
    /// @return True on success
    constexpr bool realloc_with_copy(
        Old& old_a, New& new_a, MemBlock& blk, u64 n) noexcept
    {
      MemBlock new_blk = new_a.alloc(n);
      if (new_blk.is_null())
        return false;
      
      std::memcpy(
          new_blk.ptr(), blk.ptr(),
          blk.size() < new_blk.size() ? blk.size()
                                      : new_blk.size());
      old_a.dealloc(blk);
      blk = new_blk;
      return true;
    }
  } // namespace details
} // namespace clt::mem

template<>
struct fmt::formatter<clt::mem::MemBlock>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    // TODO: add format option
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const clt::mem::MemBlock& vec, FormatContext& ctx)
  {
    return fmt::format_to(ctx.out(), "{{{}, {}}}", vec.ptr(), vec.size());
  }
};

#endif //!HG_COLT_BLOCK