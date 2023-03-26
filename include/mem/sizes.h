#ifndef HG_COLT_SIZES
#define HG_COLT_SIZES

#include <ratio>
#include "../meta/traits.h"
#include "../util/typedefs.h"

namespace clt
{
  /// @brief Byte ratio
  using B         = std::ratio<1, 1>;
  /// @brief Byte ratio
  using Byte      = std::ratio<1, 1>;
  /// @brief Kibibyte ratio
  using KiB       = std::ratio<1024, 1>;
  /// @brief Kibibyte ratio
  using KibiByte  = std::ratio<1024, 1>;
  /// @brief Mebibyte ratio
  using MiB       = std::ratio<1024 * 1024, 1>;
  /// @brief Mebibyte ratio
  using MebiByte  = std::ratio<1024 * 1024, 1>;
  /// @brief Gibibyte ratio
  using GiB       = std::ratio<1024 * 1024 * 1024, 1>;
  /// @brief Gibibyte ratio
  using GibiByte  = std::ratio<1024 * 1024 * 1024, 1>;  

  template<meta::StdRatio RatioT>
  /// @brief Class responsible of holding byte sizes
  /// @tparam RatioT The ratio to bytes
  struct size
  {
    /// @brief The count of bytes / RatioT::num
    u64 count;

    /// @brief The ratio to byte of the size
    using ratio = RatioT;

    /// @brief Constructs a size from a count of RatioT
    /// @param count The size
    constexpr size(u64 count) noexcept
      : count(count) {}

    template<typename rt> requires (RatioT::num < rt::num)
    /// @brief Constructs a size from another size only when the conversion is not lossy.
    /// Use size_cast for lossy conversions.
    /// @tparam rt The ratio of the other size
    /// @param count The other size
    constexpr size(size<rt> count) noexcept
      : count(count.count * rt::num) {}

    /// @brief Converts the size to a byte count
    /// @return Byte count
    constexpr u64 to_bytes() const noexcept { return count * RatioT::num; }

    /// @brief The value of the size
    /// @return The value of the size
    constexpr u64 value() const noexcept { return count; }

    template<typename rt>
    /// @brief Comparison operator==
    /// @tparam rt The ratio of the other size
    /// @param other The other size
    /// @return True if equal
    constexpr bool operator==(size<rt> other) const noexcept
    {
      //We would like to avoid overflow, so rather than comparing
      // to_bytes(), we convert to the biggest size.
      if constexpr (rt::num > RatioT::num)
        return other.count * rt::num / RatioT::num == count;
      else
        return count * RatioT::num / rt::num == other.count;
    }

    template<typename rt>
    /// @brief Comparison operator!=
    /// @tparam rt The ratio of the other size
    /// @param other The other size
    /// @return True if not equal
    constexpr bool operator!=(size<rt> other) const noexcept
    {
      if constexpr (rt::num > RatioT::num)
        return other.count * rt::num / RatioT::num != count;
      else
        return count * RatioT::num / rt::num != other.count;
    }

    template<typename rt>
    /// @brief Comparison operator<=
    /// @tparam rt The ratio of the other size
    /// @param other The other size
    /// @return True if less or equal
    constexpr bool operator<=(size<rt> other) const noexcept
    {
      if constexpr (rt::num > RatioT::num)
        return other.count * rt::num / RatioT::num <= count;
      else
        return count * RatioT::num / rt::num <= other.count;
    }

    template<typename rt>
    /// @brief Comparison operator>=
    /// @tparam rt The ratio of the other size
    /// @param other The other size
    /// @return True if greater or equal
    constexpr bool operator>=(size<rt> other) const noexcept
    {
      if constexpr (rt::num > RatioT::num)
        return other.count * rt::num / RatioT::num >= count;
      else
        return count * RatioT::num / rt::num >= other.count;
    }

    template<typename rt>
    /// @brief Comparison operator<
    /// @tparam rt The ratio of the other size
    /// @param other The other size
    /// @return True if less
    constexpr bool operator<(size<rt> other) const noexcept
    {
      if constexpr (rt::num > RatioT::num)
        return other.count * rt::num / RatioT::num < count;
      else
        return count * RatioT::num / rt::num < other.count;
    }

    template<typename rt>
    /// @brief Comparison operator>
    /// @tparam rt The ratio of the other size
    /// @param other The other size
    /// @return True if greater
    constexpr bool operator>(size<rt> other) const noexcept
    {
      if constexpr (rt::num > RatioT::num)
        return other.count * rt::num / RatioT::num > count;
      else
        return count * RatioT::num / rt::num > other.count;
    }

    template<meta::StdRatio U>
    friend struct size;
  };

  template<meta::StdRatio Ratio, meta::StdRatio From>
  /// @brief Converts from a size type to another one (performing lossy conversions)
  /// @tparam Ratio The ratio to convert to
  /// @tparam From The ratio to convert from
  /// @param value The value to convert
  /// @return The converted value
  constexpr size<Ratio> size_cast(size<From> value) noexcept
  {
    return (value.value() * From::num) / Ratio::num;
  }
  
  /// @brief Creates a Byte size
  /// @param i The size count
  /// @return Byte size
  constexpr size<B>   operator""_B(u64 i)   noexcept { return size<B>(i); }
  /// @brief Creates a Kibibyte size
  /// @param i The size count
  /// @return Kibibyte size
  constexpr size<KiB> operator""_KiB(u64 i) noexcept { return size<KiB>(i); }
  /// @brief Creates a Mebibyte size
  /// @param i The size count
  /// @return Mebibyte size
  constexpr size<MiB> operator""_MiB(u64 i) noexcept { return size<MiB>(i); }
  /// @brief Creates a Gibibyte size
  /// @param i The size count
  /// @return Gibibyte size
  constexpr size<GiB> operator""_GiB(u64 i) noexcept { return size<GiB>(i); }
}

#endif //!HG_COLT_SIZES