/*****************************************************************/ /**
 * @file   types.h
 * @brief  Contains typedefs used throughout the library.
 * 
 * @author RPC
 * @date   January 2025
 *********************************************************************/
#ifndef _HG_COLT_TYPES_
#define _HG_COLT_TYPES_

#include <cstdint>
#include <cstdio>
#include <compare>
#include <span>
#include <utility>
#include <type_traits>
#include <concepts>
#include <numeric>

#include "colt/config.h"
#include "colt2/macros.h"
#include "colt2/debugging.h"

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
  COLT_FORCE_INLINE
  constexpr bool implies(bool p, bool q) noexcept
  {
    return !p || q;
  }

  template<typename T, typename... Ts>
  /// @brief Returns true if at least one argument is equal to the first
  /// @param value The value to compare to
  /// @param comp The value to compare against
  /// @param ...other The values to compare against
  /// @return True if 'comp' or any of 'other' are equal to 'value'
  COLT_FORCE_INLINE constexpr bool is_one_of(
      const T& value, const T& comp, const Ts&... other)
  {
    return (value == comp) || ((value == other) || ...);
  }

  /// @brief Calls the debugger if possible or aborts.
  /// This should be used for unrecoverable errors such as
  /// bounds checks.
  COLTCPP_EXPORT void debug_break() noexcept;

  /// @brief Represents a location in source code.
  /// Used to support older compilers.
  struct source_location
  {
    /// @brief The file name
    const char* file = "";
    /// @brief The function name
    const char* function = "";
    /// @brief The line number
    std::uint32_t line_;
    /// @brief The column number
    std::uint32_t column_;

    /// @brief Default constructor
    constexpr source_location() noexcept = default;

    /// @brief Constructor
    /// @param file The file name
    /// @param fn The function name
    /// @param line The line number
    /// @param column The column number
    constexpr source_location(
        const char* file, const char* fn, std::uint32_t line,
        std::uint32_t column) noexcept
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
        std::uint32_t line = __builtin_LINE(),
  #if defined(COLT_MSVC) || defined(COLT_CLANG)
        std::uint32_t column = __builtin_COLUMN()
  #else
        std::uint32_t column = 0
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
        std::uint32_t line = 0, std::uint32_t column = 0) noexcept
#endif
    {
      return source_location(file, fn, line, column);
    }
  };

  /// @brief unsigned 8-bit integer
  using u8 = uint8_t;
  /// @brief unsigned 16-bit integer
  using u16 = uint16_t;
  /// @brief unsigned 32-bit integer
  using u32 = uint32_t;
  /// @brief unsigned 64-bit integer
  using u64 = uint64_t;
  /// @brief signed 8-bit integer
  using i8 = int8_t;
  /// @brief signed 16-bit integer
  using i16 = int16_t;
  /// @brief signed 32-bit integer
  using i32 = int32_t;
  /// @brief signed 64-bit integer
  using i64 = int64_t;
  /// @brief max-sized unsigned integer natively supported
  using usize = uintmax_t;
  /// @brief unsigned integer capable of storing an address
  using uptr = uintptr_t;
  /// @brief max-sized signed integer natively supported
  using isize = intmax_t;
  /// @brief signed integer capable of storing an address
  using iptr = intptr_t;
  /// @brief 32-bit floating point
  using f32 = float;
  /// @brief 64-bit floating point
  using f64 = double;

  /// @brief The build type
  enum class BuildType : unsigned char
  {
    /// @brief Debug build
    BUILD_DEBUG,
    /// @brief Release build
    BUILD_RELEASE,
    /// @brief The current build type
    current =
#ifdef COLT_DEBUG
        BUILD_DEBUG
#else
        BUILD_RELEASE
#endif // DEBUG
  };

  /// @brief The target endianness
  enum class TargeEndian : unsigned char
  {
    /// @brief Little endian
    LITTLE_ENDIAN,
    /// @brief Big endian
    BIG_ENDIAN,

    /// @brief The native endianness
    current =
#ifdef COLT_LITTLE_ENDIAN
        LITTLE_ENDIAN
#else
        BIG_ENDIAN
#endif // COLT_LITTLE_ENDIAN
  };

#ifdef COLT_DEBUG
  /// @brief Check if the current build is on DEBUG configuration
  /// @return True if on DEBUG
  [[nodiscard]]
  consteval bool is_debug_build() noexcept
  {
    return true;
  }
#else
  /// @brief Check if the current build is on DEBUG configuration
  /// @return True if on DEBUG
  [[nodiscard]]
  consteval bool is_debug_build() noexcept
  {
    return false;
  }
#endif // COLT_DEBUG

  /// @brief Check if the current build is on RELEASE configuration
  /// @return True if on RELEASE (not on DEBUG)
  [[nodiscard]]
  consteval bool is_release_build() noexcept
  {
    return !is_debug_build();
  }

#pragma region Target Enums

  /// @brief The target operating system
  enum class TargetOS : unsigned char
  {
    /// @brief Windows
    OS_WINDOWS,
    /// @brief Linux
    OS_LINUX,
    /// @brief Apple
    OS_APPLE,
    /// @brief The current target OS
    current =
#ifdef COLT_WINDOWS
        OS_WINDOWS
#elif defined(COLT_APPLE)
        OS_APPLE
#elif defined(COLT_LINUX)
        OS_LINUX
#else
        (unsigned char)-1
  #error "Unsupported platform!"
#endif
  };

  /// @brief The ISA of the target architecture
  enum class TargetArch : unsigned char
  {
    ARCH_x86_64,
    ARCH_x86_32,
    ARCH_ARM2,
    ARCH_ARM3,
    ARCH_ARM4T,
    ARCH_ARM5,
    ARCH_ARM6T2,
    ARCH_ARM6,
    ARCH_ARM7,
    ARCH_ARM7A,
    ARCH_ARM7R,
    ARCH_ARM7M,
    ARCH_ARM7S,
    ARCH_ARM64,
    ARCH_MIPS,
    ARCH_SUPERH,
    ARCH_POWERPC,
    ARCH_POWERPC64,
    ARCH_SPARC,
    ARCH_RV32,
    ARCH_RV64,
    ARCH_M68K,
    Unknown,

    current =
#if defined(__x86_64__) || defined(_M_X64)
        ARCH_x86_64
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
        ARCH_x86_32
#elif defined(__ARM_ARCH_2__)
        ARCH_ARM2
#elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
        ARCH_ARM3
#elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
        ARCH_ARM4T
#elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
        ARCH_ARM5
#elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
        ARCH_ARM6T2
#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__)   \
    || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) \
    || defined(__ARM_ARCH_6ZK__)
        ARCH_ARM6
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__)   \
    || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) \
    || defined(__ARM_ARCH_7S__)
        ARCH_ARM7
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) \
    || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        ARCH_ARM7A
#elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) \
    || defined(__ARM_ARCH_7S__)
        ARCH_ARM7R
#elif defined(__ARM_ARCH_7M__)
        ARCH_ARM7M
#elif defined(__ARM_ARCH_7S__)
        ARCH_ARM7S
#elif defined(__aarch64__) || defined(_M_ARM64)
        ARCH_ARM64
#elif defined(mips) || defined(__mips__) || defined(__mips)
        ARCH_MIPS
#elif defined(__sh__)
        ARCH_SUPERH
#elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) \
    || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__)        \
    || defined(_ARCH_PPC)
        ARCH_POWERPC
#elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
        ARCH_POWERPC64
#elif defined(__sparc__) || defined(__sparc)
        ARCH_SPARC
#elif (defined(__riscv) || defined(__riscv__))
  #if __riscv_xlen == 32
        ARCH_RV32
  #elif __riscv_xlen == 64
        ARCH_RV64
  #endif //  (__riscv_xlen == 32)
#elif defined(__m68k__)
        ARCH_M68K
#else
        Unknown
#endif
  };
#pragma endregion

  template<typename T>
  class ptr
  {
    T* ptr_;

  public:
    constexpr ptr() noexcept
        : ptr_(nullptr)
    {
    }
    constexpr ptr(T* ptr) noexcept
        : ptr_(ptr)
    {
    }
    MAKE_DEFAULT_COPY_AND_MOVE_FOR(ptr);

    [[nodiscard]]
    constexpr explicit operator const T*&() const noexcept
    {
      return ptr_;
    }
    [[nodiscard]]
    constexpr explicit operator T*&() noexcept
    {
      return ptr_;
    }
    [[nodiscard]]
    constexpr auto operator->() noexcept
    {
      return ptr_;
    }
    [[nodiscard]]
    constexpr auto operator->() const noexcept
    {
      return ptr_;
    }

    constexpr explicit operator bool() const noexcept { return ptr_ != nullptr; }
    constexpr bool operator!() const noexcept { return !*this; }

    friend constexpr auto operator==(const ptr& a, T* b) noexcept
    {
      return a.ptr_ == b;
    }
  };

  template<typename T>
  class notnull_ptr
  {
    T* ptr_;

  public:
    notnull_ptr() = delete;

    constexpr notnull_ptr(T* ptr) noexcept
        : ptr_(ptr)
    {
    }
    MAKE_DEFAULT_COPY_AND_MOVE_FOR(notnull_ptr);

    [[nodiscard]]
    constexpr explicit operator const T*&() const noexcept
    {
      return ptr_;
    }
    [[nodiscard]]
    constexpr explicit operator T*&() noexcept
    {
      return ptr_;
    }
    [[nodiscard]]
    constexpr operator ptr<T>() const noexcept
    {
      return ptr_;
    }
    [[nodiscard]]
    constexpr auto operator->() noexcept
    {
      return ptr_;
    }
    [[nodiscard]]
    constexpr auto operator->() const noexcept
    {
      return ptr_;
    }

    constexpr explicit operator bool() const noexcept { return ptr_ != nullptr; }
    constexpr bool operator!() const noexcept { return !*this; }

    friend constexpr auto operator==(const ptr& a, T* b) noexcept
    {
      return a.ptr_ == b;
    }
  };
} // namespace clt

/*********************************************/
// ASSERTIONS MACROS
/*********************************************/

namespace clt
{
  [[noreturn]]
  /// @brief Marks a branch as unreachable, printing an error on Debug build
  /// @param error The error message
  /// @param src The source code information
  void unreachable(
      const char* error, clt::source_location src = clt::source_location::current());
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
          std::printf(
              "FATAL: Assertion failed in function '%s' (line %u) in "
              "file:\n'%s'\n%s",
              src.function_name(), src.line(), src.file_name(), message);
          error = true;
        }
        std::printf("%u. %s evaluated to false", (u32)(i + 1), array[i]->str);
      }
      if (error)
        clt::debug_break();
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
    constexpr void discard() const noexcept
    {
      // Do nothing
    }

    /// @brief Check if the state represents a success
    [[nodiscard]] explicit constexpr operator bool() const noexcept
    {
      return is_success();
    }
  };
} // namespace clt::details

namespace clt
{
  /// @brief Boolean that represents a success/failure state that must be checked.
  using ErrorFlag = std::conditional_t<
      clt::is_debug_build(), clt::details::ErrorDebug, clt::details::ErrorRelease>;

  /// @brief Represents a span of dynamic extent
  /// @tparam T The type
  template<typename T>
  using Span = std::span<T, std::dynamic_extent>;
  /// @brief Represents a const span of dynamic extent
  /// @tparam T The type
  template<typename T>
  using View = std::span<std::add_const_t<T>, std::dynamic_extent>;

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
    const isize num;
    const isize den;

    constexpr ratio(isize num = 0, isize den = 1) noexcept
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

#endif // !_HG_COLT_TYPES_
