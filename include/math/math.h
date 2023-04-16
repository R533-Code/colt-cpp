#ifndef HG_COLT_MATH
#define HG_COLT_MATH

#include <cmath>
#include <type_traits>
#include <concepts>
#include <numeric>
#include <numbers>

#include "../util/typedefs.h"
#include "../util/contracts.h"

namespace clt
{
  template<meta::Integral Int> requires std::is_signed_v<Int>
  /// @brief Returns the absolute value (distance from zero) of an integer
  /// @param value The value whose absolute value to compute
  /// @return The absolute value
  constexpr std::make_unsigned_t<Int> abs(Int value) noexcept
    COLT_PRE(value != std::numeric_limits<Int>::min())
  {    
    if (std::is_constant_evaluated())
    {
      if (value < 0)
        return static_cast<std::make_unsigned_t<Int>>(-value);
      return static_cast<std::make_unsigned_t<Int>>(value);
    }
    else
      return static_cast<std::make_unsigned_t<Int>>(std::abs(value));
  }
  COLT_POST()

  template<meta::FloatingPoint Flt>
  /// @brief Returns the absolute value (distance from zero) of a floating point
  /// @param value The value whose absolute value to compute
  /// @return The absolute value
  constexpr Flt abs(Flt value) noexcept
  {
    if (std::is_constant_evaluated())
    {
      if (value < static_cast<Flt>(0))
        return -value;
      return value;
    }
    else
      return std::abs(value);
  }

  template<meta::Integral Int>
  constexpr Int min(Int a, Int b) noexcept
  {
    return (b < a) ? b : a;
  }

  template<meta::Integral Int>
  constexpr Int max(Int a, Int b) noexcept
  {
    return (a < b) ? b : a;
  }

  template<meta::FloatingPoint Fp>
  constexpr Fp pow(Fp base, Fp power) noexcept
  {
    if (std::is_constant_evaluated())
    {
      if (power == static_cast<Fp>(0.0))
        return static_cast<Fp>(1.0);
      if (base == static_cast<Fp>(1.0))
        return static_cast<Fp>(1.0);
      if (power < static_cast<Fp>(0.0))
      {
        Fp result = static_cast<Fp>(1.0);
        while (power < static_cast<Fp>(0.0))
        {
          result /= base;
          power += static_cast<Fp>(1.0);
        }
        return result;
      }
      else
      {
        Fp result = static_cast<Fp>(1.0);
        while (power > static_cast<Fp>(0.0))
        {
          result *= base;
          power -= static_cast<Fp>(1.0);
        }
        return result;
      }
    }
    else
      return std::pow(base, power);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  constexpr Fp pow(Int what, Int power) noexcept
  {
    return clt::pow(static_cast<Fp>(what), static_cast<Fp>(power));
  }

  template<meta::FloatingPoint Fp>
  constexpr Fp exp(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
      return clt::pow(std::numbers::e_v<Fp>, x);
    else
      return std::exp(x);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  constexpr Fp exp(Int x) noexcept
  {
    return clt::exp(static_cast<Fp>(x));
  }

  template<meta::FloatingPoint Fp>
  constexpr Fp log(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
    {
      Fp x1 = (x - 1) / (x + 1);
      Fp accumulator = static_cast<Fp>(0.0);
      for (size_t k = 1; k < 100; k += 2)
        accumulator += clt::pow(x1, static_cast<Fp>(k)) / k;
      return accumulator * static_cast<Fp>(2.0);
    }
    else
      return std::log(x);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  constexpr Fp log(Int x) noexcept
  {
    return clt::log(static_cast<Fp>(x));
  }

  template<meta::FloatingPoint Fp>
  constexpr Fp log10(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
      return clt::log(x) / clt::log(static_cast<Fp>(10.0));
    else
      return std::log10(x);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  constexpr Fp log10(Int x) noexcept
  {
    return clt::log10(static_cast<Fp>(x));
  }

  template<meta::FloatingPoint Fp>
  constexpr Fp log2(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
      return clt::log(x) / clt::log(static_cast<Fp>(2.0));
    else
      return std::log2(x);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  constexpr Fp log2(Int x) noexcept
  {
    return clt::log2(static_cast<Fp>(x));
  }
}

#endif //!HG_COLT_MATH