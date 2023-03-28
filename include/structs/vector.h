#ifndef HG_COLT_VECTOR
#define HG_COLT_VECTOR

#include <utility>
#include "../mem/global_alloc.h"

namespace clt
{
  template<typename T, auto ALLOCATOR = mem::GlobalAllocatorDescription>
    requires meta::AllocatorValue<ALLOCATOR>
  class Vector
    : private mem::allocator_ref<ALLOCATOR>
  {
    /// @brief True if the allocator is global
    static constexpr bool is_global   = meta::GlobalAllocator<ALLOCATOR>;
    /// @brief True if the allocator is local (we need a reference to it)
    static constexpr bool is_local    = meta::LocalAllocator<ALLOCATOR>;

    /// @brief Type of the allocator: use Allocator::alloc/dealloc
    using Allocator = mem::allocator_ref<ALLOCATOR>;

    /// @brief Pointer to the allocated block (can be null)
    T* blk_ptr = nullptr;
    /// @brief Capacity (count) of objects of the block
    size_t blk_capacity = 0;
    /// @brief Count of active objects in the block
    size_t blk_size = 0;

    void reserve_obj(size_t plus_capacity) noexcept
    {

    }

  public:
    template<meta::Allocator AllocT> requires is_local
    /// @brief Default constructor (when allocator is local)
    /// @param alloc Reference to the allocator
    constexpr Vector(AllocT& alloc) noexcept
      : Allocator(alloc) {}
    
    /// @brief Default constructor (when allocator is global)
    constexpr Vector() noexcept requires is_global = default;

    template<meta::Allocator AllocT> requires is_local
    /// @brief Default constructor (when allocator is local)
    /// @param alloc Reference to the allocator
    constexpr Vector(AllocT& alloc, size_t reserve) noexcept
      : Allocator(alloc) {}

    /// @brief Default constructor (when allocator is global)
    constexpr Vector(size_t reserve) noexcept requires is_global
      :
  };

  template<typename T, meta::Allocator alloc>
  Vector(alloc&)->Vector<T, mem::LocalAllocator<alloc>>;
}

#endif //!HG_COLT_VECTOR