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
#define COLT_DETAILS_CONCAT(a, b) a##b
/// @brief Concatenates 'a' and 'b'
#define COLT_CONCAT(a, b) COLT_DETAILS_CONCAT(a, b)

/// @brief Pair of ()
#define COLT_DETAILS_PARENS ()

/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_EXPAND(...)  COLT_DETAILS_EXPAND4(COLT_DETAILS_EXPAND4(COLT_DETAILS_EXPAND4(COLT_DETAILS_EXPAND4(__VA_ARGS__))))
/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_EXPAND4(...) COLT_DETAILS_EXPAND3(COLT_DETAILS_EXPAND3(COLT_DETAILS_EXPAND3(COLT_DETAILS_EXPAND3(__VA_ARGS__))))
/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_EXPAND3(...) COLT_DETAILS_EXPAND2(COLT_DETAILS_EXPAND2(COLT_DETAILS_EXPAND2(COLT_DETAILS_EXPAND2(__VA_ARGS__))))
/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_EXPAND2(...) COLT_DETAILS_EXPAND1(COLT_DETAILS_EXPAND1(COLT_DETAILS_EXPAND1(COLT_DETAILS_EXPAND1(__VA_ARGS__))))
/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_EXPAND1(...) __VA_ARGS__

/// @brief Applies 'macro' on each arguments
#define COLT_FOR_EACH(macro, ...)  \
  __VA_OPT__(COLT_DETAILS_EXPAND(COLT_DETAILS_FOR_EACH_HELPER(macro, __VA_ARGS__)))

/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_FOR_EACH_HELPER(macro, a1, ...) \
  macro(a1) \
  __VA_OPT__(COLT_DETAILS_FOR_EACH_AGAIN COLT_DETAILS_PARENS (macro, __VA_ARGS__))
/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_FOR_EACH_AGAIN() COLT_DETAILS_FOR_EACH_HELPER

/// @brief Applies 'macro' on each arguments, invoking 'macro(arg, <ARG>)'
#define COLT_FOR_EACH_1ARG(macro, arg, ...) \
  __VA_OPT__(COLT_DETAILS_EXPAND(COLT_DETAILS_FOR_EACH_HELPER_1ARG(macro, arg, __VA_ARGS__)))

/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_FOR_EACH_HELPER_1ARG(macro, arg, a1, ...) \
  macro(arg, a1) \
  __VA_OPT__(COLT_DETAILS_FOR_EACH_AGAIN_1ARG COLT_DETAILS_PARENS (macro, arg, __VA_ARGS__))

/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_FOR_EACH_AGAIN_1ARG() COLT_DETAILS_FOR_EACH_HELPER_1ARG

/// @brief Applies 'macro' on each arguments, invoking 'macro(arg1, arg2, <ARG>)'
#define COLT_FOR_EACH_2ARG(macro, arg1, arg2, ...) \
  __VA_OPT__(COLT_DETAILS_EXPAND(COLT_DETAILS_FOR_EACH_HELPER_2ARG(macro, arg1, arg2, __VA_ARGS__)))

/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_FOR_EACH_HELPER_2ARG(macro, arg1, arg2, a1, ...) \
  macro(arg1, arg2, a1) \
  __VA_OPT__(COLT_DETAILS_FOR_EACH_AGAIN_2ARG COLT_DETAILS_PARENS (macro, arg1, arg2, __VA_ARGS__))

/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_FOR_EACH_AGAIN_2ARG() COLT_DETAILS_FOR_EACH_HELPER_2ARG

/// @brief Applies 'macro' on each arguments, invoking 'macro(arg1, arg2, arg3, <ARG>)'
#define COLT_FOR_EACH_3ARG(macro, arg1, arg2, arg3, ...) \
  __VA_OPT__(COLT_DETAILS_EXPAND(COLT_DETAILS_FOR_EACH_HELPER_3ARG(macro, arg1, arg2, arg3, __VA_ARGS__)))

/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_FOR_EACH_HELPER_3ARG(macro, arg1, arg2, arg3, a1, ...) \
  macro(arg1, arg2, arg3, a1) \
  __VA_OPT__(COLT_DETAILS_FOR_EACH_AGAIN_3ARG COLT_DETAILS_PARENS (macro, arg1, arg2, arg3, __VA_ARGS__))

/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_FOR_EACH_AGAIN_3ARG() COLT_DETAILS_FOR_EACH_HELPER_3ARG

/// @brief Applies 'macro' on each arguments, invoking 'macro(arg1, arg2, arg3, arg4, <ARG>)'
#define COLT_FOR_EACH_4ARG(macro, arg1, arg2, arg3, arg4, ...) \
  __VA_OPT__(COLT_DETAILS_EXPAND(COLT_DETAILS_FOR_EACH_HELPER_4ARG(macro, arg1, arg2, arg3, arg4, __VA_ARGS__)))

/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_FOR_EACH_HELPER_4ARG(macro, arg1, arg2, arg3, arg4, a1, ...) \
  macro(arg1, arg2, arg3, arg4, a1) \
  __VA_OPT__(COLT_DETAILS_FOR_EACH_AGAIN_4ARG COLT_DETAILS_PARENS (macro, arg1, arg2, arg3, arg4, __VA_ARGS__))

/// @brief Helper for COLT_FOR_EACH_*
#define COLT_DETAILS_FOR_EACH_AGAIN_4ARG() COLT_DETAILS_FOR_EACH_HELPER_4ARG

#endif //!HG_COLT_MACRO