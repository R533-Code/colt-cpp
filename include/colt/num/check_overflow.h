#ifndef HG_NUM_CHECK_OVERFLOW
#define HG_NUM_CHECK_OVERFLOW

#include <concepts>
#include <limits>

#include "typedefs.h"
#include "hedley.h"
#include "colt/macro/config.h"

#ifdef COLT_MSVC
  #include <intrin.h>
#endif

namespace clt::num
{
  /// @brief Computes the sum of two numbers, and returns true on overflow
  /// @tparam T The integer type
  /// @param a The first integer
  /// @param b The second integer
  /// @param result The result to which to write
  /// @return True on overflow
  template<std::integral T>
  constexpr bool checked_add(T a, T b, T* result) noexcept
  {
    static_assert(sizeof(T) <= 8);
#if defined(COLT_GNU) || defined(COLT_CLANG)
    if (!std::is_constant_evaluated())
      return __builtin_add_overflow(a, b, result);
#elif defined(COLT_MSVC)
    // TODO: intrinisic only work on x86
    if (!std::is_constant_evaluated())
    {
      if constexpr (std::same_as<u8, T>)
        return static_cast<bool>(_addcarry_u8(0, a, b, result));
      if constexpr (std::same_as<u16, T>)
        return static_cast<bool>(_addcarry_u16(0, a, b, result));
      if constexpr (std::same_as<u32, T>)
        return static_cast<bool>(_addcarry_u32(0, a, b, result));
      if constexpr (std::same_as<u64, T>)
        return static_cast<bool>(_addcarry_u64(0, a, b, result));
    }
#endif
    if constexpr (sizeof(T) < 8)
    {
      T cast  = (T)a + (T)b;
      *result = (T)cast;
      return cast > std::numeric_limits<T>::max()
             || cast < std::numeric_limits<T>::min();
    }
    else if constexpr (std::same_as<i64, T>)
    {
      *result = a + b;
      return (b > 0 && a > std::numeric_limits<T>::max() - b)
             || (b < 0 && a < std::numeric_limits<T>::min() - b);
    }
    else // u64
    {
      T cast  = (T)a + (T)b;
      *result = cast;
      return cast < a;
    }
  }

  /// @brief Computes the difference of two numbers, and returns true on overflow
  /// @tparam T The integer type
  /// @param a The first integer
  /// @param b The second integer
  /// @param result The result to which to write
  /// @return True on overflow
  template<std::integral T>
  constexpr bool checked_sub(T a, T b, T* result) noexcept
  {
    static_assert(sizeof(T) <= 8);
#if defined(COLT_GNU) || defined(COLT_CLANG)
    if (!std::is_constant_evaluated())
      return __builtin_sub_overflow(a, b, result);
#elif defined(COLT_MSVC)
    // TODO: intrinisic only work on x86
    if (!std::is_constant_evaluated())
    {
      if constexpr (std::same_as<u8, T>)
        return static_cast<bool>(_subborrow_u8(0, a, b, result));
      if constexpr (std::same_as<u16, T>)
        return static_cast<bool>(_subborrow_u16(0, a, b, result));
      if constexpr (std::same_as<u32, T>)
        return static_cast<bool>(_subborrow_u32(0, a, b, result));
      if constexpr (std::same_as<u64, T>)
        return static_cast<bool>(_subborrow_u64(0, a, b, result));
    }
#endif
    if constexpr (sizeof(T) < 8)
    {
      T cast  = (T)a - (T)b;
      *result = (T)cast;
      return cast > std::numeric_limits<T>::max()
             || cast < std::numeric_limits<T>::min();
    }
    else if constexpr (std::same_as<i64, T>)
    {
      *result = a - b;
      return (b < 0 && a > std::numeric_limits<T>::max() + b)
             || (b > 0 && a < std::numeric_limits<T>::min() + b);
    }
    else // u64
    {
      T cast  = (T)a - (T)b;
      *result = cast;
      return cast > a;
    }
  }

  /// @brief Computes the product of two numbers, and returns true on overflow
  /// @tparam T The integer type
  /// @param a The first integer
  /// @param b The second integer
  /// @param result The result to which to write
  /// @return True on overflow
  template<std::integral T>
  constexpr bool checked_mul(T a, T b, T* result) noexcept
  {
    static_assert(sizeof(T) <= 8);
#if defined(COLT_GNU) || defined(COLT_CLANG)
    if (!std::is_constant_evaluated())
      return __builtin_mul_overflow(a, b, result);
#elif defined(COLT_MSVC)
    // TODO: intrinisic only work on x86
    if (!std::is_constant_evaluated())
    {
      if constexpr (std::same_as<u64, T>)
      {
        *result = a * b;
        return static_cast<bool>(__umulh(a, b) != 0);
      }
      if constexpr (std::same_as<i64, T>)
      {
        *result = a * b;
        return static_cast<bool>(__mulh(a, b) != 0);
      }
    }
#endif
    if constexpr (sizeof(T) < 8)
    {
      T cast  = (T)a * (T)b;
      *result = (T)cast;
      return cast > std::numeric_limits<T>::max()
             || cast < std::numeric_limits<T>::min();
    }
    else if constexpr (std::same_as<i64, T>)
    {
      *result = (T)a * (T)b;
      return (a == -1 && b == std::numeric_limits<T>::min())
             || (b == -1 && a == std::numeric_limits<T>::min())
             || (b != 0 && a > std::numeric_limits<T>::max() / x)
             || (b != 0 && a < std::numeric_limits<T>::min() / x);
    }
    else // u64
    {
      T cast  = (T)a * (T)b;
      *result = cast;
      return a != 0 && cast / a != b;
    }
  }

  /// @brief Computes the product of two numbers, and returns true on overflow.
  /// If 'b' is zero, this function will still perform the division.
  /// The only case where an overflow can happen is for signed division
  /// of std::numeric_limits<T>::min() and -1.
  /// @tparam T The integer type
  /// @param a The first integer
  /// @param b The second integer
  /// @param result The result to which to write
  /// @return True on overflow
  template<std::integral T>
  constexpr bool checked_div(T a, T b, T* result) noexcept
  {
    static_assert(sizeof(T) <= 8);
    *result = a / b;
    if constexpr (std::is_signed_v<T>)
      return (a == std::numeric_limits<T>::min() && b == -1)
             || (b == std::numeric_limits<T>::min() && a == -1);
    return false;
  }
} // namespace clt::num

#endif // !HG_NUM_CHECK_OVERFLOW
