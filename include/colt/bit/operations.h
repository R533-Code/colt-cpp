/*****************************************************************//**
 * @file   operations.h
 * @brief  Contains bit operations.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_BIT_OPERATIONS
#define HG_BIT_OPERATIONS

#include <concepts>
#include <bit>

#include "colt/num/typedefs.h"
#include "colt/macro/assert.h"

namespace clt::bit
{
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
}

#endif // !HG_BIT_OPERATIONS
