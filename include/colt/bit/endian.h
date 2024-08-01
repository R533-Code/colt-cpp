/*****************************************************************//**
 * @file   endian.h
 * @brief  Contains utilities related to endianness.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_BIT_ENDIAN
#define HG_BIT_ENDIAN

#include <concepts>
#include <bit>

// defines COLT_LITTLE_ENDIAN or COLT_BIG_ENDIAN
#include "colt/macro/config.h"
#include "colt/num/typedefs.h"
#include "colt/macro/assert.h"

namespace clt::bit
{
  /// @brief The target endianness
  enum class TargetEndian
  {
    /// @brief Represents a little endian architecture
    little = std::endian::little,
    /// @brief Represents a big endian architecture
    big = std::endian::big,
    /// @brief Represents the native target architecture
    native = std::endian::native
  };

  /// @brief Generate a bit mask.
  /// As an example, bitmask<u8>(3) -> 0b0000'0111.
  /// @tparam Ty The resulting type
  /// @param one_count The number of ones in the bit mask
  /// @return Bit mask
  template<std::unsigned_integral Ty>
  constexpr Ty bitmask(u8 one_count) noexcept
  {
    return static_cast<Ty>(-(one_count != 0))
           & (static_cast<Ty>(-1) >> ((sizeof(Ty) * 8) - one_count));
  }

  /// @brief Swaps the bytes of an integer (opposite endianness).
  /// @tparam T The unsigned integer type
  /// @param a The value whose bytes to swap
  /// @return The integer in the opposite endianness
  template<std::unsigned_integral T>
  constexpr T byteswap(T a) noexcept
  {
    if constexpr (sizeof(T) == 1)
      return a;
#ifdef COLT_MSVC
    if constexpr (sizeof(T) == 2)
      return _byteswap_ushort(a);
    if constexpr (sizeof(T) == 4)
      return _byteswap_ulong(a);
    if constexpr (sizeof(T) == 8)
      return _byteswap_uint64(a);
#elif defined(COLT_GNU) || defined(COLT_CLANG)
    if constexpr (sizeof(T) == 2)
      return __builtin_bswap16(a);
    if constexpr (sizeof(T) == 4)
      return __builtin_bswap32(a);
    if constexpr (sizeof(T) == 8)
      return __builtin_bswap64(a);
#else
    // Undefined behavior...
    union
    {
      u8 buffer[sizeof(T)];
      T u;
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
      dest.u8[k] = source.buffer[sizeof(T) - k - 1];

    return dest.u;
#endif // COLT_MSVC
  }

  /// @brief Converts an unsigned integer from host endianness to little endian.
  /// This function is a no-op if the current host is little endian.
  /// @tparam T The unsigned integer type
  /// @param a The value to convert
  /// @return The integer encoded as little endian
  template<std::unsigned_integral T>
  constexpr T htol(T a) noexcept
  {
    using enum TargetEndian;
    static_assert(native == little || native == big, "Unknown endianness!");
    static_assert(sizeof(T) <= 8, "Invalid integer size!");

    if constexpr (sizeof(T) == 1 || native == little)
      return a;
    else
      return byteswap(a);
  }

  /// @brief Converts an unsigned integer from host endianness to big endian.
  /// This function is a no-op if the current host is big endian.
  /// @tparam T The unsigned integer type
  /// @param a The value to convert
  /// @return The integer encoded as big endian
  template<std::unsigned_integral T>
  constexpr T htob(T a) noexcept
  {
    using enum TargetEndian;
    static_assert(native == little || native == big, "Unknown endianness!");
    static_assert(sizeof(T) <= 8, "Invalid integer size!");

    if constexpr (sizeof(T) == 1 || native == big)
      return a;
    else
      return byteswap(a);
  }

  /// @brief Converts an unsigned integer from little endian host endianness.
  /// This function is a no-op if the current host is little endian.
  /// @tparam T The unsigned integer type
  /// @param a The value to convert
  /// @return The integer encoded as host endianness
  template<std::unsigned_integral T>
  constexpr T ltoh(T a) noexcept
  {
    using enum TargetEndian;
    static_assert(native == little || native == big, "Unknown endianness!");
    static_assert(sizeof(T) <= 8, "Invalid integer size!");

    if constexpr (sizeof(T) == 1 || native == little)
      return a;
    else
      return byteswap(a);
  }

  /// @brief Converts an unsigned integer from big endian to host endianness.
  /// This function is a no-op if the current host is big endian.
  /// @tparam T The unsigned integer type
  /// @param a The value to convert
  /// @return The integer encoded as host endianness
  template<std::unsigned_integral T>
  constexpr T btoh(T a) noexcept
  {
    using enum TargetEndian;
    static_assert(native == little || native == big, "Unknown endianness!");
    static_assert(sizeof(T) <= 8, "Invalid integer size!");

    if constexpr (sizeof(T) == 1 || native == big)
      return a;
    else
      return byteswap(a);
  }

  /// @brief Sign extends a number represented by 'n' bits
  /// @tparam T The underlying type to sign extend
  /// @param value The value (represented by 'n' bits)
  /// @param n The number of bit from which to sign extend
  /// @return The sign extended integer
  template<std::unsigned_integral T>
  constexpr std::make_signed_t<T> sign_extend(T value, u8 n)
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
} // namespace clt::bit

#endif // !HG_BIT_ENDIAN
