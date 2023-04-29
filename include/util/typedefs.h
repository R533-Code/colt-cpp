/** @file typedefs.h
* Contains typedefs of built-in types.
*/

#ifndef HG_COLT_TYPEDEFS
#define HG_COLT_TYPEDEFS

#include <concepts>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <bit>
#include "../util/contracts.h"

template<typename T>
/// @brief Pointer
/// @tparam T The type pointed to by the pointer
using PTR = T*;
/// @brief 8-bit char
using char8 = char8_t;
/// @brief 16-bit char
using char16 = char16_t;
/// @brief 32-bit char
using char32 = char32_t;
/// @brief signed 8-bit integer
using i8 = int8_t;
/// @brief signed 16-bit integer
using i16 = int16_t;
/// @brief signed 32-bit integer
using i32 = int32_t;
/// @brief signed 64-bit integer
using i64 = int64_t;
/// @brief unsigned 8-bit integer
using u8 = uint8_t;
/// @brief unsigned 16-bit integer
using u16 = uint16_t;
/// @brief unsigned 32-bit integer
using u32 = uint32_t;
/// @brief unsigned 64-bit integer
using u64 = uint64_t;
/// @brief 1-bit integer
using bit = bool;
/// @brief 32-bit floating point
using f32 = float;
/// @brief 64-bit floating point
using f64 = double;

namespace clt
{
  namespace details
  {
    template<typename T> requires std::unsigned_integral<T> && (!std::same_as<T, bool>)
    /// @brief Class representing common byte sizes, simplifying bitwise operations
    class BitSet
    {
      /// @brief Underlying integer storing the value
      T value;

    public:
      /// @brief Constructs a BitSet with all the bits cleared (0)
      constexpr BitSet() noexcept
        : value(0) {}
      /// @brief Constructs a BitSet from an Integer
      /// @param value The value of the bit-set
      constexpr BitSet(T value) noexcept
        : value(value) {}
      constexpr BitSet(const BitSet&)             noexcept = default;    
      constexpr BitSet(BitSet&&)                  noexcept = default;
      constexpr BitSet& operator=(const BitSet&)  noexcept = default;
      constexpr BitSet& operator=(BitSet&&)       noexcept = default;
      
      /// @brief Returns the state of the nth-bit
      /// @param n The index of the bit (starting at 0)
      /// @return True if on (1)
      constexpr bit operator[](size_t n)  const noexcept { return (value >> n) & 1U; }
      /// @brief Check if the nth-bit is set (1)
      /// @param n The index of the bit (starting at 0)
      /// @return True if on (1)
      constexpr bool is_set(size_t n)     const noexcept { return (value >> n) & 1U; }
      /// @brief Check if the nth-bit is clear (0)
      /// @param n The index of the bit (starting at 0)
      /// @return True if off (0)
      constexpr bool is_clr(size_t n)     const noexcept { return !is_set(n); }
      /// @brief Check if all the bits are set
      /// @return True if all the bits are on (1)
      constexpr bool is_all_set()         const noexcept { return value == (T)(-1); }
      /// @brief Check if at least a bit is set
      /// @return True if at least a bit is on (1)
      constexpr bool is_any_set()         const noexcept { return std::popcount(value) != 0; }
      /// @brief Check if no bit is set
      /// @return True if no bit is on (1)
      constexpr bool is_none_set()        const noexcept { return value == 0; }

      /// @brief Sets the state of the nth-bit to 1
      /// @param n The index of the bit (starting at 0)
      constexpr void set_n(size_t n) noexcept
      COLT_PRE(n < sizeof(T) * 8)
        value |= 1UL << n;
      COLT_POST()
      
      /// @brief Clears the state of the nth-bit (sets the bit to 0)
      /// @param n The index of the bit (starting at 0)
      constexpr void clr_n(size_t n) noexcept
      COLT_PRE(n < sizeof(T) * 8)
        value &= ~(1UL << n);
      COLT_POST()

      /// @brief Toggles the state of the nth-bit (if 0 -> 1, if 1 -> 0)
      /// @param n The index of the bit (starting at 0)
      constexpr void tgl_n(size_t n) noexcept
      COLT_PRE(n < sizeof(T) * 8)
        value ^= 1UL << n;
      COLT_POST()

      /// @brief Changes the state of the nth-bit to 'to'
      /// @param n The index of the bit (starting at 0)
      /// @param to The value to set the bit to
      constexpr void chg_n(size_t n, bit to) noexcept
      COLT_PRE(n < sizeof(T) * 8)
        value ^= (-(T)to ^ value) & (1UL << n);
      COLT_POST()

      /// @brief Bitwise OR operator
      /// @param byte The bit-set with which to perform the operation
      /// @return New bit-set containing the result of the operation
      constexpr BitSet operator|(BitSet byte) const noexcept { return value | byte.value; }
      /// @brief Bitwise XOR operator
      /// @param byte The bit-set with which to perform the operation
      /// @return New bit-set containing the result of the operation
      constexpr BitSet operator^(BitSet byte) const noexcept { return value ^ byte.value; }
      /// @brief Bitwise AND operator
      /// @param byte The bit-set with which to perform the operation
      /// @return New bit-set containing the result of the operation
      constexpr BitSet operator&(BitSet byte) const noexcept { return value & byte.value; }
      /// @brief Shift Left operator
      /// @param by By how many bits to shift
      /// @return New bit-set containing the result of the operation
      constexpr BitSet operator<<(size_t by) const noexcept
      COLT_PRE(by < sizeof(T) * 8)
        return value << by;
      COLT_POST()
      
      /// @brief Shift Right operator
      /// @param by By how many bits to shift
      /// @return New bit-set containing the result of the operation
      constexpr BitSet operator>>(size_t by) const noexcept
      COLT_PRE(by < sizeof(T) * 8)
        return value >> by;
      COLT_POST()
      
      /// @brief Bitwise NOT operator
      /// @return New bit-set containing the result of the operation
      constexpr BitSet operator~() const noexcept { return ~value; }
      
      /// @brief Bitwise OR operator
      /// @param byte The bit-set with which to perform the operation
      /// @return Self
      constexpr BitSet& operator|=(BitSet byte) noexcept { value |= byte.value; return *this; }
      /// @brief Bitwise XOR operator
      /// @param byte The bit-set with which to perform the operation
      /// @return Self
      constexpr BitSet& operator^=(BitSet byte) noexcept { value ^= byte.value; return *this; }
      /// @brief Bitwise AND operator
      /// @param byte The bit-set with which to perform the operation
      /// @return Self
      constexpr BitSet& operator&=(BitSet byte) noexcept { value &= byte.value; return *this; }
      /// @brief Shift Left operator
      /// @param by By how many bits to shift
      /// @return Self
      constexpr BitSet& operator<<=(size_t by) noexcept 
      COLT_PRE(by < sizeof(T) * 8)
      {
        value <<= by; return *this;
      }
      COLT_POST()
      
      /// @brief Shift Right operator
      /// @param by By how many bits to shift
      /// @return Self
      constexpr BitSet& operator>>=(size_t by) noexcept
      COLT_PRE(by < sizeof(T) * 8)
      {
        value >>= by; return *this;
      }
      COLT_POST()

      /// @brief Counts the number of set bits
      /// @return The number of bits that are set (=1)
      constexpr size_t count() const noexcept { return std::popcount(value); }

      /// @brief Returns the underlying integer storing the bit-set
      /// @return Reference to the underlying integer
      constexpr T&        to_underlying() noexcept { return value; }
      /// @brief Returns the underlying integer storing the bit-set
      /// @return Reference to the underlying integer
      constexpr const T&  to_underlying() const noexcept { return value; }

      /// @brief Sets all the bits to 0
      /// @return Self
      constexpr BitSet& clear() noexcept { value = 0; return *this; }

      template<typename From>
      constexpr BitSet bit_assign(From frm) noexcept
      {
        std::memcpy(&value, &frm, sizeof(From));
        return *this;
      }

      template<typename To>
      constexpr To as() const noexcept
      {
        To ret;
        std::memcpy(&ret, &value, sizeof(To));
        return ret;
      }

      /// @brief Underlying integer type storing the value
      using underlying_type = T;
    };
  }

  /// @brief 8-bit BitSet
  using BYTE_t = details::BitSet<u8>;
  /// @brief 16-bit BitSet
  using WORD_t = details::BitSet<u16>;
  /// @brief 32-bit BitSet
  using DWORD_t = details::BitSet<u32>;
  /// @brief 64-bit BitSet
  using QWORD_t = details::BitSet<u64>;

  template<typename T>
  concept BitType = std::same_as<T, BYTE_t>
    || std::same_as<T, WORD_t>
    || std::same_as<T, DWORD_t>    
    || std::same_as<T, QWORD_t>;

  template<BitType T, typename Wt> requires (sizeof(Wt) == sizeof(T))
  /// @brief Converts a type to its bit-set equivalent
  /// @tparam Wt The type to convert
  /// @tparam T One of [BYTE, WORD, DWORD, QWORD]
  /// @param what The value to convert
  /// @return The converted value
  constexpr T bit_as(const Wt& what) noexcept
  {
    using cnv_t = typename T::underlying_type;
    return T(std::bit_cast<cnv_t>(what));
  }

  namespace meta
  {
    template<typename T>
    /// @brief Unsigned integral
    concept UnsignedIntegral = std::same_as<std::decay_t<T>, u8>
      || std::same_as<std::decay_t<T>, u16>
      || std::same_as<std::decay_t<T>, u32>
      || std::same_as<std::decay_t<T>, u64>;

    template<typename T>
    /// @brief Signed integral
    concept SignedIntegral = std::same_as<std::decay_t<T>, i8>
      || std::same_as<std::decay_t<T>, i16>
      || std::same_as<std::decay_t<T>, i32>
      || std::same_as<std::decay_t<T>, i64>;

    template<typename T>
    /// @brief Signed/Unsigned integral
    concept Integral = UnsignedIntegral<T> || SignedIntegral<T>;

    template<typename T>
    /// @brief Floating point (f32, f64)
    concept FloatingPoint = std::same_as<std::decay_t<T>, f32>
      || std::same_as<std::decay_t<T>, f64>;
  }
}

#endif //!HG_TYPEDEFS