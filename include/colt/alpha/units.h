#ifndef HG_COLT_UNITS
#define HG_COLT_UNITS

#include <ratio>
#include <cstdint>
#include <numeric>
#include <concepts>
#include <compare>
#include <type_traits>

#include <colt/typedefs.h>
#include <colt/macro/assert.h>

namespace clt
{
  struct ratio
  {
    const std::intmax_t num;
    const std::intmax_t den;

    constexpr ratio(std::intmax_t num = 0, std::intmax_t den = 1) noexcept
        : num(den < 0 ? -num / std::gcd(num, den) : num / std::gcd(num, den))
        , den(den < 0 ? -den / std::gcd(num, den) : den / std::gcd(num, den))
    {
    }

    friend constexpr std::strong_ordering operator<=>(ratio a, ratio b) noexcept
    {
      return a.num * b.den <=> a.den * b.num;
    }
  };

  template<ratio Ratio>
  struct allocation_size
  {
    size_t size;

    template<clt::ratio Ratio2>
    friend constexpr auto operator+(
        const allocation_size a, const allocation_size<Ratio2> b) noexcept
    {
      if constexpr (Ratio < Ratio2)
        return allocation_size{
            a.size + b.size * Ratio.den * Ratio2.num / (Ratio2.den * Ratio.num)};
      else
        return allocation_size<Ratio2>{
            b.size + a.size * Ratio2.den * Ratio.num / (Ratio.den * Ratio2.num)};
    }
  };

  using bits = allocation_size<ratio{1, 8}>;
  consteval bits operator"" _bit(unsigned long long lit) noexcept
  {
    return {lit};
  }
  consteval bits operator"" _b(unsigned long long lit) noexcept
  {
    return {lit};
  }
  using bytes = allocation_size<ratio{1}>;
  consteval bytes operator"" _B(unsigned long long lit) noexcept
  {
    return {lit};
  }

  using kibibytes = allocation_size<ratio{1024ULL}>;
  consteval kibibytes operator"" _KiB(unsigned long long lit) noexcept
  {
    return {lit};
  }
  using kilobytes = allocation_size<ratio{1000ULL}>;
  consteval kilobytes operator"" _KB(unsigned long long lit) noexcept
  {
    return {lit};
  }

  using mebibytes = allocation_size<ratio{1024ULL * 1024ULL}>;
  consteval mebibytes operator"" _MiB(unsigned long long lit) noexcept
  {
    return {lit};
  }
  using megabytes = allocation_size<ratio{1000ULL * 1000ULL}>;
  consteval megabytes operator"" _MB(unsigned long long lit) noexcept
  {
    return {lit};
  }

  using gibibytes = allocation_size<ratio{1024ULL * 1024ULL * 1024ULL}>;
  consteval gibibytes operator"" _GiB(unsigned long long lit) noexcept
  {
    return {lit};
  }
  using gigabytes = allocation_size<ratio{1000ULL * 1000ULL * 1000ULL}>;
  consteval gigabytes operator"" _GB(unsigned long long lit) noexcept
  {
    return {lit};
  }

  using tebibytes = allocation_size<ratio{1024ULL * 1024ULL * 1024ULL * 1024ULL}>;
  consteval tebibytes operator"" _TiB(unsigned long long lit) noexcept
  {
    return {lit};
  }
  using terabytes = allocation_size<ratio{1000ULL * 1000ULL * 1000ULL * 1000ULL}>;
  consteval terabytes operator"" _TB(unsigned long long lit) noexcept
  {
    return {lit};
  }
} // namespace clt

#endif // !HG_COLT_UNITS
