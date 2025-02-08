/*****************************************************************//**
 * @file   macros.h
 * @brief  Contains useful macros.
 * 
 * @author RPC
 * @date   January 2025
 *********************************************************************/
#ifndef _HG_COLT_MACROS_
#define _HG_COLT_MACROS_

#include <hedley/hedley.h>
#include "colt/config.h"

// From `https://meghprkh.github.io/blog/posts/c++-force-inline/`
#if defined(__clang__)
  #define COLT_FORCE_INLINE [[gnu::always_inline]] [[gnu::gnu_inline]] extern inline
#elif defined(__GNUC__)
  #define COLT_FORCE_INLINE [[gnu::always_inline]] inline
#elif defined(_MSC_VER)
  #pragma warning(error : 4714)
  #define COLT_FORCE_INLINE __forceinline
#else
  #error Unsupported compiler
#endif

#define COLT_UNREACHABLE() HEDLEY_UNREACHABLE()

#pragma region UGLY MACRO RECURSIONS HELPERS
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
#pragma endregion

#pragma region DYNAMIC LINK MACROS
#ifdef COLT_MSVC
  #define CLT_EXPORT __declspec(dllexport)
  #define CLT_IMPORT __declspec(dllimport)
#elif defined(COLT_GNU) || defined(COLT_CLANG)
  #define CLT_EXPORT __attribute__((visibility("default")))
  #define CLT_IMPORT
#else
  #define CLT_EXPORT
  #define CLT_IMPORT
  #error "Unknown dynamic link import/export semantics."
#endif
#pragma endregion

#pragma region HELPER MACROS
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

#if defined(__GNUC__) || defined(__clang__)
  #define __COLT_DO_PRAGMA(X) _Pragma(#X)
  /// @brief push a new scope for warnings
  #define COLT_DISABLE_WARNING_PUSH __COLT_DO_PRAGMA(GCC diagnostic push)
  /// @brief pop a the top scope for warnings
  #define COLT_DISABLE_WARNING_POP __COLT_DO_PRAGMA(GCC diagnostic pop)
  /// @brief disable a warning
  /// ("warning name" is for clang/gcc, warning number is for MSVC)
  #define COLT_DISABLE_WARNING(warningName, warningNumber) \
    __COLT_DO_PRAGMA(GCC diagnostic ignored warningName)
#elif defined(_MSC_VER)
  /// @brief push a new scope for warnings
  #define COLT_DISABLE_WARNING_PUSH __pragma(warning(push))
  /// @brief pop a the top scope for warnings
  #define COLT_DISABLE_WARNING_POP  __pragma(warning(pop))
  /// @brief disable a warning
  /// ("warning name" is for clang/gcc, warning number is for MSVC)
  #define COLT_DISABLE_WARNING(warningName, warningNumber) \
    __pragma(warning(disable : warningNumber))
#else
  // unknown compiler, ignoring suppression directives
  #define COLT_DISABLE_WARNING_PUSH
  #define COLT_DISABLE_WARNING_POP
  #define COLT_DISABLE_WARNING
#endif
#pragma endregion

#endif // !_HG_COLT_MACROS_
