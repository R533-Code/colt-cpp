#include "unicode.h"
#include "colt/bit/operations.h"
#include "colt/bit/detect_simd.h"

size_t strlen8default(const char8_t* ptr) noexcept
{
  size_t len = 0;
  auto end   = ptr;
  char8_t current;
  while ((current = *end) != u8'\0')
  {
    end += clt::uni::sequence_length(current);
    ++len;
  }
  return len;
}

size_t strlen16LEdefault(const char16_t* ptr) noexcept
{
  size_t len = 0;
  auto end   = ptr;
  char16_t current;
  while ((current = (char16_t)(clt::bit::ltoh((u16)*end)) != u'\0'))
  {
    end += clt::uni::sequence_length(current);
    ++len;
  }
  return len;
}

size_t strlen16BEdefault(const char16_t* ptr) noexcept
{
  size_t len = 0;
  auto end   = ptr;
  char16_t current;
  while ((current = (char16_t)(clt::bit::btoh((u16)*end)) != u'\0'))
  {
    end += clt::uni::sequence_length(current);
    ++len;
  }
  return len;
}

size_t unitlen16default(const char16_t* ptr) noexcept
{
  const auto copy = ptr;
  while (*ptr != 0)
    ptr++;
  return copy - ptr;
}

size_t unitlen32default(const char32_t* ptr) noexcept
{
  const auto copy = ptr;
  while (*ptr != 0)
    ptr++;
  return copy - ptr;
}

#if defined(COLT_x86_64)

COLT_FORCE_SSE2 size_t strlen8SSE2(const char8_t* ptr) noexcept
{
  size_t len = 0;
  // Align pointer to 16 byte boundary to use aligned load
  // and avoid page faults.
  // We can't use sequence_length here as the goal is to align
  // and most likely adding sequence_length will not align the pointer.
  while (uintptr_t(ptr) % 16 != 0)
  {
    if (*ptr == '\0')
      return len;
    len += (size_t)(!clt::uni::is_trail(*ptr));
    ++ptr;
  }

  const __m128i zero = _mm_setzero_si128();
  const __m128i trail_mask  = _mm_set1_epi8((u8)0b1100'0000);
  const __m128i trail_value = _mm_set1_epi8((u8)0b1000'0000);
  constexpr auto PACK_COUNT = sizeof(__m128i) / sizeof(u8);
  unsigned int mask;
  while (true)
  {
    __m128i values   = _mm_load_si128(reinterpret_cast<const __m128i*>(ptr));
    __m128i cmp      = _mm_cmpeq_epi8(values, zero);
    mask             = _mm_movemask_epi8(cmp);
    // If we found NUL-terminator break.
    if (mask != 0)
      break;
    __m128i is_trail = _mm_and_si128(values, trail_mask);
    is_trail         = _mm_cmpeq_epi8(is_trail, trail_value);
    len += PACK_COUNT - std::popcount((unsigned int)_mm_movemask_epi8(is_trail));
    ptr += PACK_COUNT;
  }
  // We might have ended on trailing byte
  // so we can't really use sequence_length here.
  // In any cases, there are at most 16 byte to check.
  while (true)
  {
    if (*ptr == '\0')
      return len;
    len += (size_t)(!clt::uni::is_trail(*ptr));
    ++ptr;
  }
  clt::unreachable("programming error");
}

COLT_FORCE_AVX2 size_t strlen8AVX2(const char8_t* ptr) noexcept
{
  size_t len = 0;
  while (uintptr_t(ptr) % 32 != 0)
  {
    if (*ptr == '\0')
      return len;
    len += (size_t)(!clt::uni::is_trail(*ptr));
    ++ptr;
  }

  const __m256i zero        = _mm256_setzero_si256();
  const __m256i trail_mask  = _mm256_set1_epi8((u8)0b1100'0000);
  const __m256i trail_value = _mm256_set1_epi8((u8)0b1000'0000);
  constexpr auto PACK_COUNT = sizeof(__m256i) / sizeof(u8);
  unsigned int mask;
  while (true)
  {
    __m256i values = _mm256_load_si256(reinterpret_cast<const __m256i*>(ptr));
    __m256i cmp    = _mm256_cmpeq_epi8(values, zero);
    mask           = _mm256_movemask_epi8(cmp);
    // If we found NUL-terminator break.
    if (mask != 0)
      break;
    __m256i is_trail = _mm256_and_si256(values, trail_mask);
    is_trail         = _mm256_cmpeq_epi8(is_trail, trail_value);
    len += PACK_COUNT - std::popcount((unsigned int)_mm256_movemask_epi8(is_trail));
    ptr += PACK_COUNT;
  }
  // We might have ended on trailing byte
  // so we can't really use sequence_length here.
  // In any cases, there are at most 16 byte to check.
  while (true)
  {
    if (*ptr == '\0')
      return len;
    len += (size_t)(!clt::uni::is_trail(*ptr));
    ++ptr;
  }
  clt::unreachable("programming error");
}

COLT_FORCE_SSE2 size_t unitlen16SSE2(const char16_t* ptr) noexcept
{
  const auto copy = ptr;
  // Align pointer to 16 byte boundary to use aligned load
  // and avoid page faults.
  while (uintptr_t(ptr) % 16 != 0)
  {
    if (*ptr == 0)
      return ptr - copy;
    ++ptr;
  }

  const __m128i zero        = _mm_setzero_si128();
  constexpr auto PACK_COUNT = sizeof(__m128i) / sizeof(u16);
  unsigned int mask;
  while (true)
  {
    __m128i values = _mm_load_si128(reinterpret_cast<const __m128i*>(ptr));
    __m128i cmp    = _mm_cmpeq_epi16(values, zero);
    mask           = _mm_movemask_epi8(cmp);
    if (mask != 0)
      break;
    ptr += PACK_COUNT;
  }
  return (ptr - copy) + std::countr_zero(mask) / 2;
}

// unitlen using AVX2 SIMD instructions
COLT_FORCE_AVX2 size_t unitlen16AVX2(const char16_t* ptr) noexcept
{
  const auto copy = ptr;
  // Align pointer to 32 byte boundary to use aligned load
  // and avoid page faults.
  while (uintptr_t(ptr) % 32 != 0)
  {
    if (*ptr == 0)
      return ptr - copy;
    ++ptr;
  }

  const __m256i zero        = _mm256_setzero_si256();
  constexpr auto PACK_COUNT = sizeof(__m256i) / sizeof(u16);
  unsigned int mask;
  while (true)
  {
    __m256i values = _mm256_load_si256(reinterpret_cast<const __m256i*>(ptr));
    __m256i cmp    = _mm256_cmpeq_epi16(values, zero);
    mask           = _mm256_movemask_epi8(cmp);
    if (mask != 0)
      break;
    ptr += PACK_COUNT;
  }
  return (ptr - copy) + std::countr_zero(mask) / 2;
}

// unitlen using AVX512 SIMD instructions
COLT_FORCE_AVX512BW size_t unitlen16AVX512F(const char16_t* ptr) noexcept
{
  const auto copy = ptr;
  // Align pointer to 64 byte boundary to use aligned load
  while (uintptr_t(ptr) % 64 != 0)
  {
    if (*ptr == 0)
      return ptr - copy;
    ++ptr;
  }
  // Zero mask
  const __m512i zero        = _mm512_setzero_si512();
  constexpr auto PACK_COUNT = sizeof(__m512i) / sizeof(u16);
  __mmask32 mask;
  while (true)
  {
    __m512i values = _mm512_load_epi32(reinterpret_cast<const __m512i*>(ptr));
    mask           = _mm512_cmpeq_epi16_mask(values, zero);
    if (mask != 0)
      break;
    ptr += PACK_COUNT;
  }
  return (ptr - copy) + std::countr_zero(mask);
}

// ^^^ unitlen16
// vvv unitlen32

COLT_FORCE_SSE2 size_t unitlen32SSE2(const char32_t* ptr) noexcept
{
  const auto copy = ptr;
  // Align pointer to 16 byte boundary to use aligned load
  // and avoid page faults.
  while (uintptr_t(ptr) % 16 != 0)
  {
    if (*ptr == 0)
      return ptr - copy;
    ++ptr;
  }

  const __m128i zero        = _mm_setzero_si128();
  constexpr auto PACK_COUNT = sizeof(__m128i) / sizeof(u32);
  unsigned int mask;
  while (true)
  {
    __m128i values = _mm_load_si128(reinterpret_cast<const __m128i*>(ptr));
    __m128i cmp    = _mm_cmpeq_epi32(values, zero);
    mask           = _mm_movemask_epi8(cmp);
    if (mask != 0)
      break;
    ptr += PACK_COUNT;
  }
  return (ptr - copy) + std::countr_zero(mask) / 4;
}

// unitlen using AVX2 SIMD instructions
COLT_FORCE_AVX2 size_t unitlen32AVX2(const char32_t* ptr) noexcept
{
  const auto copy = ptr;
  // Align pointer to 32 byte boundary to use aligned load
  while (uintptr_t(ptr) % 32 != 0)
  {
    if (*ptr == 0)
      return ptr - copy;
    ++ptr;
  }
  // Zero mask
  const __m256i zero        = _mm256_setzero_si256();
  constexpr auto PACK_COUNT = sizeof(__m256i) / sizeof(u32);
  unsigned int mask;
  while (true)
  {
    __m256i values = _mm256_load_si256(reinterpret_cast<const __m256i*>(ptr));
    __m256i cmp    = _mm256_cmpeq_epi32(values, zero);
    mask           = _mm256_movemask_epi8(cmp);
    if (mask != 0)
      break;
    ptr += PACK_COUNT;
  }
  return (ptr - copy) + std::countr_zero(mask) / 4;
}

// unitlen using AVX512 SIMD instructions
COLT_FORCE_AVX512F size_t unitlen32AVX512F(const char32_t* ptr) noexcept
{
  const auto copy = ptr;
  // Align pointer to 64 byte boundary to use aligned load
  while (uintptr_t(ptr) % 64 != 0)
  {
    if (*ptr == 0)
      return ptr - copy;
    ++ptr;
  }
  // Zero mask
  const __m512i zero        = _mm512_setzero_si512();
  constexpr auto PACK_COUNT = sizeof(__m512i) / sizeof(u32);
  __mmask16 mask;
  while (true)
  {
    __m512i values = _mm512_load_epi32(reinterpret_cast<const __m512i*>(ptr));
    mask           = _mm512_cmpeq_epi32_mask(values, zero);
    if (mask != 0)
      break;
    ptr += PACK_COUNT;
  }
  return (ptr - copy) + std::countr_zero(mask);
}
#elif defined(COLT_ARM_7or8)

size_t unitlen16NEON(const char16_t* ptr) noexcept
{
  const auto copy = ptr;

  // Zero mask
  const uint16x8_t zero     = vdupq_n_u16(0);
  constexpr auto PACK_COUNT = sizeof(uint16x8_t) / sizeof(u32);
  u64 mask;
  while (true)
  {
    // See link below for vshrn
    // https://community.arm.com/arm-community-blogs/b/infrastructure-solutions-blog/posts/porting-x86-vector-bitmask-optimizations-to-arm-neon
    uint16x8_t values   = vld1q_u16(reinterpret_cast<const u16*>(ptr));
    uint16x8_t cmp      = vceqq_u16(values, zero);
    const uint8x8_t res = vshrn_n_u16(cmp, 8);
    mask                = vget_lane_u64(vreinterpret_u64_u8(res), 0);
    if (mask != 0)
      break;
    ptr += PACK_COUNT;
  }
  return (ptr - copy) + std::countr_zero(mask) / 2;
}

size_t unitlen32NEON(const char32_t* ptr) noexcept
{
  const auto copy = ptr;

  // Zero mask
  const uint32x4_t zero     = vdupq_n_u32(0);
  constexpr auto PACK_COUNT = sizeof(uint32x4_t) / sizeof(u32);
  u64 mask;
  while (true)
  {
    // See link below for vshrn
    // https://community.arm.com/arm-community-blogs/b/infrastructure-solutions-blog/posts/porting-x86-vector-bitmask-optimizations-to-arm-neon
    uint32x4_t values    = vld1q_u32(reinterpret_cast<const u32*>(ptr));
    uint32x4_t cmp       = vceqq_u32(values, zero);
    const uint16x4_t res = vshrn_n_u32(cmp, 8);
    mask                 = vget_lane_u64(vreinterpret_u64_u16(res), 0);
    if (mask != 0)
      break;
    ptr += PACK_COUNT;
  }
  return (ptr - copy) + std::countr_zero(mask) / 2;
}

#endif // COLT_x86_64

size_t clt::uni::details::strlen8(const char8_t* ptr) noexcept
{
  using namespace clt::bit;
#ifdef COLT_x86_64
  static const auto FN =
      choose_simd_function<simd_flag::AVX2, simd_flag::DEFAULT>(
          &strlen8AVX2, &strlen8SSE2);
  return (*FN)(ptr);
#else
  return strlen8default(ptr);
#endif // COLT_x86_64
}

size_t clt::uni::details::strlen16LE(const char16_t* ptr) noexcept
{
#ifdef COLT_x86_64
  return strlen16LEdefault(ptr);
#else
  return strlen16LEdefault(ptr);
#endif // COLT_x86_64
}

size_t clt::uni::details::strlen16BE(const char16_t* ptr) noexcept
{
#ifdef COLT_x86_64
  return strlen16BEdefault(ptr);
#else
  return strlen16BEdefault(ptr);
#endif // COLT_x86_64
}

size_t clt::uni::details::strlen16(const char16_t* ptr) noexcept
{
  if constexpr (StringEncoding::UTF16 == StringEncoding::UTF16LE)
    return strlen16LE(ptr);
  else
    return strlen16BE(ptr);
}

size_t clt::uni::details::unitlen16(const char16_t* ptr) noexcept
{
  using namespace clt::bit;
#ifdef COLT_x86_64
  static const auto FN =
      choose_simd_function<simd_flag::AVX512F, simd_flag::AVX2, simd_flag::DEFAULT>(
          &unitlen16AVX512F, &unitlen16AVX2, &unitlen16SSE2);
  return (*FN)(ptr);
#elif defined(COLT_ARM_7or8)
  static const auto FN = choose_simd_function<simd_flag::NEON, simd_flag::DEFAULT>(
      &unitlen16NEON, &unitlen16default);
  return (*FN)(ptr);
#else
  return unitlen16default(ptr);
#endif // COLT_x86_64
}

size_t clt::uni::details::unitlen32(const char32_t* ptr) noexcept
{
  using namespace clt::bit;
#ifdef COLT_x86_64
  static const auto FN =
      choose_simd_function<simd_flag::AVX512F, simd_flag::AVX2, simd_flag::DEFAULT>(
          &unitlen32AVX512F, &unitlen32AVX2, &unitlen32SSE2);
  return (*FN)(ptr);
#elif defined(COLT_ARM_7or8)
  static const auto FN = choose_simd_function<simd_flag::NEON, simd_flag::DEFAULT>(
      &unitlen32NEON, &unitlen32default);
  return (*FN)(ptr);
#else
  return unitlen32default(ptr);
#endif // COLT_x86_64
}