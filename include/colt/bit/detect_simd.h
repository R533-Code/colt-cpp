#ifndef HG_BIT_DETECT_SIMD
#define HG_BIT_DETECT_SIMD

#include "colt/macro/macro.h"
#include "colt/num/typedefs.h"

// We make use of an internal header of SIMDUTF
#include <simdutf/internal/isadetection.h>

#if defined(COLT_CLANG) || defined(COLT_GNU)
  #define COLT_ATTRIBUTE_TARGET(_target) __attribute__((target(_target)))
#else
  #define COLT_ATTRIBUTE_TARGET(target)
#endif // (defined(COLT_CLANG) || defined(COLT_GNU))

#if defined(COLT_x86_64)
  #define COLT_FORCE_AVX2  COLT_ATTRIBUTE_TARGET("avx2")
  #define COLT_FORCE_SSE2  COLT_ATTRIBUTE_TARGET("sse2")
  #define COLT_FORCE_SSE42 COLT_ATTRIBUTE_TARGET("sse4.2")
  // AVX-512 Foundation (F) � expands most 32-bit and 64-bit based AVX instructions
  // with the EVEX coding scheme to support 512-bit registers, operation masks,
  // parameter broadcasting, and embedded rounding and exception control.
  #define COLT_FORCE_AVX512F COLT_ATTRIBUTE_TARGET("avx512f")
  // AVX-512 Doubleword and Quadword Instructions (DQ) � adds new 32-bit and 64-bit
  // AVX-512 instructions.
  #define COLT_FORCE_AVX512DQ COLT_ATTRIBUTE_TARGET("avx512dq")
  // AVX-512 Integer Fused Multiply Add (IFMA) � fused multiply add of integers
  // using 52-bit precision.
  #define COLT_FORCE_AVX512IFMA COLT_ATTRIBUTE_TARGET("avx512ifma")
  // AVX-512 Prefetch Instructions (PF) � new prefetch capabilities.
  #define COLT_FORCE_AVX512PF
  // AVX-512 Exponential and Reciprocal Instructions (ER) � exponential and reciprocal
  // operations designed to help implement transcendental operations.
  #define COLT_FORCE_AVX512ER COLT_ATTRIBUTE_TARGET("avx512er")
  // AVX-512 Conflict Detection Instructions (CD) � efficient conflict detection
  // to allow more loops to be vectorized.
  #define COLT_FORCE_AVX512CD COLT_ATTRIBUTE_TARGET("avx512cd")
  // AVX-512 Vector Length Extensions (VL) � extends most AVX-512 operations
  // to also operate on XMM (128-bit) and YMM (256-bit) registers
  #define COLT_FORCE_AVX512VL COLT_ATTRIBUTE_TARGET("avx512vl")
  // Same as AVX-512VL but with bytes and words
  #define COLT_FORCE_AVX512BW COLT_ATTRIBUTE_TARGET("avx512bw")
  // AVX512 Vector population count instruction
  #define COLT_FORCE_AVX512VPOPCNTDQ COLT_ATTRIBUTE_TARGET("avx512vpopcntdq")
  // AVX-512 Vector Byte Manipulation Instructions 2 (VBMI2) � byte/word load, store and concatenation with shift.
  #define COLT_FORCE_AVX512VBMI2 COLT_ATTRIBUTE_TARGET("avx512vbmi2")
#endif // COLT_x86_64

namespace clt::bit
{
  using simd_flag = simdutf::internal::instruction_set;

  /// @brief Use to detect SIMD supported features
  /// @return simd_flag with the supported features marked as 1
  static inline simd_flag detect_supported_architectures() noexcept
  {
    static const auto value =
        simdutf::internal::detect_supported_architectures() | simd_flag::DEFAULT;
    return static_cast<simd_flag>(value);
  }

  /// @brief Chooses a function depending on supported features of the CPU.
  /// The result of this function must be cached for performance.
  /// @code{.cpp}
  /// size_t unitlen32(const char32_t* ptr) noexcept
  /// {
  ///   // The code below chooses the first supported function or return null
  ///   // if none are supported.
  ///   // The result is cached for performance.
  ///   static const auto FN = choose_simd_function<simd_flag::AVX512F, simd_flag::AVX2>(
  ///       &unitlen32AVX512F, &unitlen32AVX2);
  ///   return (*FN)(ptr);
  /// }
  /// @endcode
  /// @tparam ...FnPtrs Function pointer type pack
  /// @tparam FnPtr Function pointer type
  /// @tparam ...PREFERED The pack of simd_flag starting with most performant
  ///                     to least.
  /// @param first The first function pointer
  /// @param ...pack The pack of function pointers
  /// @return The first supported function (from left to right) or null if none are supported.
  template<simd_flag... PREFERED, typename FnPtr, typename... FnPtrs>
    requires(sizeof...(PREFERED) == sizeof...(FnPtrs) + 1)
            && meta::are_all_same<FnPtr, FnPtrs...>
  auto choose_simd_function(FnPtr first, FnPtrs... pack) noexcept
  {
    auto support                = detect_supported_architectures();
    constexpr size_t ARRAY_SIZE = sizeof...(PREFERED);
    simd_flag ARRAY[]           = {PREFERED...};
    FnPtr ARRAYFN[]             = {first, pack...};
    for (size_t i = 0; i < ARRAY_SIZE; i++)
      if (support & ARRAY[i])
        return ARRAYFN[i];
    return static_cast<FnPtr>(nullptr);
  }
} // namespace clt::bit
#endif // !HG_BIT_DETECT_SIMD