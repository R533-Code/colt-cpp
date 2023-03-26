#ifndef HG_COLT_DEBUG_LEVEL
#define HG_COLT_DEBUG_LEVEL

namespace clt
{
  enum debug_level
  {
    COLT_NO_DEBUG,
    COLT_DEBUG_L1,
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
}

#endif //!HG_COLT_DEBUG_LEVEL