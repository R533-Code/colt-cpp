/*****************************************************************/ /**
 * @file   typedefs.h
 * @brief  Contains typedefs used throughout the library.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_NUM_TYPEDEFS
#define HG_NUM_TYPEDEFS

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <span>
#include <utility>
#include <concepts>
#include <type_traits>
#include <bit>
#include <ratio>
#include <numeric>
#include <compare>

#include <hedley.h>

#include <colt/coltcpp_export.h> // for COLTCPP_EXPORT
#include <colt/config.h>         // for endianness and architecture info

/// @brief Signed 8-bit integer
using i8 = int8_t;
/// @brief Unsigned 8-bit integer
using u8 = uint8_t;
/// @brief Signed 16-bit integer
using i16 = int16_t;
/// @brief Unsigned 16-bit integer
using u16 = uint16_t;
/// @brief Signed 32-bit integer
using i32 = int32_t;
/// @brief Unsigned 32-bit integer
using u32 = uint32_t;
/// @brief Signed 64-bit integer
using i64 = int64_t;
/// @brief Unsigned 64-bit integer
using u64 = uint64_t;

/// @brief 32-bit float
using f32 = float;
/// @brief 64-bit float
using f64 = double;

/*********************************************/
// MACROS AND UTILITIES
/*********************************************/

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
  inline void debug_break() noexcept
  {
    __DETAILS__COLT_DBREAK();
    std::abort();
  }

  /// @brief Represents a location in source code.
  /// Used to support older compilers.
  struct source_location
  {
    /// @brief The file name
    const char* file = "";
    /// @brief The function name
    const char* function = "";
    /// @brief The line number
    std::uint_least32_t line_;
    /// @brief The column number
    std::uint_least32_t column_;

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

/*********************************************/
// ASSERTIONS MACROS
/*********************************************/

namespace clt
{
  [[noreturn]]
  /// @brief Marks a branch as unreachable, printing an error on Debug build
  /// @param error The error message
  /// @param src The source code information
  inline void unreachable(
      const char* error, clt::source_location src = clt::source_location::current())
  {
    if constexpr (is_debug_build())
    {
      printf(
          "FATAL: Unreachable branch hit in function '%s' (line %u) in "
          "file:\n'%s'\n%s",
          src.function_name(), (u32)src.line(), src.file_name(), error);
    }
    debug_break();
    HEDLEY_UNREACHABLE();
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
          printf(
              "FATAL: Assertion failed in function '%s' (line %u) in "
              "file:\n'%s'\n%s",
              src.function_name(), (u32)src.line(), src.file_name(), message);
          error = true;
        }
        printf("%u) %s == false", (u32)(i + 1), array[i]->str);
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

#ifndef COLT_MSVC
  #define __DETAILS__COLT_TO_ASSUME(expr)
#else
  #define __DETAILS__COLT_TO_ASSUME(expr) , HEDLEY_ASSUME(expr)
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

namespace clt::details
{
  /// @brief Represents an Error (with runtime checks)
  class ErrorDebug
  {
    /// @brief The source location
    clt::source_location src;
    /// @brief True if represents an error
    bool iserror;
    /// @brief True if the error state was read at least once
    mutable bool is_checked = false;

    /// @brief Constructs an error with a specific state.
    /// The constructor is marked private so that the more readable success and error
    /// methods are used.
    /// @param is_error True if an error
    ErrorDebug(bool is_error, const clt::source_location& src)
        : src(src)
        , iserror(is_error)
    {
    }

    /// @brief Asserts that the state was read at least once
    void assert_checked() const noexcept
    {
      if (!is_checked)
        clt::unreachable("'Error' result must be checked!", src);
    }

  public:
    ErrorDebug(const ErrorDebug&)           = delete;
    ErrorDebug operator=(const ErrorDebug&) = delete;

    // Move Constructor (steals the state of 'move')
    ErrorDebug(ErrorDebug&& move) noexcept
        : iserror(move.iserror)
        , is_checked(std::exchange(move.is_checked, true))
    {
    }
    // Move assignment operator
    ErrorDebug& operator=(ErrorDebug&& move) noexcept
    {
      assert_checked();
      iserror    = move.iserror;
      is_checked = std::exchange(move.is_checked, true);
      return *this;
    }

    /// @brief Constructs a success
    /// @return State representing a success
    [[nodiscard]] static auto success(
        clt::source_location src = clt::source_location::current()) noexcept
    {
      return ErrorDebug(false, src);
    }
    /// @brief Constructs an error
    /// @return State representing an error
    [[nodiscard]] static auto error(
        clt::source_location src = clt::source_location::current()) noexcept
    {
      return ErrorDebug(true, src);
    }

    /// @brief Check if the state represents an error
    [[nodiscard]] constexpr bool is_error() const noexcept
    {
      is_checked = true;
      return iserror;
    }
    /// @brief Check if the state represents a success
    [[nodiscard]] constexpr bool is_success() const noexcept { return !is_error(); }

    /// @brief Check if the state represents a success
    [[nodiscard]] explicit constexpr operator bool() const noexcept
    {
      return is_success();
    }

    /// @brief Discards the value
    constexpr void discard() const noexcept { is_checked = true; }

    /// @brief Ensures the state was at least read once
    ~ErrorDebug() { assert_checked(); }
  };

  /// @brief Represents an Error (without any runtime checks)
  class ErrorRelease
  {
    /// @brief True if error
    bool iserror;

    /// @brief Constructs an error with a specific state.
    /// The constructor is marked private so that the more readable success and error
    /// methods are used.
    /// @param is_error True if an error
    constexpr ErrorRelease(bool is_error)
        : iserror(is_error)
    {
    }

  public:
    ErrorRelease(const ErrorRelease&)            = delete;
    ErrorRelease& operator=(const ErrorRelease&) = delete;

    // Default move constructor
    constexpr ErrorRelease(ErrorRelease&&) noexcept = default;
    // Default move assignment operator
    constexpr ErrorRelease& operator=(ErrorRelease&&) noexcept = default;

    /// @brief Constructs a success
    /// @return State representing a success
    [[nodiscard]] constexpr static auto success() noexcept
    {
      return ErrorRelease(false);
    }
    /// @brief Constructs an error
    /// @return State representing an error
    [[nodiscard]] constexpr static auto error() noexcept
    {
      return ErrorRelease(true);
    }

    /// @brief Check if the state represents an error
    [[nodiscard]] constexpr bool is_error() const noexcept { return iserror; }
    /// @brief Check if the state represents a success
    [[nodiscard]] constexpr bool is_success() const noexcept { return !iserror; }

    /// @brief Does nothing on Release config
    constexpr void discard() const noexcept {}

    /// @brief Check if the state represents a success
    [[nodiscard]] explicit constexpr operator bool() const noexcept
    {
      return is_success();
    }
  };
} // namespace clt::details

/// @brief Boolean that represents a success/failure state that must be checked.
using ErrorFlag = std::conditional_t<
    clt::is_debug_build(), clt::details::ErrorDebug, clt::details::ErrorRelease>;

namespace clt
{
  /// @brief Represents a span of dynamic extent
  /// @tparam T The type
  template<typename T>
  using Span = std::span<T, std::dynamic_extent>;
  /// @brief Represents a const span of dynamic extent
  /// @tparam T The type
  template<typename T>
  using View = std::span<std::add_const_t<T>, std::dynamic_extent>;
} // namespace clt

/*********************************************/
// BITS AND ENDIANNESS
/*********************************************/

namespace clt
{
  /// @brief The target endianness
  enum class TargetEndian : std::underlying_type_t<std::endian>
  {
    /// @brief Represents a little endian architecture
    little = (std::underlying_type_t<std::endian>)std::endian::little,
    /// @brief Represents a big endian architecture
    big = (std::underlying_type_t<std::endian>)std::endian::big,
    /// @brief Represents the native target architecture
    native = (std::underlying_type_t<std::endian>)std::endian::native
  };

  /// @brief Swaps the bytes of an integer (opposite endianness).
  /// @tparam T The unsigned integer type
  /// @param a The value whose bytes to swap
  /// @return The integer in the opposite endianness
  template<std::unsigned_integral T>
  constexpr T byteswap(T a) noexcept
  {
    if constexpr (sizeof(T) == 1)
      return a;
    if (!std::is_constant_evaluated())
    {
#ifdef COLT_MSVC
      if constexpr (sizeof(T) == 2)
        return _byteswap_ushort(a);
      if constexpr (sizeof(T) == 4)
        return _byteswap_ulong(a);
      if constexpr (sizeof(T) == 8)
        return _byteswap_uint64(a);
#elif defined(COLT_GNU) || defined(COLT_CLANG)
      if constexpr (sizeof(T) == 2)
        return __builtin_bswap16(a);
      if constexpr (sizeof(T) == 4)
        return __builtin_bswap32(a);
      if constexpr (sizeof(T) == 8)
        return __builtin_bswap64(a);
#else
      // Undefined behavior...
      union
      {
        u8 buffer[sizeof(T)];
        T u;
      } source, dest;
      source.u = a;

      for (size_t k = 0; k < sizeof(T); k++)
        dest.buffer[k] = source.buffer[sizeof(T) - k - 1];
      return dest.u;
#endif // COLT_MSVC
    }
    else if constexpr (sizeof(T) == 2)
      return static_cast<T>((a >> 8) | (a << 8));
    else if constexpr (sizeof(T) == 4)
      return static_cast<T>(
          ((a & 0x000000FF) << 24) | ((a & 0x0000FF00) << 8)
          | ((a & 0x00FF0000) >> 8) | ((a & 0xFF000000) >> 24));
    else if constexpr (sizeof(T) == 8)
      return static_cast<T>(
          ((a & 0x00000000000000FFULL) << 56) | ((a & 0x000000000000FF00ULL) << 40)
          | ((a & 0x0000000000FF0000ULL) << 24) | ((a & 0x00000000FF000000ULL) << 8)
          | ((a & 0x000000FF00000000ULL) >> 8) | ((a & 0x0000FF0000000000ULL) >> 24)
          | ((a & 0x00FF000000000000ULL) >> 40)
          | ((a & 0xFF00000000000000ULL) >> 56));
  }

  /// @brief Converts an unsigned integer from host endianness to little endian.
  /// This function is a no-op if the current host is little endian.
  /// @tparam T The unsigned integer type
  /// @param a The value to convert
  /// @return The integer encoded as little endian
  template<std::unsigned_integral T>
  constexpr T htol(T a) noexcept
  {
    using enum TargetEndian;
    static_assert(native == little || native == big, "Unknown endianness!");
    static_assert(sizeof(T) <= 8, "Invalid integer size!");

    if constexpr (sizeof(T) == 1 || native == little)
      return a;
    else
      return byteswap(a);
  }

  /// @brief Converts an unsigned integer from host endianness to big endian.
  /// This function is a no-op if the current host is big endian.
  /// @tparam T The unsigned integer type
  /// @param a The value to convert
  /// @return The integer encoded as big endian
  template<std::unsigned_integral T>
  constexpr T htob(T a) noexcept
  {
    using enum TargetEndian;
    static_assert(native == little || native == big, "Unknown endianness!");
    static_assert(sizeof(T) <= 8, "Invalid integer size!");

    if constexpr (sizeof(T) == 1 || native == big)
      return a;
    else
      return byteswap(a);
  }

  /// @brief Converts an unsigned integer from little endian host endianness.
  /// This function is a no-op if the current host is little endian.
  /// @tparam T The unsigned integer type
  /// @param a The value to convert
  /// @return The integer encoded as host endianness
  template<std::unsigned_integral T>
  constexpr T ltoh(T a) noexcept
  {
    using enum TargetEndian;
    static_assert(native == little || native == big, "Unknown endianness!");
    static_assert(sizeof(T) <= 8, "Invalid integer size!");

    if constexpr (sizeof(T) == 1 || native == little)
      return a;
    else
      return byteswap(a);
  }

  /// @brief Converts an unsigned integer from big endian to host endianness.
  /// This function is a no-op if the current host is big endian.
  /// @tparam T The unsigned integer type
  /// @param a The value to convert
  /// @return The integer encoded as host endianness
  template<std::unsigned_integral T>
  constexpr T btoh(T a) noexcept
  {
    using enum TargetEndian;
    static_assert(native == little || native == big, "Unknown endianness!");
    static_assert(sizeof(T) <= 8, "Invalid integer size!");

    if constexpr (sizeof(T) == 1 || native == big)
      return a;
    else
      return byteswap(a);
  }
} // namespace clt

/*********************************************/
// UNITS
/*********************************************/

namespace clt
{
  struct ratio
  {
    const std::intmax_t num;
    const std::intmax_t den;

    constexpr ratio(std::intmax_t num = 0, std::intmax_t den = 1) noexcept
        : num(den < 0 ? -num / std::gcd(num, den) : num / std::gcd(num, den))
        , den(den < 0 ? -den / std::gcd(num, den) : den / std::gcd(num, den))
    {
    }

    friend constexpr std::strong_ordering operator<=>(ratio a, ratio b) noexcept
    {
      return a.num * b.den <=> a.den * b.num;
    }
  };

  template<ratio Ratio>
  struct allocation_size
  {
    size_t size;

    template<clt::ratio Ratio2>
    friend constexpr auto operator+(
        const allocation_size a, const allocation_size<Ratio2> b) noexcept
    {
      if constexpr (Ratio < Ratio2)
        return allocation_size{
            a.size + b.size * Ratio.den * Ratio2.num / (Ratio2.den * Ratio.num)};
      else
        return allocation_size<Ratio2>{
            b.size + a.size * Ratio2.den * Ratio.num / (Ratio.den * Ratio2.num)};
    }
  };

  using bits = allocation_size<ratio{1, 8}>;
  consteval bits operator"" _bit(unsigned long long lit) noexcept
  {
    return {lit};
  }
  consteval bits operator"" _b(unsigned long long lit) noexcept
  {
    return {lit};
  }
  using bytes = allocation_size<ratio{1}>;
  consteval bytes operator"" _B(unsigned long long lit) noexcept
  {
    return {lit};
  }

  using kibibytes = allocation_size<ratio{1024ULL}>;
  consteval kibibytes operator"" _KiB(unsigned long long lit) noexcept
  {
    return {lit};
  }
  using kilobytes = allocation_size<ratio{1000ULL}>;
  consteval kilobytes operator"" _KB(unsigned long long lit) noexcept
  {
    return {lit};
  }

  using mebibytes = allocation_size<ratio{1024ULL * 1024ULL}>;
  consteval mebibytes operator"" _MiB(unsigned long long lit) noexcept
  {
    return {lit};
  }
  using megabytes = allocation_size<ratio{1000ULL * 1000ULL}>;
  consteval megabytes operator"" _MB(unsigned long long lit) noexcept
  {
    return {lit};
  }

  using gibibytes = allocation_size<ratio{1024ULL * 1024ULL * 1024ULL}>;
  consteval gibibytes operator"" _GiB(unsigned long long lit) noexcept
  {
    return {lit};
  }
  using gigabytes = allocation_size<ratio{1000ULL * 1000ULL * 1000ULL}>;
  consteval gigabytes operator"" _GB(unsigned long long lit) noexcept
  {
    return {lit};
  }

  using tebibytes = allocation_size<ratio{1024ULL * 1024ULL * 1024ULL * 1024ULL}>;
  consteval tebibytes operator"" _TiB(unsigned long long lit) noexcept
  {
    return {lit};
  }
  using terabytes = allocation_size<ratio{1000ULL * 1000ULL * 1000ULL * 1000ULL}>;
  consteval terabytes operator"" _TB(unsigned long long lit) noexcept
  {
    return {lit};
  }
} // namespace clt

/*********************************************/
// SCOPE GUARD
/*********************************************/

namespace clt::details
{
  template<typename Fun>
  /// @brief Helper for ON_SCOPE_EXIT
  struct ScopeGuard
  {
    /// @brief The function to execute
    Fun fn;

    /// @brief Takes in a lambda to execute
    /// @param fn The function to execute
    ScopeGuard(Fun&& fn) noexcept
        : fn(std::forward<Fun>(fn))
    {
    }

    /// @brief Runs the lambda in the destructor
    ~ScopeGuard() noexcept { fn(); }
  };

  /// @brief Helper for ON_SCOPE_EXIT
  enum class ScopeGuardOnExit
  {
  };

  template<typename Fun>
  /// @brief Helper for ON_SCOPE_EXIT
  ScopeGuard<Fun> operator+(ScopeGuardOnExit, Fun&& fn) noexcept
  {
    return ScopeGuard<Fun>(std::forward<Fun>(fn));
  }
} // namespace clt::details

/// @brief Register an action to be called at the end of the scope.
/// Example:
/// @code{.cpp}
/// {
///		auto hello = 10;
///		ON_SCOPE_EXIT {
///			std::cout << hello;
///		}; // <- do not forget the semicolon
/// }
/// @endcode
#define ON_SCOPE_EXIT                             \
  auto COLT_CONCAT(SCOPE_EXIT_HELPER, __LINE__) = \
      clt::details::ScopeGuardOnExit() + [&]()

#endif // !HG_NUM_TYPEDEFS
