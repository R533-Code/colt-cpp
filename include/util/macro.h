#ifndef HG_COLT_MACRO
#define HG_COLT_MACRO

#if defined(__has_builtin)
  #if __has_builtin(__builtin_debugtrap)
    /// @brief Intrinsic trap
    #define colt_intrinsic_dbreak() __builtin_debugtrap()
  #elif __has_builtin(__debugbreak)
    /// @brief Intrinsic trap
    #define colt_intrinsic_dbreak() __debugbreak()
  #endif
#endif

#ifndef colt_intrinsic_dbreak
  #if defined(_MSC_VER) || defined(__INTEL_COMPILER)
    /// @brief Intrinsic trap
    #define colt_intrinsic_dbreak() __debugbreak()
  #else
    /// @brief Intrinsic trap
    #define colt_intrinsic_dbreak() do { (void)std::fgetc(stdin); std::exit(1); } while (0)
  #endif
#endif

#if defined(_MSC_VER)
  /// @brief Current function name
  #define COLT_FUNCTION_NAME __FUNCTION__
#else
  /// @brief Current function name
  #define COLT_FUNCTION_NAME __func__
#endif

/// @brief The line
#define COLT_LINE_NUM __LINE__
/// @brief The current filename
#define COLT_FILENAME __FILE__

/// @brief Internal concatenating macro
#define COLT_CONCAT_IMPL(a, b) a##b
/// @brief Concatenates 'a' and 'b'
#define COLT_CONCAT(a, b) COLT_CONCAT_IMPL(a, b)

#endif //!HG_COLT_MACRO