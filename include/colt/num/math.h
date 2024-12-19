/*****************************************************************/ /**
 * @file   math.h
 * @brief  Contains mathematical functions that are constexpr.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_MATH_MATH
#define HG_MATH_MATH

#include <cmath>
#include <numeric>
#include <concepts>
#include <algorithm>
#include <utility>

#include <gcem.hpp>
#include <colt/typedefs.h>

#define MAKE_CONSTEXPR_MATH_UNARY(name, concept_name) \
  template<concept_name T>                            \
  constexpr auto name(T value) noexcept               \
  {                                                   \
    if (std::is_constant_evaluated())                 \
      return gcem::name(value);                       \
    return std::name(value);                          \
  }

#define MAKE_CONSTEXPR_MATH_BINARY(name, concept_name) \
  template<concept_name T>                             \
  constexpr auto name(T first, T second) noexcept      \
  {                                                    \
    using type = decltype(std::name(first, second));   \
    if (std::is_constant_evaluated())                  \
      return (type)gcem::name(first, second);          \
    return std::name(first, second);                   \
  }

namespace clt
{
  MAKE_CONSTEXPR_MATH_UNARY(sin, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(asin, std::floating_point);

  MAKE_CONSTEXPR_MATH_UNARY(sinh, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(asinh, std::floating_point);

  MAKE_CONSTEXPR_MATH_UNARY(cos, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(acos, std::floating_point);

  MAKE_CONSTEXPR_MATH_UNARY(cosh, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(acosh, std::floating_point);

  MAKE_CONSTEXPR_MATH_UNARY(tan, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(atan, std::floating_point);

  MAKE_CONSTEXPR_MATH_BINARY(atan2, std::floating_point);

  MAKE_CONSTEXPR_MATH_UNARY(tanh, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(atanh, std::floating_point);

  MAKE_CONSTEXPR_MATH_UNARY(sqrt, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(sqrt, std::integral);

  MAKE_CONSTEXPR_MATH_UNARY(abs, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(abs, std::integral);

  MAKE_CONSTEXPR_MATH_BINARY(pow, std::floating_point);
  MAKE_CONSTEXPR_MATH_BINARY(pow, std::integral);

  MAKE_CONSTEXPR_MATH_UNARY(exp, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(exp, std::integral);

  MAKE_CONSTEXPR_MATH_UNARY(log, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(log, std::integral);

  MAKE_CONSTEXPR_MATH_UNARY(log10, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(log10, std::integral);

  MAKE_CONSTEXPR_MATH_UNARY(log2, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(log2, std::integral);

  MAKE_CONSTEXPR_MATH_UNARY(floor, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(ceil, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(round, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(trunc, std::floating_point);

  MAKE_CONSTEXPR_MATH_BINARY(fmod, std::floating_point);
  MAKE_CONSTEXPR_MATH_BINARY(hypot, std::floating_point);

  MAKE_CONSTEXPR_MATH_BINARY(gcd, std::integral);
  MAKE_CONSTEXPR_MATH_BINARY(lcm, std::integral);

  MAKE_CONSTEXPR_MATH_BINARY(min, std::integral);
  MAKE_CONSTEXPR_MATH_BINARY(max, std::integral);

  MAKE_CONSTEXPR_MATH_BINARY(min, std::floating_point);
  MAKE_CONSTEXPR_MATH_BINARY(max, std::floating_point);

  template<typename T>
  constexpr T min(std::initializer_list<T> ilist) noexcept
  {
    return std::min(ilist);
  }

  template<typename T>
  constexpr T max(std::initializer_list<T> ilist) noexcept
  {
    return std::max(ilist);
  }

  /// @brief Count the number of ones in an unsigned value
  /// @tparam T The type
  /// @param value The values whose ones to count
  /// @return The count of bit set to 1 in 'value'
  template<std::unsigned_integral T>
  constexpr size_t count_ones(T value) noexcept
  {
    return (size_t)std::popcount(value);
  }

  /// @brief Count the number of zeroes in an unsigned value
  /// @tparam T The type
  /// @param value The values whose zeroes to count
  /// @return The count of bit set to 0 in 'value'
  template<std::unsigned_integral T>
  constexpr size_t count_zeroes(T value) noexcept
  {
    return sizeof(T) * 8 - (size_t)std::popcount(value);
  }

  /// @brief Computes a circular shift left
  /// @tparam T The type
  /// @param x The value whose bits to shift
  /// @param s The amount by which to shift left
  /// @return rotl(x,s)
  template<std::unsigned_integral T>
  constexpr T circular_sl(T x, int s) noexcept
  {
    return std::rotl(x, s);
  }

  /// @brief Computes a circular shift right
  /// @tparam T The type
  /// @param x The value whose bits to shift
  /// @param s The amount by which to shift right
  /// @return rotr(x,s)
  template<std::unsigned_integral T>
  constexpr T circular_sr(T x, int s) noexcept
  {
    return std::rotr(x, s);
  }

  /// @brief Generate a bit mask.
  /// As an example, bitmask<u8>(3) -> 0b0000'0111.
  /// @tparam Ty The resulting type
  /// @param one_count The number of ones in the bit mask
  /// @return Bit mask
  template<std::unsigned_integral Ty>
  constexpr Ty bitmask(size_t one_count) noexcept
  {
    assert_true("Invalid count of ones for bit mask!", one_count <= sizeof(Ty) * 8);
    return static_cast<Ty>(-(one_count != 0))
           & (static_cast<Ty>(-1) >> ((sizeof(Ty) * 8) - one_count));
  }

  /// @brief Sign extends a number represented by 'n' bits
  /// @tparam T The underlying type to sign extend
  /// @param value The value (represented by 'n' bits)
  /// @param n The number of bit from which to sign extend
  /// @return The sign extended integer
  template<std::unsigned_integral T>
  constexpr std::make_signed_t<T> sext(T value, u8 n)
  {
    assert_true("Invalid bit count!", n > 0 && n < sizeof(T) * 8);
    T sign = (1 << (n - 1)) & value;
    T mask = ((~0U) >> (n - 1)) << (n - 1);
    if (sign != 0)
      value |= mask;
    else
      value &= ~mask;
    return static_cast<std::make_signed_t<T>>(value);
  }
} // namespace clt::math

#undef MAKE_CONSTEXPR_MATH_UNARY
#undef MAKE_CONSTEXPR_MATH_BINARY

#endif //!HG_MATH_MATH
