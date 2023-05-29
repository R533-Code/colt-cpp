/** @file sizes.h
* Contains an abstraction for byte sizes.
*/

#ifndef HG_COLT_SIZES
#define HG_COLT_SIZES

#include <ratio>
#include "scn/scn.h"
#include "../meta/traits.h"
#include "../util/typedefs.h"
#include "../util/on_exit.h"

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

  template<meta::StdRatio RatioT = B>
  /// @brief Class responsible of holding byte sizes
  /// @tparam RatioT The ratio to bytes
  struct byte_size
  {
    /// @brief The count of bytes / RatioT::num
    u64 count = 0;

    /// @brief The ratio to byte of the size
    using ratio = RatioT;

    /// @brief Constructs a size of 0
    constexpr byte_size() noexcept = default;

    /// @brief Constructs a size from a count of RatioT
    /// @param count The size
    constexpr byte_size(u64 count) noexcept
      : count(count) {}

    template<typename rt> requires (RatioT::num < rt::num)
    /// @brief Constructs a size from another size only when the conversion is not lossy.
    /// Use size_cast for lossy conversions.
    /// @tparam rt The ratio of the other size
    /// @param count The other size
    constexpr byte_size(byte_size<rt> count) noexcept
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
    constexpr bool operator==(byte_size<rt> other) const noexcept
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
    constexpr bool operator!=(byte_size<rt> other) const noexcept
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
    constexpr bool operator<=(byte_size<rt> other) const noexcept
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
    constexpr bool operator>=(byte_size<rt> other) const noexcept
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
    constexpr bool operator<(byte_size<rt> other) const noexcept
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
    constexpr bool operator>(byte_size<rt> other) const noexcept
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
  constexpr byte_size<Ratio> size_cast(byte_size<From> value) noexcept
  {
    return (value.value() * From::num) / Ratio::num;
  }
  
  /// @brief Creates a Byte size
  /// @param i The size count
  /// @return Byte size
  constexpr byte_size<B>   operator""_B(unsigned long long int i)   noexcept { return byte_size<B>(i); }
  /// @brief Creates a Kibibyte size
  /// @param i The size count
  /// @return Kibibyte size
  constexpr byte_size<KiB> operator""_KiB(unsigned long long int i) noexcept { return byte_size<KiB>(i); }
  /// @brief Creates a Mebibyte size
  /// @param i The size count
  /// @return Mebibyte size
  constexpr byte_size<MiB> operator""_MiB(unsigned long long int i) noexcept { return byte_size<MiB>(i); }
  /// @brief Creates a Gibibyte size
  /// @param i The size count
  /// @return Gibibyte size
  constexpr byte_size<GiB> operator""_GiB(unsigned long long int i) noexcept { return byte_size<GiB>(i); }
}

template<>
struct scn::scanner<clt::byte_size<clt::B>>
  : scn::empty_parser
{
  template <typename Context>
  error scan(clt::byte_size<clt::B>& val, Context& ctx)
  {
    using namespace clt;
    
    u64 count;
    std::string_view str;
    auto r = scn::scan(ctx.range(), "{}{}", count, str);
    ON_SCOPE_EXIT{
      ctx.range() = std::move(r.range());
    };

    if (!r)
    {
      if (r.error().code() == error::value_out_of_range)
        return r.error();
      return { r.error().code(), "Expected an integer followed by 'B', 'KiB', 'MiB', or 'GiB'." };
    }
    StringView strv = str;
    if (strv.iequal("B"))
    {
      val = byte_size<B>{ count };
      return { error::good, nullptr };
    }
    if (strv.iequal("KiB"))
    {
      if (count > 18'014'398'509'481'984)
        return { error::value_out_of_range, "Value too great to be representable as bytes!" };
      val = byte_size<KiB>{ count };
      return { error::good, nullptr };
    }
    if (strv.iequal("MiB"))
    {
      if (count > 17'592'186'044'416)
        return { error::value_out_of_range, "Value too great to be representable as bytes!" };
      val = byte_size<MiB>{ count };
      return { error::good, nullptr };
    }
    if (strv.iequal("GiB"))
    {
      if (count > 17'179'869'184)
        return { error::value_out_of_range, "Value too great to be representable as bytes!" };
      val = byte_size<GiB>{ count };
      return { error::good, nullptr };
    }
    return { error::invalid_scanned_value, "Expected 'B', 'KiB', 'MiB', or 'GiB'." };
  }
};

template<>
struct fmt::formatter<clt::byte_size<clt::Byte>>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    assert_true("Possible format for byte_size is: {}!", ctx.begin() == ctx.end());
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const clt::byte_size<clt::Byte>& vec, FormatContext& ctx)
  {
    using namespace clt;
    if (vec.count % GiB::num == 0)
      return fmt::format_to(ctx.out(), "{}GiB", vec.count / GiB::num);
    if (vec.count % MiB::num == 0)
      return fmt::format_to(ctx.out(), "{}MiB", vec.count / MiB::num);
    if (vec.count % KiB::num == 0)
      return fmt::format_to(ctx.out(), "{}KiB", vec.count / KiB::num);
    return fmt::format_to(ctx.out(), "{}B", vec.count);
  }
};

#endif //!HG_COLT_SIZES