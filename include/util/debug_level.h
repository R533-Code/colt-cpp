#ifndef HG_COLT_DEBUG_LEVEL
#define HG_COLT_DEBUG_LEVEL

namespace clt
{
  /// @brief The debug level of the 'colt' library
  enum debug_level
  {
    /// @brief No debug (for release configuration)
    COLT_NO_DEBUG,
    /// @brief Check assertions, contracts
    COLT_DEBUG_L1,
    /// @brief Same as COLT_DEBUG_L1, but with more checks
    COLT_DEBUG_L2
  };

#ifndef COLT_DEBUG_LEVEL
  /// @brief The debugging level of the Colt library
  inline constexpr debug_level ColtDebugLevel =
  #if defined(NDEBUG)
      COLT_NO_DEBUG;
  #else
      COLT_DEBUG_L1;
  #endif //!COLT_DEBUG_LEVEL
#else
  /// @brief The debugging level of the Colt library
  inline constexpr debug_level ColtDebugLevel = COLT_DEBUG_LEVEL;
#endif //!COLT_DEBUG_LEVEL

  /// @brief Check if the library is on debug configuration or not
  /// @return True if on debug
  consteval bool is_debug() noexcept
  {
    return ColtDebugLevel != COLT_NO_DEBUG;
  }

  /// @brief Check if the library is on release configuration or not
  /// @return True if not on debug
  consteval bool is_release() noexcept
  {
    return !is_debug();
  }
}

#endif //!HG_COLT_DEBUG_LEVEL