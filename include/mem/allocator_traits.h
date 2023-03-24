#ifndef HG_COLT_ALLOCATOR_TRAITS
#define HG_COLT_ALLOCATOR_TRAITS

#include <type_traits>
#include <concepts>

#include "./block.h"

namespace clt::meta
{
  template<typename T>
  /// @brief An allocator provides a 'alloc()' and 'dealloc()' method
  concept Allocator = requires (T t, mem::MemBlock b, size<Byte> sz)
  {
    { t.alloc(sz) } -> std::same_as<mem::MemBlock>;
    { t.dealloc(b) } -> std::same_as<void>;
  };

  template<typename T>
  /// @brief An allocator provides a 'alloc()', 'dealloc()' and 'owns()' method
  concept OwningAllocator = Allocator<T> && requires (T t, mem::MemBlock b)
  {
    { t.owns(b) } -> std::same_as<bool>;
  };
}

#endif //!HG_COLT_ALLOCATOR_TRAITS