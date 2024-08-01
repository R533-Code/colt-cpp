#ifndef HG_MACRO_ASSERT
#define HG_MACRO_ASSERT

#include "colt/macro/macro.h"
#include "colt/io/print.h"

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

  /// @brief Used to cause a compile-time failure
  inline void constexpr_assert_true_failed() noexcept
  {
    //ASSERTION FAILED AT COMPILE TIME!
  }

  template<typename... BoolTs>
    requires(sizeof...(BoolTs) != 0)
  /// @brief Asserts that multiple conditions are true, and if not,
  /// stops the application and prints the failed assertions.
  /// @tparam ...BoolTs The type parameter pack
  /// @param message The message to print
  /// @param src The source location
  /// @param ...bools The Assertion pack
  constexpr void assert_true_multiple(
      const char* message, std::source_location src, BoolTs... bools) noexcept
  {
    static_assert(
        (std::is_same_v<Assertion, std::remove_cvref_t<BoolTs>> && ...),
        "This function expects 'Assertion'! Use assert_true rather than calling it "
        "directly!");
    if (std::is_constant_evaluated())
    {
      Assertion* array[sizeof...(BoolTs)] = {&bools...};
      for (size_t i = 0; i < sizeof...(BoolTs); i++)
      {
        if (array[i]->value)
          continue;
        constexpr_assert_true_failed();
      }
    }
    else if constexpr (is_debug_build())
    {
      Assertion* array[sizeof...(BoolTs)] = {&bools...};

      bool error = false;
      for (size_t i = 0; i < sizeof...(BoolTs); i++)
      {
        if (array[i]->value)
          continue;
        if (!error)
        {
          io::print_fatal(
              "Assertion failed in function '{}' (line {}) in file:\n'{}'\n{}",
              src.function_name(), src.line(), src.file_name(), message);
          error = true;
        }
        io::print("{}) {} == false", i + 1, array[i]->str);
      }
      if (error)
        colt_intrinsic_dbreak();
    }
  }
} // namespace clt::details

  /// @brief Helper for transforming assertions into strings and their evaluated value
  #define __DETAILS__COLT_TO_ASSERTION(expr) \
    , clt::details::Assertion                \
    {                                        \
      #expr, (expr)                          \
    }

  /// @brief Asserts that all condition are true
  #define assert_true(MESSAGE, COND, ...)                                           \
    clt::details::assert_true_multiple(                                             \
        MESSAGE, std::source_location::current() __DETAILS__COLT_TO_ASSERTION(COND) \
                     COLT_FOR_EACH(__DETAILS__COLT_TO_ASSERTION, __VA_ARGS__))

  /// @brief switch case with no default
  #define switch_no_default(...)                                    \
    switch (__VA_ARGS__)                                            \
    default:                                                        \
      if (true)                                                     \
      {                                                             \
        clt::unreachable("Invalid value for 'switch_no_default'."); \
      }                                                             \
      else

#endif // !HG_MACRO_ASSERT
