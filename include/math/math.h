#ifndef HG_COLT_MATH
#define HG_COLT_MATH

#include <cmath>
#include <type_traits>
#include <concepts>
#include <numeric>

#include "../util/contracts.h"

namespace clt
{
  template<std::integral Int> requires std::is_signed_v<Int>
  /// @brief Returns the absolute value (distance from zero) of an integer
  /// @param value The value whose absolute value to compute
  /// @return The absolute value
  constexpr Int abs(Int value) noexcept
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

  template<std::floating_point Flt>
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
}

#endif //!HG_COLT_MATH