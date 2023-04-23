/** @file math.h
* Contains constexpr math functions and helpers.
*/

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
  template<std::forward_iterator ForwardIt>
  constexpr ForwardIt min_element(ForwardIt first, ForwardIt last)
  {
    if (first == last)
      return last;

    ForwardIt smallest = first;
    ++first;

    for (; first != last; ++first)
      if (*first < *smallest)
        smallest = first;

    return smallest;
  }

  template<std::forward_iterator ForwardIt>
  constexpr ForwardIt max_element(ForwardIt first, ForwardIt last)
  {
    if (first == last)
      return last;

    ForwardIt largest = first;
    ++first;

    for (; first != last; ++first)
      if (*largest < *first)
        largest = first;

    return largest;
  }

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

  template<typename T>
  constexpr T min(std::initializer_list<T> ilist)
  {
    return *clt::min_element(ilist.begin(), ilist.end());
  }

  template<meta::Integral Int>
  constexpr Int max(Int a, Int b) noexcept
  {
    return (a < b) ? b : a;
  }

  template<typename T>
  constexpr T max(std::initializer_list<T> ilist) noexcept
  {
    return *clt::max_element(ilist.begin(), ilist.end());
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

  template<meta::FloatingPoint Fp>
  constexpr Fp round(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
    {
      if (x >= (std::numeric_limits<Fp>::radix / std::numeric_limits<Fp>::epsilon()))
        return x;
      if (x <= -(std::numeric_limits<Fp>::radix / std::numeric_limits<Fp>::epsilon()))
        return x;

      if (x > 0)
      {
        auto floor_x = (Fp)(uintmax_t)x;
        if (x - floor_x >= static_cast<Fp>(0.5))
          floor_x += static_cast<Fp>(1.0);
        return floor_x;
      }

      if (x < 0)
        return -clt::round(-x);
      return x; //  x is 0.0, -0.0 or NaN
    }
    else
      return std::round(x);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  constexpr Fp round(Int x) noexcept
  {
    return clt::round(static_cast<Fp>(x));
  }

  template<meta::FloatingPoint Fp>
  //Forward declaration as 'ceil' makes use of 'floor'
  constexpr Fp floor(Fp x) noexcept;

  template<meta::FloatingPoint Fp>
  constexpr Fp ceil(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
    {
      if (x >= (std::numeric_limits<Fp>::radix / std::numeric_limits<Fp>::epsilon()))
        return x;
      if (x <= -(std::numeric_limits<Fp>::radix / std::numeric_limits<Fp>::epsilon()))
        return x;

      if (x > 0)
        return (Fp)((uintmax_t)x + 1);

      if (x < 0)
        return -clt::floor(-x);
      return x; // x is 0.0, -0.0 or NaN
    }    
    else
      return std::ceil(x);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  constexpr Fp ceil(Int x) noexcept
  {
    return clt::ceil(static_cast<Fp>(x));
  }

  template<meta::FloatingPoint Fp>
  constexpr Fp floor(Fp x) noexcept
  {
    if (std::is_constant_evaluated())
    {
      if (x >= (std::numeric_limits<Fp>::radix / std::numeric_limits<Fp>::epsilon()))
        return x;
      if (x <= -(std::numeric_limits<Fp>::radix / std::numeric_limits<Fp>::epsilon()))
        return x;

      if (x > 0)
        return (Fp)(uintmax_t)x;

      if (x < 0)
        return -clt::ceil(-x);
      return x; // x is 0.0, -0.0 or NaN
    }
    else
      return std::floor(x);
  }

  template<meta::FloatingPoint Fp = double, meta::Integral Int>
  constexpr Fp floor(Int x) noexcept
  {
    return clt::floor(static_cast<Fp>(x));
  }
}

#endif //!HG_COLT_MATH