/*****************************************************************//**
 * @file   allocator_ref.h
 * @brief  Contains GlobalAllocatorRef and LocalAllocatorRef.
 * These types are the actual allocators inherited by data structures.
 * 
 * @author RPC
 * @date   September 2024
 *********************************************************************/
#ifndef HG_ALLOCATOR_REF
#define HG_ALLOCATOR_REF

#include "simple_alloc.h"
#include "composable_alloc.h"

namespace clt::mem
{
  // TODO: add polymorphic allocator.

  /// @brief Reference to a global allocator
  /// @tparam ALIGN The alignment of the allocator
  /// @tparam ALLOC The allocation function
  /// @tparam DEALLOC The deallocation function
  /// @tparam OWNS The owning function or null
  /// @tparam EXPAND The expansion function or null
  /// @tparam REALLOC The reallocation function or null
  template<
      u64 ALIGN, fn_alloc_t ALLOC, fn_dealloc_t DEALLOC, fn_owns_t OWNS,
      fn_expand_t EXPAND, fn_realloc_t REALLOC>
    requires(ALLOC != nullptr) && (DEALLOC != nullptr)
  struct GlobalAllocatorRef
  {
    /// @brief The alignment of the allocator
    static constexpr u64 alignment = ALIGN;
    /// @brief Helper flag
    static constexpr bool is_global_allocator_ref = true;

    /// @brief Allocates a MemBlock of size u64
    /// @param sz The size
    /// @return The allocated MemBlock
    MemBlock alloc(u64 sz) const { return ALLOC(sz); }
    /// @brief Deallocates a MemBlock
    /// @param sz The block to deallocate
    void dealloc(MemBlock sz) const { DEALLOC(sz); }

    /// @brief Check if a MemBlock is owned by this allocator
    /// @param sz The MemBlock
    /// @return True if owned by this allocator
    bool owns(MemBlock sz) const
      requires(OWNS != nullptr)
    {
      return OWNS(sz);
    }

    /// @brief Expand a MemBlock
    /// @param sz The MemBlock
    /// @param dt The new size
    /// @return MemBlock
    MemBlock expand(MemBlock sz, u64 dt) const
      requires(EXPAND != nullptr)
    {
      return EXPAND(sz, dt);
    }

    /// @brief Reallocates a MemBlock
    /// @param sz The MemBlock
    /// @param dt The new size
    /// @return MemBlock
    MemBlock realloc(MemBlock sz, u64 dt) const
      requires(REALLOC != nullptr)
    {
      return REALLOC(sz, dt);
    }
  };

  /// @brief Reference to a local allocator
  /// @tparam T The type of the allocator
  template<meta::Allocator T>
  class LocalAllocatorRef
  {
    /// @brief Pointer to the allocator (never null)
    T* ptr;

  public:
    /// @brief Helper flag
    static constexpr bool is_global_allocator_ref = false;

    LocalAllocatorRef() = delete;
    /// @brief Constructor
    /// @param ptr The reference to the local allocator
    LocalAllocatorRef(T& ptr) noexcept
        : ptr(&ptr)
    {
    }

    /// @brief The alignment of the allocator (same as T)
    static constexpr u64 alignment = T::alignment;

    LocalAllocatorRef(LocalAllocatorRef&&)                 = default;
    LocalAllocatorRef& operator=(LocalAllocatorRef&&)      = default;
    LocalAllocatorRef(const LocalAllocatorRef&)            = default;
    LocalAllocatorRef& operator=(const LocalAllocatorRef&) = default;

    /// @brief Allocates a MemBlock of size u64
    /// @param sz The size
    /// @return The allocated MemBlock
    MemBlock alloc(u64 sz) const { return ptr->alloc(sz); }
    /// @brief Deallocates a MemBlock
    /// @param sz The block to deallocate
    void dealloc(MemBlock sz) const { ptr->dealloc(sz); }

    /// @brief Check if a block is owned by the current allocator
    /// @param sz The size
    /// @return True if owned by this allocator
    bool owns(MemBlock sz) const
      requires meta::OwningAllocator<T>
    {
      return ptr->owns(sz);
    }

    /// @brief Expand
    /// @param sz The block
    /// @param dt The new size
    /// @return The result of expansion
    MemBlock expand(MemBlock sz, u64 dt) const
      requires meta::ExpandingAllocator<T>
    {
      return ptr->expand(sz, dt);
    }

    /// @brief Realloc
    /// @param sz The block
    /// @param dt The new size
    /// @return The result of reallocation
    MemBlock realloc(MemBlock sz, u64 dt) const
      requires meta::ReallocatableAllocator<T>
    {
      return ptr->realloc(sz, dt);
    }
  };

  /// @brief The default global allocator
  inline FreeList<Mallocator, 16, size_t{4096}, size_t{4096}> DefaultGlobalAllocator;

  /// @brief Global alloc
  /// @param size The size of the block
  /// @return The allocated block
  inline MemBlock global_alloc(u64 size) noexcept
  {
    return DefaultGlobalAllocator.alloc(size);
  }

  /// @brief Global dealloc
  /// @param blk The block
  inline void global_dealloc(MemBlock blk) noexcept
  {
    DefaultGlobalAllocator.dealloc(blk);
  }

  /// @brief The default GlobalAllocatorRef used by make_*
  static constexpr GlobalAllocatorRef<
      decltype(DefaultGlobalAllocator)::alignment, &global_alloc, &global_dealloc,
      nullptr, nullptr, nullptr>
      GlobalAllocator;
} // namespace clt::mem

namespace clt::meta
{
  /// @brief Reference to either a global allocator or a local allocator
  template<typename T>
  concept AllocatorRef =
      Allocator<T> && std::same_as<bool, decltype(T::is_global_allocator_ref)>;
}
#endif //!HG_ALLOCATOR_REF