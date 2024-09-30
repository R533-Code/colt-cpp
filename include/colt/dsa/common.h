/*****************************************************************/ /**
 * @file   common.h
 * @brief  Contains common utilities and types used by data structures.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_DSA_COMMON
#define HG_DSA_COMMON

#include <concepts>
#include <functional>
#include <type_traits>
#include <compare>
#include <zpp_bits.h>
#include <fmt/format.h>

#include "colt/num/math.h"
#include "colt/typedefs.h"
#include "colt/meta/traits.h"
#include "colt/macro/assert.h"

namespace clt
{
  template<typename To, typename From>
  /// @brief Helper to converts a pointer to a type to a pointer to another type
  /// @tparam To The type to convert
  /// @tparam From The type to convert from
  /// @param frm The value to convert
  /// @return Converted value
  constexpr To ptr_to(From frm) noexcept
    requires std::is_pointer_v<To> && std::is_pointer_v<From>
  {
    return static_cast<To>(
        static_cast<meta::match_cv_t<std::remove_pointer_t<From>, void>*>(frm));
  }

  /// @brief Tag type for constructing an empty Option
  struct none_t
  {
  };

  /// @brief Tag object for constructing an empty Option
  static constexpr none_t None;

  /// @brief Tag struct for constructing an object in place
  struct in_place_t
  {
  };

  /// @brief Tag object for constructing an object in place
  inline constexpr in_place_t InPlace;

  /// @brief Tag struct for constructing errors in Expect
  struct error_t
  {
  };

  /// @brief Tag object for constructing errors in Expect
  inline constexpr error_t Error;

  namespace details
  {
    template<typename T>
    /// @brief Moves and destructs 'count' objects from a memory location to another.
    /// @tparam T The type of the object to move and destruct
    /// @param from Pointer to the objects to move then destruct
    /// @param to Pointer to where to move constructs the objects
    /// @param count The number of objects to move constructs
    constexpr void contiguous_destructive_move(T* from, T* to, size_t count) noexcept(
        std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>)
    {
      assert_true("Invalid arguments!", clt::math::abs(from - to) >= count);

      if (!std::is_constant_evaluated())
      {
        if constexpr (
            std::is_trivially_move_constructible_v<T>
            && std::is_trivially_destructible_v<T>)
        {
          std::memcpy(to, from, count * sizeof(T));
          return;
        }
      }
      for (size_t i = 0; i < count; i++)
      {
        new (to + i) T(std::move(from[i]));
        from[i].~T();
      }
    }

    template<typename T>
    /// @brief Moves 'count' objects from a memory location to another.
    /// @tparam T The type to move
    /// @param from Pointer to the objects to move
    /// @param to Pointer to where to move constructs the objects
    /// @param count The number of objects to move
    constexpr void contiguous_move(T* from, T* to, size_t count) noexcept(
        std::is_nothrow_move_constructible_v<T>)
    {
      assert_true("Invalid arguments!", clt::math::abs(from - to) >= count);
      if (!std::is_constant_evaluated())
      {
        if constexpr (std::is_trivially_move_constructible_v<T>)
        {
          std::memcpy(to, from, count * sizeof(T));
          return;
        }
      }
      for (size_t i = 0; i < count; i++)
        new (to + i) T(std::move(from[i]));
    }

    template<typename T, typename... Args>
    /// @brief Constructs 'count' objects 'where' by forwarding 'args' to the constructor
    /// @tparam T The type to construct
    /// @tparam ...Args The parameter pack
    /// @param where Pointer to where to constructs the objects
    /// @param count The count of objects to construct
    /// @param ...args The argument pack
    constexpr void contiguous_construct(
        T* where, size_t count,
        Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
      for (size_t i = 0; i < count; i++)
        new (where + i) T(std::forward<Args>(args)...);
    }

    template<typename T>
    /// @brief Copies 'count' objects from a memory location to another one.
    /// @tparam T The type to copy
    /// @param from Pointer to where to copy the objects from
    /// @param to Pointer to where to copy constructs the objects
    /// @param count The number of objects to copy constructs
    inline void contiguous_copy(const T* from, T* to, size_t count) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
    {
      if (!std::is_constant_evaluated())
      {
        if constexpr (
            std::is_trivially_move_constructible_v<T>
            && std::is_trivially_destructible_v<T>)
        {
          std::memcpy(to, from, count * sizeof(T));
          return;
        }
      }
      for (size_t i = 0; i < count; i++)
        new (to + i) T(from[i]);
    }

    template<typename T>
    /// @brief Destroys 'count' objects from 'begin'
    /// @tparam T The type to destroy
    /// @param begin Pointer to the objects to destroy
    /// @param count The number of objects to destroy
    inline void contiguous_destruct(T* begin, size_t count) noexcept(
        std::is_nothrow_destructible_v<T>)
    {
      if constexpr (!std::is_trivially_destructible_v<T>)
      {
        for (size_t i = 0; i < count; i++)
          begin[i].~T();
      }
    }
  } // namespace details
} // namespace clt

#endif // !HG_DSA_COMMON
