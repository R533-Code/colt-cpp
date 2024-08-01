#ifndef HG_MATH_MATH
#define HG_MATH_MATH

#include <cmath>
#include <numeric>
#include <concepts>
#include <algorithm>
#include <utility>

#include <gcem.hpp>

#define MAKE_CONSTEXPR_MATH_UNARY(name, concept_name) \
  template<concept_name T>                            \
  constexpr auto name(T value) noexcept               \
  {                                                   \
    if (std::is_constant_evaluated())                 \
      return gcem::name(value);                       \
    return std::name(value);                          \
  }

#define MAKE_CONSTEXPR_MATH_BINARY(name, concept_name) \
  template<concept_name T>                             \
  constexpr auto name(T first, T second) noexcept      \
  {                                                    \
    using type = decltype(std::name(first, second));   \
    if (std::is_constant_evaluated())                  \
      return (type)gcem::name(first, second);          \
    return std::name(first, second);                   \
  }

namespace clt::math
{
  MAKE_CONSTEXPR_MATH_UNARY(sin, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(asin, std::floating_point);

  MAKE_CONSTEXPR_MATH_UNARY(sinh, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(asinh, std::floating_point);

  MAKE_CONSTEXPR_MATH_UNARY(cos, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(acos, std::floating_point);

  MAKE_CONSTEXPR_MATH_UNARY(cosh, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(acosh, std::floating_point);

  MAKE_CONSTEXPR_MATH_UNARY(tan, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(atan, std::floating_point);

  MAKE_CONSTEXPR_MATH_BINARY(atan2, std::floating_point);

  MAKE_CONSTEXPR_MATH_UNARY(tanh, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(atanh, std::floating_point);

  MAKE_CONSTEXPR_MATH_UNARY(sqrt, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(sqrt, std::integral);

  MAKE_CONSTEXPR_MATH_UNARY(abs, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(abs, std::integral);

  MAKE_CONSTEXPR_MATH_BINARY(pow, std::floating_point);
  MAKE_CONSTEXPR_MATH_BINARY(pow, std::integral);

  MAKE_CONSTEXPR_MATH_UNARY(exp, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(exp, std::integral);

  MAKE_CONSTEXPR_MATH_UNARY(log, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(log, std::integral);

  MAKE_CONSTEXPR_MATH_UNARY(log10, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(log10, std::integral);

  MAKE_CONSTEXPR_MATH_UNARY(log2, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(log2, std::integral);

  MAKE_CONSTEXPR_MATH_UNARY(floor, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(ceil, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(round, std::floating_point);
  MAKE_CONSTEXPR_MATH_UNARY(trunc, std::floating_point);

  MAKE_CONSTEXPR_MATH_BINARY(fmod, std::floating_point);
  MAKE_CONSTEXPR_MATH_BINARY(hypot, std::floating_point);

  MAKE_CONSTEXPR_MATH_BINARY(gcd, std::integral);
  MAKE_CONSTEXPR_MATH_BINARY(lcm, std::integral);

  MAKE_CONSTEXPR_MATH_BINARY(min, std::integral);
  MAKE_CONSTEXPR_MATH_BINARY(max, std::integral);

  MAKE_CONSTEXPR_MATH_BINARY(min, std::floating_point);
  MAKE_CONSTEXPR_MATH_BINARY(max, std::floating_point);

  template<typename T>
  constexpr T min(std::initializer_list<T> ilist) noexcept
  {
    return std::min(ilist);
  }

  template<typename T>
  constexpr T max(std::initializer_list<T> ilist) noexcept
  {
    return std::max(ilist);
  }
} // namespace clt::math

#undef MAKE_CONSTEXPR_MATH_UNARY
#undef MAKE_CONSTEXPR_MATH_BINARY

#endif //!HG_MATH_MATH
