/*****************************************************************/ /**
 * @file   unicode.cpp
 * @brief  Contains SIMD implementations of Unicode helpers.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "unicode.h"
#include "colt/num/math.h"
#include "colt/algo/detect_simd.h"

#pragma region // DEFAULT: len8 len16[BL]E

static clt::uni::LenInfo len8default(const char8_t* ptr) noexcept
{
  const auto copy = ptr;
  size_t len      = 0;
  auto end        = ptr;
  char8_t current;
  while ((current = *end) != u8'\0')
  {
    end += clt::uni::sequence_length(current);
    ++len;
  }
  return {len, static_cast<size_t>(ptr - copy)};
}

static clt::uni::LenInfo len16LEdefault(const char16_t* ptr) noexcept
{
  const auto copy = ptr;
  size_t len      = 0;
  auto end        = ptr;
  char16_t current;
  while ((current = (char16_t)(clt::ltoh((u16)*end)) != u'\0'))
  {
    end += clt::uni::sequence_length(current);
    ++len;
  }
  return {len, static_cast<size_t>(ptr - copy)};
}

static clt::uni::LenInfo len16BEdefault(const char16_t* ptr) noexcept
{
  const auto copy = ptr;
  size_t len      = 0;
  auto end        = ptr;
  char16_t current;
  while ((current = (char16_t)(clt::btoh((u16)*end)) != u'\0'))
  {
    end += clt::uni::sequence_length(current);
    ++len;
  }
  return {len, static_cast<size_t>(ptr - copy)};
}

#pragma endregion

#pragma region // DEFAULT: unitlen16  unitlen32

static size_t unitlen16default(const char16_t* ptr) noexcept
{
  const auto copy = ptr;
  while (*ptr != 0)
    ptr++;
  return copy - ptr;
}

static size_t unitlen32default(const char32_t* ptr) noexcept
{
  const auto copy = ptr;
  while (*ptr != 0)
    ptr++;
  return copy - ptr;
}

#pragma endregion

#if defined(COLT_x86_64)

  #pragma region // len8 SSE2, AVX2, AXV512BW
static COLT_FORCE_SSE2 clt::uni::LenInfo len8SSE2(const char8_t* ptr) noexcept
{
  const auto copy = ptr;
  size_t len      = 0;
  // Align pointer to 16 byte boundary to use aligned load
  // and avoid page faults.
  // We can't use sequence_length here as the goal is to align
  // and most likely adding sequence_length will not align the pointer.
  while (uintptr_t(ptr) % 16 != 0)
  {
    if (*ptr == '\0')
      return {len, static_cast<size_t>(ptr - copy)};
    len += (size_t)(!clt::uni::is_trail(*ptr));
    ++ptr;
  }

  const __m128i zero        = _mm_setzero_si128();
  const __m128i trail_mask  = _mm_set1_epi8((u8)0b1100'0000);
  const __m128i trail_value = _mm_set1_epi8((u8)0b1000'0000);
  constexpr auto PACK_COUNT = sizeof(__m128i) / sizeof(u8);
  unsigned int mask;
  while (true)
  {
    __m128i values = _mm_load_si128(reinterpret_cast<const __m128i*>(ptr));
    __m128i cmp    = _mm_cmpeq_epi8(values, zero);
    mask           = _mm_movemask_epi8(cmp);
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
      return {len, static_cast<size_t>(ptr - copy)};
    len += (size_t)(!clt::uni::is_trail(*ptr));
    ++ptr;
  }
  clt::unreachable("programming error");
}

static COLT_FORCE_AVX2 clt::uni::LenInfo len8AVX2(const char8_t* ptr) noexcept
{
  const auto copy = ptr;
  size_t len      = 0;
  while (uintptr_t(ptr) % 32 != 0)
  {
    if (*ptr == '\0')
      return {len, static_cast<size_t>(ptr - copy)};
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
  // In any cases, there are at most 32 byte to check.
  while (true)
  {
    if (*ptr == '\0')
      return {len, static_cast<size_t>(ptr - copy)};
    len += (size_t)(!clt::uni::is_trail(*ptr));
    ++ptr;
  }
  clt::unreachable("programming error");
}

static COLT_FORCE_AVX512BW clt::uni::LenInfo len8AVX512BW(
    const char8_t* ptr) noexcept
{
  const auto copy = ptr;
  size_t len      = 0;
  while (uintptr_t(ptr) % 64 != 0)
  {
    if (*ptr == '\0')
      return {len, static_cast<size_t>(ptr - copy)};
    len += (size_t)(!clt::uni::is_trail(*ptr));
    ++ptr;
  }

  const __m512i zero        = _mm512_setzero_si512();
  const __m512i trail_mask  = _mm512_set1_epi8((u8)0b1100'0000);
  const __m512i trail_value = _mm512_set1_epi8((u8)0b1000'0000);
  constexpr auto PACK_COUNT = sizeof(__m512i) / sizeof(u8);
  unsigned long long mask;
  while (true)
  {
    __m512i values = _mm512_load_si512(reinterpret_cast<const __m512i*>(ptr));
    mask           = _mm512_cmpeq_epi8_mask(values, zero);
    // If we found NUL-terminator break.
    if (mask != 0)
      break;
    __m512i is_trail = _mm512_and_si512(values, trail_mask);
    len += PACK_COUNT - std::popcount(_mm512_cmpeq_epi8_mask(is_trail, trail_value));
    ptr += PACK_COUNT;
  }
  // We might have ended on trailing byte
  // so we can't really use sequence_length here.
  // In any cases, there are at most 64 byte to check.
  while (true)
  {
    if (*ptr == '\0')
      return {len, static_cast<size_t>(ptr - copy)};
    len += (size_t)(!clt::uni::is_trail(*ptr));
    ++ptr;
  }
  clt::unreachable("programming error");
}

  #pragma endregion

  #pragma region // len16 SSE2, AVX2, AVX512BW

template<bool SWAP>
static COLT_FORCE_SSE2 clt::uni::LenInfo len16SSE2(const char16_t* ptr) noexcept
{
  const auto copy = ptr;
  size_t len      = 0;
  // Align pointer to 16 byte boundary to use aligned load
  // and avoid page faults.
  // We can't use sequence_length here as the goal is to align
  // and most likely adding sequence_length will not align the pointer.
  while (uintptr_t(ptr) % 16 != 0)
  {
    if (*ptr == '\0')
      return {len, static_cast<size_t>(ptr - copy)};
    if constexpr (SWAP)
      len += (size_t)(!clt::uni::is_trail_surrogate(clt::byteswap(*ptr)));
    else
      len += (size_t)(!clt::uni::is_trail_surrogate(*ptr));
    ++ptr;
  }

  const __m128i zero        = _mm_setzero_si128();
  const __m128i trail_mask  = _mm_set1_epi16(SWAP ? (u16)0x00FC : (u16)0xFC00);
  const __m128i trail_value = _mm_set1_epi16(SWAP ? (u16)0x00DC : (u16)0xDC00);
  constexpr auto PACK_COUNT = sizeof(__m128i) / sizeof(u16);
  unsigned int mask;
  while (true)
  {
    __m128i values = _mm_load_si128(reinterpret_cast<const __m128i*>(ptr));
    __m128i cmp    = _mm_cmpeq_epi16(values, zero);
    mask           = _mm_movemask_epi8(cmp);
    // If we found NUL-terminator break.
    if (mask != 0)
      break;
    __m128i is_trail = _mm_and_si128(values, trail_mask);
    is_trail         = _mm_cmpeq_epi16(is_trail, trail_value);
    len += PACK_COUNT - std::popcount((unsigned int)_mm_movemask_epi8(is_trail)) / 2;
    ptr += PACK_COUNT;
  }
  // We might have ended on trailing byte
  // so we can't really use sequence_length here.
  // In any cases, there are at most 16 byte to check.
  while (true)
  {
    if (*ptr == 0)
      return {len, static_cast<size_t>(ptr - copy)};
    if constexpr (SWAP)
      len += (size_t)(!clt::uni::is_trail_surrogate(clt::byteswap(*ptr)));
    else
      len += (size_t)(!clt::uni::is_trail_surrogate(*ptr));
    ++ptr;
  }
  clt::unreachable("programming error");
}

template<bool SWAP>
static COLT_FORCE_AVX2 clt::uni::LenInfo len16AVX2(const char16_t* ptr) noexcept
{
  const auto copy = ptr;
  size_t len      = 0;
  // We can't use sequence_length here as the goal is to align
  // and most likely adding sequence_length will not align the pointer.
  while (uintptr_t(ptr) % 32 != 0)
  {
    if (*ptr == 0)
      return {len, static_cast<size_t>(ptr - copy)};
    if constexpr (SWAP)
      len += (size_t)(!clt::uni::is_trail_surrogate(clt::byteswap(*ptr)));
    else
      len += (size_t)(!clt::uni::is_trail_surrogate(*ptr));
    ++ptr;
  }

  const __m256i zero        = _mm256_setzero_si256();
  const __m256i trail_mask  = _mm256_set1_epi16(SWAP ? (u16)0x00FC : (u16)0xFC00);
  const __m256i trail_value = _mm256_set1_epi16(SWAP ? (u16)0x00DC : (u16)0xDC00);
  constexpr auto PACK_COUNT = sizeof(__m256i) / sizeof(u16);
  unsigned int mask;
  while (true)
  {
    __m256i values = _mm256_load_si256(reinterpret_cast<const __m256i*>(ptr));
    __m256i cmp    = _mm256_cmpeq_epi16(values, zero);
    mask           = _mm256_movemask_epi8(cmp);
    // If we found NUL-terminator break.
    if (mask != 0)
      break;
    __m256i is_trail = _mm256_and_si256(values, trail_mask);
    is_trail         = _mm256_cmpeq_epi16(is_trail, trail_value);
    len +=
        PACK_COUNT - std::popcount((unsigned int)_mm256_movemask_epi8(is_trail)) / 2;
    ptr += PACK_COUNT;
  }
  // We might have ended on trailing byte
  // so we can't really use sequence_length here.
  // In any cases, there are at most 32 byte to check.
  while (true)
  {
    if (*ptr == 0)
      return {len, static_cast<size_t>(ptr - copy)};
    if constexpr (SWAP)
      len += (size_t)(!clt::uni::is_trail_surrogate(clt::byteswap(*ptr)));
    else
      len += (size_t)(!clt::uni::is_trail_surrogate(*ptr));
    ++ptr;
  }
  clt::unreachable("programming error");
}

template<bool SWAP>
static COLT_FORCE_AVX512BW clt::uni::LenInfo len16AVX512BW(
    const char16_t* ptr) noexcept
{
  const auto copy = ptr;
  size_t len      = 0;
  // We can't use sequence_length here as the goal is to align
  // and most likely adding sequence_length will not align the pointer.
  while (uintptr_t(ptr) % 64 != 0)
  {
    if (*ptr == 0)
      return {len, static_cast<size_t>(ptr - copy)};
    if constexpr (SWAP)
      len += (size_t)(!clt::uni::is_trail_surrogate(clt::byteswap(*ptr)));
    else
      len += (size_t)(!clt::uni::is_trail_surrogate(*ptr));
    ++ptr;
  }

  const __m512i zero        = _mm512_setzero_si512();
  const __m512i trail_mask  = _mm512_set1_epi16(SWAP ? (u16)0x00FC : (u16)0xFC00);
  const __m512i trail_value = _mm512_set1_epi16(SWAP ? (u16)0x00DC : (u16)0xDC00);
  constexpr auto PACK_COUNT = sizeof(__m512i) / sizeof(u16);
  unsigned int mask;
  while (true)
  {
    __m512i values = _mm512_load_si512(reinterpret_cast<const __m512i*>(ptr));
    mask           = _mm512_cmpeq_epi16_mask(values, zero);
    // If we found NUL-terminator break.
    if (mask != 0)
      break;
    __m512i is_trail = _mm512_and_si512(values, trail_mask);
    len +=
        PACK_COUNT - std::popcount(_mm512_cmpeq_epi16_mask(is_trail, trail_value));
    ptr += PACK_COUNT;
  }
  // We might have ended on trailing byte
  // so we can't really use sequence_length here.
  // In any cases, there are at most 32 byte to check.
  while (true)
  {
    if (*ptr == 0)
      return {len, static_cast<size_t>(ptr - copy)};
    if constexpr (SWAP)
      len += (size_t)(!clt::uni::is_trail_surrogate(clt::byteswap(*ptr)));
    else
      len += (size_t)(!clt::uni::is_trail_surrogate(*ptr));
    ++ptr;
  }
  clt::unreachable("programming error");
}

  #pragma endregion

  #pragma region // unitlen16 SSE2, AVX2, AVX512F

static COLT_FORCE_SSE2 size_t unitlen16SSE2(const char16_t* ptr) noexcept
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
static COLT_FORCE_AVX2 size_t unitlen16AVX2(const char16_t* ptr) noexcept
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
static COLT_FORCE_AVX512BW size_t unitlen16AVX512BW(const char16_t* ptr) noexcept
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

  #pragma endregion

  #pragma region // unitlen32 SSE2, AVX2, AVX512F
static COLT_FORCE_SSE2 size_t unitlen32SSE2(const char32_t* ptr) noexcept
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
static COLT_FORCE_AVX2 size_t unitlen32AVX2(const char32_t* ptr) noexcept
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
static COLT_FORCE_AVX512F size_t unitlen32AVX512F(const char32_t* ptr) noexcept
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
  #pragma endregion

#elif defined(COLT_ARM_7or8)

// See link below for vshrn
// https://community.arm.com/arm-community-blogs/b/infrastructure-solutions-blog/posts/porting-x86-vector-bitmask-optimizations-to-arm-neon

  #pragma region // unitlen16 NEON
static COLT_FORCE_NEON size_t unitlen16NEON(const char16_t* ptr) noexcept
{
  const auto copy = ptr;
  while (uintptr_t(ptr) % alignof(uint16x8_t) != 0)
  {
    if (*ptr == 0)
      return ptr - copy;
    ++ptr;
  }

  // Zero mask
  const uint16x8_t zero     = vdupq_n_u16(0);
  constexpr auto PACK_COUNT = sizeof(uint16x8_t) / sizeof(u16);
  u64 mask;
  while (true)
  {
    uint16x8_t values   = vld1q_u16(reinterpret_cast<const u16*>(ptr));
    uint16x8_t cmp      = vceqq_u16(values, zero);
    const uint8x8_t res = vshrn_n_u16(cmp, 8);
    mask                = vget_lane_u64(vreinterpret_u64_u8(res), 0);
    if (mask != 0)
      break;
    ptr += PACK_COUNT;
  }
  return (ptr - copy) + std::countr_zero(mask) / 8;
}
  #pragma endregion

  #pragma region // unitlen32 NEON
static COLT_FORCE_NEON size_t unitlen32NEON(const char32_t* ptr) noexcept
{
  const auto copy = ptr;
  while (uintptr_t(ptr) % alignof(uint32x4_t) != 0)
  {
    if (*ptr == 0)
      return ptr - copy;
    ++ptr;
  }

  // Zero mask
  const uint32x4_t zero     = vdupq_n_u32(0);
  constexpr auto PACK_COUNT = sizeof(uint32x4_t) / sizeof(u32);
  u64 mask;
  while (true)
  {
    uint32x4_t values    = vld1q_u32(reinterpret_cast<const u32*>(ptr));
    uint32x4_t cmp       = vceqq_u32(values, zero);
    const uint16x4_t res = vshrn_n_u32(cmp, 8);
    mask                 = vget_lane_u64(vreinterpret_u64_u16(res), 0);
    if (mask != 0)
      break;
    ptr += PACK_COUNT;
  }
  return (ptr - copy) + std::countr_zero(mask) / 16;
}
  #pragma endregion

  #pragma region // len8 NEON
static COLT_FORCE_NEON clt::uni::LenInfo len8NEON(const char8_t* ptr) noexcept
{
  const auto copy = ptr;
  size_t len      = 0;
  // We can't use sequence_length here as the goal is to align
  // and most likely adding sequence_length will not align the pointer.
  while (uintptr_t(ptr) % alignof(uint8x16_t) != 0)
  {
    if (*ptr == '\0')
      return {len, static_cast<size_t>(ptr - copy)};
    len += (size_t)(!clt::uni::is_trail(*ptr));
    ++ptr;
  }

  // Zero mask
  const uint8x16_t zero        = vdupq_n_u8(0);
  const uint8x16_t trail_mask  = vdupq_n_u8((u8)0b1100'0000);
  const uint8x16_t trail_value = vdupq_n_u8((u8)0b1000'0000);
  constexpr auto PACK_COUNT    = sizeof(uint8x16_t) / sizeof(u8);
  u64 mask;
  while (true)
  {
    uint8x16_t values   = vld1q_u8(reinterpret_cast<const u8*>(ptr));
    uint8x16_t cmp      = vceqq_u8(values, zero);
    const uint8x8_t res = vshrn_n_u16(vreinterpretq_u16_u8(cmp), 4);
    mask                = vget_lane_u64(vreinterpret_u64_u8(res), 0);
    if (mask != 0)
      break;
    uint8x16_t is_trail = vandq_u8(values, trail_mask);
    is_trail            = vceqq_u8(is_trail, trail_value);
    len += PACK_COUNT
           - std::popcount(vget_lane_u64(
                 vreinterpret_u64_u8(vshrn_n_u16(vreinterpretq_u16_u8(is_trail), 4)),
                 0))
                 / 4;
    ptr += PACK_COUNT;
  }
  while (true)
  {
    if (*ptr == '\0')
      return {len, static_cast<size_t>(ptr - copy)};
    len += (size_t)(!clt::uni::is_trail(*ptr));
    ++ptr;
  }
  clt::unreachable("programming error");
}
  #pragma endregion

  #pragma region // len16 NEON
template<bool SWAP>
static COLT_FORCE_NEON clt::uni::LenInfo len16NEON(const char16_t* ptr) noexcept
{
  const auto copy = ptr;
  size_t len      = 0;
  // We can't use sequence_length here as the goal is to align
  // and most likely adding sequence_length will not align the pointer.
  while (uintptr_t(ptr) % alignof(uint16x8_t) != 0)
  {
    if (*ptr == '\0')
      return {len, static_cast<size_t>(ptr - copy)};
    if constexpr (SWAP)
      len += (size_t)(!clt::uni::is_trail_surrogate(clt::byteswap(*ptr)));
    else
      len += (size_t)(!clt::uni::is_trail_surrogate(*ptr));
    ++ptr;
  }

  // Zero mask
  const uint16x8_t zero        = vdupq_n_u16(0);
  const uint16x8_t trail_mask  = vdupq_n_u16(SWAP ? (u16)0x00FC : (u16)0xFC00);
  const uint16x8_t trail_value = vdupq_n_u16(SWAP ? (u16)0x00DC : (u16)0xDC00);
  constexpr auto PACK_COUNT    = sizeof(uint16x8_t) / sizeof(u16);
  u64 mask;
  while (true)
  {
    uint16x8_t values   = vld1q_u16(reinterpret_cast<const u16*>(ptr));
    uint16x8_t cmp      = vceqq_u16(values, zero);
    const uint8x8_t res = vshrn_n_u16(cmp, 8);
    mask                = vget_lane_u64(vreinterpret_u64_u8(res), 0);
    if (mask != 0)
      break;
    uint16x8_t is_trail = vandq_u16(values, trail_mask);
    is_trail            = vceqq_u16(is_trail, trail_value);
    len += PACK_COUNT
           - std::popcount(
                 vget_lane_u64(vreinterpret_u64_u8(vshrn_n_u16(is_trail, 4)), 0))
                 / 8;
    ptr += PACK_COUNT;
  }
  while (true)
  {
    if (*ptr == '\0')
      return {len, static_cast<size_t>(ptr - copy)};
    if constexpr (SWAP)
      len += (size_t)(!clt::uni::is_trail_surrogate(clt::byteswap(*ptr)));
    else
      len += (size_t)(!clt::uni::is_trail_surrogate(*ptr));
    ++ptr;
  }
  clt::unreachable("programming error");
}
  #pragma endregion

#endif // COLT_x86_64

/// @brief Function pointer for len8
using len8_fn_t = clt::uni::LenInfo (*)(const char8_t*) noexcept;
/// @brief Function pointer for len16
using len16_fn_t = clt::uni::LenInfo (*)(const char16_t*) noexcept;
/// @brief Function pointer for unitlen16
using unitlen16_fn_t = size_t (*)(const char16_t*) noexcept;
/// @brief Function pointer for unitlen32
using unitlen32_fn_t = size_t (*)(const char32_t*) noexcept;

/// @brief Type containing pointer to SIMD versions
struct SIMDImpl
{
  /// @brief len8 function pointer
  len8_fn_t len8;
  /// @brief len16le function pointer
  len16_fn_t len16le;
  /// @brief len16be function pointer
  len16_fn_t len16be;
  /// @brief unitlen16 function pointer
  unitlen16_fn_t unit16;
  /// @brief unitlen32 function pointer
  unitlen32_fn_t unit32;
};

/// @brief Returns the SIMD implementation function pointers.
/// This is where the different SIMD implementation on each architecture
/// must be choosen with their specific extensions.
/// @return Reference to the SIMD implementation function pointers
static const SIMDImpl& get_colt_unicode_simd() noexcept
{
  using namespace clt;

  static constexpr bool SWAP = (TargetEndian::native == TargetEndian::big);
#ifdef COLT_x86_64
  static auto ret = choose_simd_implementation<
      simd_flag::AVX512BW, simd_flag::AVX2, simd_flag::DEFAULT>{}(
      SIMDImpl{
          &len8AVX512BW, &len16AVX512BW<SWAP>, &len16AVX512BW<!SWAP>,
          &unitlen16AVX512BW, &unitlen32AVX512F},
      SIMDImpl{
          &len8AVX2, &len16AVX2<SWAP>, &len16AVX2<!SWAP>, &unitlen16AVX2,
          &unitlen32AVX2},
      SIMDImpl{
          &len8SSE2, &len16SSE2<SWAP>, &len16SSE2<!SWAP>, &unitlen16SSE2,
          &unitlen32SSE2});
  return ret;
#elif defined(COLT_ARM_7or8)
  static auto ret =
      choose_simd_implementation<simd_flag::NEON, simd_flag::DEFAULT>{}(
          SIMDImpl{
              &len8NEON, &len16NEON<SWAP>, &len16NEON<!SWAP>, &unitlen16NEON,
              &unitlen32NEON},
          SIMDImpl{
              &len8default, &len16LEdefault, &len16BEdefault, &unitlen16default,
              &unitlen32default});
  return ret;
#else
  static auto ret = SIMDImpl{
      &len8default, &len16LEdefault, &len16BEdefault, &unitlen16default,
      &unitlen32default};
  return ret;
#endif // COLT_x86_64
}

clt::uni::LenInfo clt::uni::details::len8(const char8_t* ptr) noexcept
{
  return get_colt_unicode_simd().len8(ptr);
}

clt::uni::LenInfo clt::uni::details::len16LE(const char16_t* ptr) noexcept
{
  return get_colt_unicode_simd().len16le(ptr);
}

clt::uni::LenInfo clt::uni::details::len16BE(const char16_t* ptr) noexcept
{
  return get_colt_unicode_simd().len16be(ptr);
}

clt::uni::LenInfo clt::uni::details::len16(const char16_t* ptr) noexcept
{
  if constexpr (StringEncoding::UTF16 == StringEncoding::UTF16LE)
    return len16LE(ptr);
  else
    return len16BE(ptr);
}

size_t clt::uni::details::unitlen16(const char16_t* ptr) noexcept
{
  return get_colt_unicode_simd().unit16(ptr);
}

size_t clt::uni::details::unitlen32(const char32_t* ptr) noexcept
{
  return get_colt_unicode_simd().unit32(ptr);
}