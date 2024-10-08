/*****************************************************************/ /**
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
#include "colt/typedefs.h"
#include "colt/macro/assert.h"

namespace clt::bit
{
  /// @brief The target endianness
  enum class TargetEndian : std::underlying_type_t<std::endian>
  {
    /// @brief Represents a little endian architecture
    little = (std::underlying_type_t<std::endian>)std::endian::little,
    /// @brief Represents a big endian architecture
    big = (std::underlying_type_t<std::endian>)std::endian::big,
    /// @brief Represents the native target architecture
    native = (std::underlying_type_t<std::endian>)std::endian::native
  };

  /// @brief Swaps the bytes of an integer (opposite endianness).
  /// @tparam T The unsigned integer type
  /// @param a The value whose bytes to swap
  /// @return The integer in the opposite endianness
  template<std::unsigned_integral T>
  constexpr T byteswap(T a) noexcept
  {
    if constexpr (sizeof(T) == 1)
      return a;
    if (!std::is_constant_evaluated())
    {
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
      source.u = a;

      for (size_t k = 0; k < sizeof(T); k++)
        dest.buffer[k] = source.buffer[sizeof(T) - k - 1];
      return dest.u;
#endif // COLT_MSVC
    }
    else if constexpr (sizeof(T) == 2)
      return static_cast<T>((a >> 8) | (a << 8));
    else if constexpr (sizeof(T) == 4)
      return static_cast<T>(
          ((a & 0x000000FF) << 24) | ((a & 0x0000FF00) << 8)
          | ((a & 0x00FF0000) >> 8) | ((a & 0xFF000000) >> 24));
    else if constexpr (sizeof(T) == 8)
      return static_cast<T>(
          ((a & 0x00000000000000FFULL) << 56) | ((a & 0x000000000000FF00ULL) << 40)
          | ((a & 0x0000000000FF0000ULL) << 24) | ((a & 0x00000000FF000000ULL) << 8)
          | ((a & 0x000000FF00000000ULL) >> 8) | ((a & 0x0000FF0000000000ULL) >> 24)
          | ((a & 0x00FF000000000000ULL) >> 40)
          | ((a & 0xFF00000000000000ULL) >> 56));
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
} // namespace clt::bit

#endif // !HG_BIT_ENDIAN
