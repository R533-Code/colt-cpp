#ifndef HG_ALLOCATOR_REF
#define HG_ALLOCATOR_REF

#include "simple_alloc.h"
#include "composable_alloc.h"

namespace clt::mem
{
  template<
      u64 ALIGN, fn_alloc_t ALLOC, fn_dealloc_t DEALLOC, fn_owns_t OWNS, fn_expand_t EXPAND,
      fn_realloc_t REALLOC>
    requires(ALLOC != nullptr) && (DEALLOC != nullptr)
  struct GlobalAllocatorRef
  {
    static constexpr u64 alignment = ALIGN;

    MemBlock alloc(u64 sz) const { return ALLOC(sz); }

    void dealloc(MemBlock sz) const { DEALLOC(sz); }

    bool owns(MemBlock sz) const
      requires(OWNS != nullptr)
    {
      return OWNS(sz);
    }

    mem::MemBlock expand(mem::MemBlock sz, u64 dt) const
      requires(EXPAND != nullptr)
    {
      return EXPAND(sz, dt);
    }

    mem::MemBlock realloc(mem::MemBlock sz, u64 dt) const
      requires(REALLOC != nullptr)
    {
      return REALLOC(sz, dt);
    }
  };

  template<meta::Allocator T>
  class LocalAllocatorRef
  {
    T* ptr;

  public:
    LocalAllocatorRef() = delete;
    LocalAllocatorRef(T& ptr) noexcept
        : ptr(&ptr)
    {
    }

    static constexpr u64 alignment = T::alignment;

    LocalAllocatorRef(LocalAllocatorRef&&)                 = default;
    LocalAllocatorRef& operator=(LocalAllocatorRef&&)      = default;
    LocalAllocatorRef(const LocalAllocatorRef&)            = default;
    LocalAllocatorRef& operator=(const LocalAllocatorRef&) = default;

    MemBlock alloc(u64 sz) const { return ptr->alloc(sz); }

    void dealloc(MemBlock sz) const { ptr->dealloc(sz); }

    bool owns(MemBlock sz) const
      requires meta::OwningAllocator<T>
    {
      return ptr->owns(sz);
    }

    mem::MemBlock expand(mem::MemBlock sz, u64 dt) const
      requires meta::ExpandingAllocator<T>
    {
      return ptr->expand(sz, dt);
    }

    mem::MemBlock realloc(mem::MemBlock sz, u64 dt) const
      requires meta::ReallocatableAllocator<T>
    {
      return ptr->realloc(sz, dt);
    }
  };

  inline FreeList<Mallocator, 16, size_t{4096}, size_t{4096}> DefaultGlobalAllocator;

  inline MemBlock global_alloc(u64 size) noexcept
  {
    return DefaultGlobalAllocator.alloc(size);
  }

  inline void global_dealloc(MemBlock blk) noexcept
  {
    DefaultGlobalAllocator.dealloc(blk);
  }

  static constexpr GlobalAllocatorRef<
      decltype(DefaultGlobalAllocator)::alignment, &global_alloc, &global_dealloc, nullptr, nullptr,
      nullptr>
      GlobalAllocator;  
} // namespace clt::mem
#endif //!HG_ALLOCATOR_REF