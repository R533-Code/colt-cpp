/*****************************************************************/ /**
 * @file   assert.h
 * @brief  Contains 'assert_true', 'unreachable' and 'switch_no_default'.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_MACRO_ASSERT
#define HG_MACRO_ASSERT

#include "colt/macro/macro.h"
#include "colt/io/print.h"
#include <hedley.h>

namespace clt
{
  [[noreturn]]
  /// @brief Marks a branch as unreachable, printing an error on Debug build
  /// @param error The error message
  /// @param src The source code information
  inline void
      unreachable(
          const char* error,
          clt::source_location src = clt::source_location::current())
  {
    HEDLEY_UNREACHABLE();
    if constexpr (is_debug_build())
    {
      io::print_fatal(
          "Unreachable branch hit in function '{}' (line {}) in file:\n'{}'\n{}",
          src.function_name(), src.line(), src.file_name(), error);
    }
    debug_break();
  }
} // namespace clt

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
      const char* message, clt::source_location src, BoolTs... bools) noexcept
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

#ifndef COLT_CLANG
  #define __DETAILS__COLT_TO_ASSUME(expr) , (void)HEDLEY_ASSUME(expr)
#else
  #define __DETAILS__COLT_TO_ASSUME(expr)
#endif // COLT_CLANG


/// @brief Asserts that all condition are true
#define assert_true(MESSAGE, COND, ...)                                           \
  clt::details::assert_true_multiple(                                             \
      MESSAGE, clt::source_location::current() __DETAILS__COLT_TO_ASSERTION(COND) \
                   COLT_FOR_EACH(__DETAILS__COLT_TO_ASSERTION, __VA_ARGS__))      \
      __DETAILS__COLT_TO_ASSUME(COND)                                             \
          COLT_FOR_EACH(__DETAILS__COLT_TO_ASSUME, __VA_ARGS__)

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
