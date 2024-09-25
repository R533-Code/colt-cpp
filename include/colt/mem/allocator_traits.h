/*****************************************************************/ /**
 * @file   allocator_traits.h
 * @brief  Contains concepts to identify allocators.
 *
 * @author RPC
 * @date   January 2024
 *********************************************************************/
#ifndef HG_COLT_ALLOCATOR_TRAITS
#define HG_COLT_ALLOCATOR_TRAITS

#include <type_traits>
#include <concepts>

#include "block.h"

namespace clt::meta
{
  template<typename T>
  /// @brief An allocator provides a 'alloc()' and 'dealloc()' method
  concept Allocator = std::same_as<std::decay_t<decltype(T::alignment)>, u64>
                      && requires(T t, mem::MemBlock b, u64 sz) {
                           {
                             t.alloc(sz)
                           } -> std::same_as<mem::MemBlock>;
                           {
                             t.dealloc(b)
                           } -> std::same_as<void>;
                         };

  template<typename T>
  /// @brief An allocator provides a 'alloc()', 'dealloc()' and 'owns()' method
  concept OwningAllocator = Allocator<T> && requires(T t, mem::MemBlock b) {
    {
      t.owns(b)
    } -> std::same_as<bool>;
  };

  template<typename T>
  /// @brief An allocator provides a 'alloc()', 'dealloc()' and 'owns()' method
  concept ExpandingAllocator =
      Allocator<T> && requires(T t, mem::MemBlock b, u64 delta) {
        {
          t.expand(b, delta)
        } -> std::same_as<bool>;
      };

  template<typename T>
  /// @brief An allocator provides a 'alloc()', 'dealloc()' and 'owns()' method
  concept ReallocatableAllocator =
      Allocator<T> && requires(T t, mem::MemBlock b, u64 delta) {
        {
          t.realloc(b, delta)
        } -> std::same_as<bool>;
      };
} // namespace clt::meta

namespace clt::mem
{
  /// @brief Represents an allocation function
  using fn_alloc_t = mem::MemBlock(*)(u64);
  /// @brief Represents a deallocation function
  using fn_dealloc_t = void(*)(mem::MemBlock);
  /// @brief Represents an owning function
  using fn_owns_t = bool(*)(mem::MemBlock);
  /// @brief Represents an expansion function
  using fn_expand_t = mem::MemBlock(*)(mem::MemBlock, u64);
  /// @brief Represents a reallocation function
  using fn_realloc_t = mem::MemBlock(*)(mem::MemBlock, u64);
}

#endif //!HG_COLT_ALLOCATOR_TRAITS