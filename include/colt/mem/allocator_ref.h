#ifndef HG_ALLOCATOR_REF
#define HG_ALLOCATOR_REF

#include "composable_alloc.h"

namespace clt::mem
{
  template<fn_alloc_t ALLOC, fn_dealloc_t DEALLOC, fn_owns_t OWNS,
    fn_expand_t EXPAND, fn_realloc_t REALLOC>
    requires (ALLOC != nullptr) && (DEALLOC != nullptr)
  struct GlobalAllocatorRef
  {
    MemBlock alloc(u64 sz)
    {
      return ALLOC(sz);
    }

    void dealloc(MemBlock sz)
    {
      DEALLOC(sz);
    }

    bool owns(MemBlock sz)
      requires (OWNS != nullptr)
    {
      return OWNS(sz);
    }

    mem::MemBlock expand(mem::MemBlock sz, u64 dt)
      requires (EXPAND != nullptr)
    {
      return EXPAND(sz, dt);
    }

    mem::MemBlock realloc(mem::MemBlock sz, u64 dt)
      requires (REALLOC != nullptr)
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
    
    LocalAllocatorRef(LocalAllocatorRef&&) = default;
    LocalAllocatorRef& operator=(LocalAllocatorRef&&) = default;
    LocalAllocatorRef(const LocalAllocatorRef&) = default;
    LocalAllocatorRef& operator=(const LocalAllocatorRef&) = default;

    MemBlock alloc(u64 sz)
    {
      return ptr->alloc(sz);
    }

    void dealloc(MemBlock sz)
    {
      ptr->dealloc(sz);
    }

    bool owns(MemBlock sz)
      requires meta::OwningAllocator<T>
    {
      return ptr->owns(sz);
    }

    mem::MemBlock expand(mem::MemBlock sz, u64 dt)
      requires meta::ExpandingAllocator<T>
    {
      return ptr->expand(sz, dt);
    }

    mem::MemBlock realloc(mem::MemBlock sz, u64 dt)
      requires meta::ReallocatableAllocator<T>
    {
      return ptr->realloc(sz, dt);
    }
  };
}

#endif //!HG_ALLOCATOR_REF