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
#include "colt/macro/config.h"
#include "colt/macro/assert.h"

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

#endif // !HG_NUM_TYPEDEFS
