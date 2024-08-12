#include "unicode.h"

size_t clt::uni::unitlen16(const char16_t* ptr) noexcept
{
#ifdef COLT_x86_64
  const auto copy = ptr;
  // Align pointer to 32 byte boundary to use aligned load
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
#elif defined(COLT_ARM_NEON)
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
#endif // COLT_x86_64
}

size_t clt::uni::unitlen32(const char32_t* ptr) noexcept
{
#ifdef COLT_x86_64
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
#elif defined(COLT_ARM_NEON)
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
#endif // COLT_x86_64
}