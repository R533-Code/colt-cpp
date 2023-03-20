#ifndef HG_COLT_CONTRACTS
#define HG_COLT_CONTRACTS

#include <cassert>
#include "./typedefs.h"
#include "../meta/traits.h"
#include "./macro.h"

namespace clt::details
{
  /// @brief Responsible of storing an expressions value and its source code string
  struct Assertion
  {
    /// @brief The string representing the value
    const char* str;
    /// @brief The value of the assertion
    bool value;
  };

  template<bool is_pre, typename... BoolTs> requires (sizeof...(BoolTs) != 0)
  /// @brief Asserts that multiple conditions are true, and if not,
  /// stops the application and prints the failed assertions.
  /// @tparam ...BoolTs The type parameter pack
  /// @param fn_name The function name
  /// @param ...bools The Assertion pack
  inline void assert_multiple(const char* fn_name, BoolTs... bools) noexcept
  {
    static_assert((std::is_same_v<Assertion, std::decay_t<BoolTs>> && ...),
      "This function expects 'Assertion'! Use COLT_PRE and COLT_POST rather than calling it directly!");

    Assertion* array[sizeof...(BoolTs)] = { &bools... };

    bool error = false;
    for (size_t i = 0; i < sizeof...(BoolTs); i++)
    {
      if (array[i]->value)
        continue;
      if constexpr (is_pre)
      {
        if (!error) {
          printf("CONTRACT FAILED (PRECONDITION %llu): in function:\n\"%s\":\n", i + 1, fn_name);
          error = true;
        }
        printf("%s => false\n", array[i]->str);
      }
      else
      {        
        if (!error) {
          printf("CONTRACT FAILED (POSTCONDITION %llu): in function:\n\"%s\":\n", i + 1, fn_name);
          error = true;
        }
        printf("%s => false\n", array[i]->str);
      }
    }
    if (error)
      colt_intrinsic_dbreak();
  }

  template<bool is_pre, typename... BoolTs> requires (sizeof...(BoolTs) == 0)
  /// @brief Overload of 'assert_multiple' that does nothing (for when no expression are to be asserted)
  /// @tparam ...BoolTs The type parameter pack
  /// @param fn_name The function name
  /// @param ...bools The value (for this overload, empty)
  inline void assert_multiple(const char* fn_name, BoolTs... bools) noexcept
  {
    //DO NOTHING
  }
}

/// @brief Helper for transforming assertions into strings and their evaluated value
#define COLT_TO_BOOL_STR_PAIR(expr) clt::details::Assertion{ #expr, expr }

/// @brief Creates a precondition for a function body
#define COLT_PRE(...) { clt::details::assert_multiple<true>(COLT_FUNCTION_NAME __VA_OPT__(,) COLT_FOR_EACH(COLT_TO_BOOL_STR_PAIR, __VA_ARGS__));
/// @brief Creates a postcondition for a function body
#define COLT_POST(...) clt::details::assert_multiple<false>(COLT_FUNCTION_NAME __VA_OPT__(,) COLT_FOR_EACH(COLT_TO_BOOL_STR_PAIR, __VA_ARGS__)); };

#undef COLT_TO_BOOL_STR_PAIR

#endif //!HG_COLT_CONTRACTS