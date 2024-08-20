/*****************************************************************/ /**
 * @file   macro.h
 * @brief  Contains COLT_FOR_EACH, debug_break, and other macro utilities.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_MACRO_MACRO
#define HG_MACRO_MACRO

#include <cstdio>
#include <cstdlib>
#include <concepts>
#include <cstdint>

#include "hedley.h"
#include "config.h"

#if defined(__has_builtin)
  #if __has_builtin(__builtin_debugtrap)
    /// @brief Intrinsic trap
    #define __DETAILS__COLT_DBREAK() __builtin_debugtrap()
  #elif __has_builtin(__debugbreak)
    /// @brief Intrinsic trap
    #define __DETAILS__COLT_DBREAK() __debugbreak()
  #endif
#endif

#ifndef __DETAILS__COLT_DBREAK
  #if defined(_MSC_VER) || defined(__INTEL_COMPILER)
    /// @brief Intrinsic trap
    #define __DETAILS__COLT_DBREAK() __debugbreak()
  #else
    /// @brief Intrinsic trap
    #define __DETAILS__COLT_DBREAK() (void)std::fgetc(stdin), std::abort()
  #endif
#endif

#if defined(COLT_MSVC)
  #define CLT_EXPORT __declspec(dllexport)
  #define CLT_IMPORT __declspec(dllimport)
#elif defined(COLT_GNU) || defined(COLT_CLANG)
  #define CLT_EXPORT __attribute__((visibility("default")))
  #define CLT_IMPORT
#else
  #define CLT_EXPORT
  #define CLT_IMPORT
  #pragma warning "Unknown dynamic link import/export semantics."
#endif

/// @brief p => q
#define COLT_IMPLIES(p, q) !(bool)(p) || (q)

/// @brief Expands to default copy/move constructor/assignment operator
#define MAKE_DEFAULT_COPY_AND_MOVE_FOR(type)                 \
  constexpr type(type&&) noexcept                 = default; \
  constexpr type(const type&) noexcept            = default; \
  constexpr type& operator=(type&&) noexcept      = default; \
  constexpr type& operator=(const type&) noexcept = default;

/// @brief Expands to delete copy/move constructor/assignment operator
#define MAKE_DELETE_COPY_AND_MOVE_FOR(type) \
  type(type&&)                 = delete;    \
  type(const type&)            = delete;    \
  type& operator=(type&&)      = delete;    \
  type& operator=(const type&) = delete;

namespace clt
{
  HEDLEY_ALWAYS_INLINE
  constexpr bool implies(bool p, bool q) noexcept
  {
    return !p || q;
  }

  [[noreturn]]
  /// @brief Aborts the program (or call the debugger if possible)
  inline void
      debug_break() noexcept
  {
    __DETAILS__COLT_DBREAK();
    std::abort();
  }

  /// @brief Represents a location in source code.
  /// Used to support older compilers.
  class source_location
  {
    /// @brief The file name
    const char* file = "";
    /// @brief The function name
    const char* function = "";
    /// @brief The line number
    std::uint_least32_t line_;
    /// @brief The column number
    std::uint_least32_t column_;

  public:
    /// @brief Default constructor
    constexpr source_location() noexcept = default;

    /// @brief Constructor
    /// @param file The file name
    /// @param fn The function name
    /// @param line The line number
    /// @param column The column number
    constexpr source_location(
        const char* file, const char* fn, std::uint_least32_t line,
        std::uint_least32_t column) noexcept
        : file(file)
        , function(fn)
        , line_(line)
        , column_(column)
    {
    }

    MAKE_DEFAULT_COPY_AND_MOVE_FOR(source_location);

    /// @brief Returns the file name
    /// @return File name
    constexpr const char* file_name() const noexcept { return file; }
    /// @brief Returns the function name
    /// @return Function name
    constexpr const char* function_name() const noexcept { return function; }
    /// @brief Returns the line number
    /// @return The line number
    constexpr auto line() const noexcept { return line_; }
    /// @brief Returns the column
    /// @return The column
    constexpr auto column() const noexcept { return column_; }

#if defined(COLT_MSVC) || defined(COLT_CLANG) || defined(COLT_GNU)
    /// @brief Constructs a source_location using the current location
    /// @param file The file name
    /// @param fn The function name
    /// @param line The line number
    /// @param column The column number
    static constexpr source_location current(
        const char* file = __builtin_FILE(), const char* fn = __builtin_FUNCTION(),
        std::uint_least32_t line = __builtin_LINE(),
  #if defined(COLT_MSVC) || defined(COLT_CLANG)
        std::uint_least32_t column = __builtin_COLUMN()
  #else
        std::uint_least32_t column = 0
  #endif // COLT_MSVC || COLT_CLANG
            ) noexcept
#else
    /// @brief Constructs a source_location using the current location
    /// @param file The file name
    /// @param fn The function name
    /// @param line The line number
    /// @param column The column number
    static constexpr source_location current(
        const char* file = "unknown", const char* fn = "unknown",
        std::uint_least32_t line = 0, std::uint_least32_t column = 0) noexcept
#endif
    {
      return source_location(file, fn, line, column);
    }
  };
} // namespace clt

/// @brief Colt intrinsic debug break
#define colt_intrinsic_dbreak() clt::debug_break()

#if defined(_MSC_VER)
  /// @brief Current function name
  #define COLT_FUNCTION_NAME __FUNCSIG__
#elif defined(__clang__) || defined(__GNUC__)
  /// @brief Current function name
  #define COLT_FUNCTION_NAME __PRETTY_FUNCTION__
#else
  /// @brief Current function name
  #define COLT_FUNCTION_NAME __func__
#endif

/// @brief The line
#define COLT_LINE_NUM __LINE__
/// @brief The current filename
#define COLT_FILENAME __FILE__

/// @brief Internal concatenating macro
#define __DETAILS__COLT_CONCAT(a, b) a##b
/// @brief Concatenates 'a' and 'b'
#define COLT_CONCAT(a, b) __DETAILS__COLT_CONCAT(a, b)

/// @brief Pair of ()
#define __DETAILS__COLT_PARENS ()

/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_EXPAND(...)                \
  __DETAILS__COLT_EXPAND4(__DETAILS__COLT_EXPAND4( \
      __DETAILS__COLT_EXPAND4(__DETAILS__COLT_EXPAND4(__VA_ARGS__))))
/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_EXPAND4(...)               \
  __DETAILS__COLT_EXPAND3(__DETAILS__COLT_EXPAND3( \
      __DETAILS__COLT_EXPAND3(__DETAILS__COLT_EXPAND3(__VA_ARGS__))))
/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_EXPAND3(...)               \
  __DETAILS__COLT_EXPAND2(__DETAILS__COLT_EXPAND2( \
      __DETAILS__COLT_EXPAND2(__DETAILS__COLT_EXPAND2(__VA_ARGS__))))
/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_EXPAND2(...)               \
  __DETAILS__COLT_EXPAND1(__DETAILS__COLT_EXPAND1( \
      __DETAILS__COLT_EXPAND1(__DETAILS__COLT_EXPAND1(__VA_ARGS__))))
/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_EXPAND1(...) __VA_ARGS__

/// @brief Applies 'macro' on each arguments
#define COLT_FOR_EACH(macro, ...) \
  __VA_OPT__(                     \
      __DETAILS__COLT_EXPAND(__DETAILS__COLT_FOR_EACH_HELPER(macro, __VA_ARGS__)))

/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_FOR_EACH_HELPER(macro, a1, ...) \
  macro(a1) __VA_OPT__(                                 \
      __DETAILS__COLT_FOR_EACH_AGAIN __DETAILS__COLT_PARENS(macro, __VA_ARGS__))
/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_FOR_EACH_AGAIN() __DETAILS__COLT_FOR_EACH_HELPER

/// @brief Applies 'macro' on each arguments, invoking 'macro(arg, <ARG>)'
#define COLT_FOR_EACH_1ARG(macro, arg, ...) \
  __VA_OPT__(__DETAILS__COLT_EXPAND(        \
      __DETAILS__COLT_FOR_EACH_HELPER_1ARG(macro, arg, __VA_ARGS__)))

/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_FOR_EACH_HELPER_1ARG(macro, arg, a1, ...)            \
  macro(arg, a1)                                                             \
      __VA_OPT__(__DETAILS__COLT_FOR_EACH_AGAIN_1ARG __DETAILS__COLT_PARENS( \
          macro, arg, __VA_ARGS__))

/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_FOR_EACH_AGAIN_1ARG() __DETAILS__COLT_FOR_EACH_HELPER_1ARG

/// @brief Applies 'macro' on each arguments, invoking 'macro(arg1, arg2, <ARG>)'
#define COLT_FOR_EACH_2ARG(macro, arg1, arg2, ...) \
  __VA_OPT__(__DETAILS__COLT_EXPAND(               \
      __DETAILS__COLT_FOR_EACH_HELPER_2ARG(macro, arg1, arg2, __VA_ARGS__)))

/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_FOR_EACH_HELPER_2ARG(macro, arg1, arg2, a1, ...)     \
  macro(arg1, arg2, a1)                                                      \
      __VA_OPT__(__DETAILS__COLT_FOR_EACH_AGAIN_2ARG __DETAILS__COLT_PARENS( \
          macro, arg1, arg2, __VA_ARGS__))

/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_FOR_EACH_AGAIN_2ARG() __DETAILS__COLT_FOR_EACH_HELPER_2ARG

/// @brief Applies 'macro' on each arguments, invoking 'macro(arg1, arg2, arg3, <ARG>)'
#define COLT_FOR_EACH_3ARG(macro, arg1, arg2, arg3, ...) \
  __VA_OPT__(__DETAILS__COLT_EXPAND(                     \
      __DETAILS__COLT_FOR_EACH_HELPER_3ARG(macro, arg1, arg2, arg3, __VA_ARGS__)))

/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_FOR_EACH_HELPER_3ARG(macro, arg1, arg2, arg3, a1, ...) \
  macro(arg1, arg2, arg3, a1)                                                  \
      __VA_OPT__(__DETAILS__COLT_FOR_EACH_AGAIN_3ARG __DETAILS__COLT_PARENS(   \
          macro, arg1, arg2, arg3, __VA_ARGS__))

/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_FOR_EACH_AGAIN_3ARG() __DETAILS__COLT_FOR_EACH_HELPER_3ARG

/// @brief Applies 'macro' on each arguments, invoking 'macro(arg1, arg2, arg3, arg4, <ARG>)'
#define COLT_FOR_EACH_4ARG(macro, arg1, arg2, arg3, arg4, ...)            \
  __VA_OPT__(__DETAILS__COLT_EXPAND(__DETAILS__COLT_FOR_EACH_HELPER_4ARG( \
      macro, arg1, arg2, arg3, arg4, __VA_ARGS__)))

/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_FOR_EACH_HELPER_4ARG(                                \
    macro, arg1, arg2, arg3, arg4, a1, ...)                                  \
  macro(arg1, arg2, arg3, arg4, a1)                                          \
      __VA_OPT__(__DETAILS__COLT_FOR_EACH_AGAIN_4ARG __DETAILS__COLT_PARENS( \
          macro, arg1, arg2, arg3, arg4, __VA_ARGS__))

/// @brief Helper for COLT_FOR_EACH_*
#define __DETAILS__COLT_FOR_EACH_AGAIN_4ARG() __DETAILS__COLT_FOR_EACH_HELPER_4ARG

#endif // !HG_MACRO_MACRO
