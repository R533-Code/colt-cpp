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
  static void dealloc(MemBlock blk) noexcept
    COLT_PRE(!blk.is_null())
  {
    GlobalAllocator.dealloc(blk);
  }
  COLT_POST()

    /// @brief Describes a global allocator
    struct AllocatorDescription
  {
    using AllocFn   = MemBlock(*)(size<Byte>)       noexcept;
    using DeallocFn = void(*)(MemBlock)             noexcept;
    using ReallocFn = bool(*)(MemBlock, size<Byte>) noexcept;
    using ExpandFn  = bool(*)(MemBlock, size<Byte>) noexcept;
    using OwnFn     = bool(*)(MemBlock)             noexcept;

    AllocFn     alloc_fn;
    DeallocFn   dealloc_fn;
    ReallocFn   realloc_fn;
    ExpandFn    expand_fn;
    OwnFn       own_fn;
  };

  /// @brief Description of the GlobalAllocator
  inline constexpr AllocatorDescription GlobalAllocatorDescription = { &alloc, &dealloc, nullptr, nullptr, nullptr };

  /// @brief Tag type for asking
  template<typename T>
  struct LocalAllocatorTag
  {
    using allocator_type = T;
  };

  template<typename T>
  inline constexpr LocalAllocatorTag<T> LocalAllocator;
}

namespace clt::meta
{
template<auto T>
concept LocalAllocator = Allocator<std::decay_t<typename decltype(T)::allocator_type>>;
  
template<auto T>
concept GlobalAllocator = std::same_as<std::decay_t<decltype(T)>, mem::AllocatorDescription>;

template<auto T>
concept AllocatorValue = GlobalAllocator<T> || LocalAllocator<T>;
}

namespace clt::mem
{
  template<auto ALLOCATOR>
    requires meta::AllocatorValue<ALLOCATOR>
  struct allocator_ref {};

  template<auto ALLOCATOR>
    requires (meta::LocalAllocator<ALLOCATOR>)
  struct allocator_ref<ALLOCATOR>
  {
    using alloc_t = typename decltype(ALLOCATOR)::allocator_type;    

    alloc_t& ref;

    constexpr MemBlock alloc(size<Byte> size) noexcept
    {
      return ref.alloc(size);
    }

    constexpr void dealloc(MemBlock blk) noexcept
    {
      return ref.dealloc(blk);
    }
  };

  template<meta::Allocator alloc>
  allocator_ref(alloc&)->allocator_ref<mem::LocalAllocator<alloc>>;

  template<auto ALLOCATOR>
    requires (meta::GlobalAllocator<ALLOCATOR>)
  struct allocator_ref<ALLOCATOR>
  {
    constexpr allocator_ref() = default;

    constexpr MemBlock alloc(size<Byte> size) noexcept
    {
      return (*ALLOCATOR.alloc_fn)(size);
    }

    constexpr void dealloc(MemBlock blk) noexcept
    {
      return (*ALLOCATOR.dealloc_fn)(size);
    }
  };
}


#endif //!HG_COLT_GLOBAL_ALLOC