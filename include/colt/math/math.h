#ifndef HG_MATH_MATH
#define HG_MATH_MATH

#include <cmath>
#include <concepts>

#include <gcem.hpp>

#define MAKE_CONSTEXPR_MATH_FUNCTION(name, concept_name) \
  template<concept_name T>                               \
  constexpr auto name(T value) noexcept                  \
  {                                                      \
    if (std::is_constant_evaluated())                    \
      return gcem::name(value);                          \
    return std::name(value);                             \
  }

namespace clt::math
{
  MAKE_CONSTEXPR_MATH_FUNCTION(sin, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(asin, std::floating_point);

  MAKE_CONSTEXPR_MATH_FUNCTION(sinh, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(asinh, std::floating_point);

  MAKE_CONSTEXPR_MATH_FUNCTION(cos, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(acos, std::floating_point);

  MAKE_CONSTEXPR_MATH_FUNCTION(cosh, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(acosh, std::floating_point);

  MAKE_CONSTEXPR_MATH_FUNCTION(tan, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(atan, std::floating_point);

  MAKE_CONSTEXPR_MATH_FUNCTION(tanh, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(atanh, std::floating_point);

  MAKE_CONSTEXPR_MATH_FUNCTION(sqrt, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(sqrt, std::integral);

  MAKE_CONSTEXPR_MATH_FUNCTION(abs, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(abs, std::integral);

  MAKE_CONSTEXPR_MATH_FUNCTION(pow, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(pow, std::integral);

  MAKE_CONSTEXPR_MATH_FUNCTION(exp, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(exp, std::integral);

  MAKE_CONSTEXPR_MATH_FUNCTION(log, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(log, std::integral);

  MAKE_CONSTEXPR_MATH_FUNCTION(log10, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(log10, std::integral);

  MAKE_CONSTEXPR_MATH_FUNCTION(log2, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(log2, std::integral);

  MAKE_CONSTEXPR_MATH_FUNCTION(floor, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(ceil, std::floating_point);
  MAKE_CONSTEXPR_MATH_FUNCTION(round, std::floating_point);  
} // namespace clt::math

#undef MAKE_CONSTEXPR_MATH_FUNCTION

#endif //!HG_MATH_MATH
