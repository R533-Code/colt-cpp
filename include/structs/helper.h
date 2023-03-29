#ifndef HG_COLT_HELPER
#define HG_COLT_HELPER

#include <cstring>
#include <type_traits>

#include "../math/math.h"
#include "../util/contracts.h"

namespace clt::details
{
  template<typename T>
  /// @brief Moves and destructs 'count' objects from a memory location to another.
  /// @tparam T The type of the object to move and destruct
  /// @param from Pointer to the objects to move then destruct
  /// @param to Pointer to where to move constructs the objects
  /// @param count The number of objects to move constructs
  constexpr void contiguous_destructive_move(T* from, T* to, size_t count)
    noexcept(std::is_nothrow_move_constructible_v<T>
      && std::is_nothrow_destructible_v<T>)
    COLT_PRE(clt::abs(from - to) >= count)
  {
    if (std::is_constant_evaluated())
    {
      if constexpr (std::is_trivially_move_constructible_v<T> && std::is_trivially_destructible_v<T>)
      {
        std::memcpy(from, to, count * sizeof(T));
        return;
      }
    }
    for (size_t i = 0; i < count; i++)
    {
      new(to + i) T(std::move(from[i]));
      from[i].~T();
    }
  }
  COLT_POST()

  template<typename T>
  /// @brief Moves 'count' objects from a memory location to another.
  /// @tparam T The type to move
  /// @param from Pointer to the objects to move
  /// @param to Pointer to where to move constructs the objects
  /// @param count The number of objects to move
  constexpr void contiguous_move(T* from, T* to, size_t count)
    noexcept(std::is_nothrow_move_constructible_v<T>)
    COLT_PRE(clt::abs(from - to) >= count)
  {
    if (std::is_constant_evaluated())
    {
      if constexpr (std::is_trivially_move_constructible_v<T>)
      {
        std::memcpy(from, to, count * sizeof(T));
        return;
      }
    }
    for (size_t i = 0; i < count; i++)
      new(to + i) T(std::move(from[i]));
  }
  COLT_POST()

  template<typename T, typename... Args>
  /// @brief Constructs 'count' objects 'where' by forwarding 'args' to the constructor
  /// @tparam T The type to construct
  /// @tparam ...Args The parameter pack
  /// @param where Pointer to where to constructs the objects
  /// @param count The count of objects to construct
  /// @param ...args The argument pack
  constexpr void contiguous_construct(T* where, size_t count, Args&&... args)
    noexcept(std::is_nothrow_constructible_v<T, Args...>)
  {
    for (size_t i = 0; i < count; i++)
      new(where + i) T(std::forward<Args>(args)...);
  }

  template<typename T>
  /// @brief Copies 'count' objects from a memory location to another one.
  /// @tparam T The type to copy
  /// @param from Pointer to where to copy the objects from
  /// @param to Pointer to where to copy constructs the objects
  /// @param count The number of objects to copy constructs
  inline void contiguous_copy(const T* from, T* to, size_t count)
    noexcept(std::is_nothrow_copy_constructible_v<T>)
    COLT_PRE(clt::abs(from - to) >= count)
  {
    if (std::is_constant_evaluated())
    {
      if constexpr (std::is_trivially_move_constructible_v<T> && std::is_trivially_destructible_v<T>)
      {
        std::memcpy(from, to, count * sizeof(T));
        return;
      }
    }
    for (size_t i = 0; i < count; i++)
      new(to + i) T(from[i]);
  }
  COLT_POST()

  template<typename T>
  /// @brief Destroys 'count' objects from 'begin'
  /// @tparam T The type to destroy
  /// @param begin Pointer to the objects to destroy
  /// @param count The number of objects to destroy
  inline void contiguous_destruct(T* begin, size_t count)
    noexcept(std::is_nothrow_destructible_v<T>)
  {
    if constexpr (!std::is_trivially_destructible_v<T>)
    {
      for (size_t i = 0; i < count; i++)
        begin[i].~T();
    }
  }
}

namespace clt::meta
{
  /// @brief Tag struct for constructing in place
  struct InPlaceT{};
}

namespace clt
{
  /// @brief Tag object for constructing in place
  inline constexpr meta::InPlaceT InPlace;
}

#endif //!HG_COLT_HELPER