/*****************************************************************/ /**
 * @file   versions.h
 * @brief  Contains version of dependencies.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_COLT_VERSIONS
#define HG_COLT_VERSIONS

#include "colt/typedefs.h"
#include <fmt/format.h>
#include <simdutf.h>
#include <scn/scan.h>
#include <uni_algo/version.h>

namespace clt::vers
{
  /// @brief Parses the major version of '{}.{}.{}.{}'.
  /// This should only used to parse from string macros at compile-time.
  /// @param ptr The string from which to parse
  /// @return The parsed version
  consteval u16 parse_major(const char* ptr)
  {
    assert_true("Invalid format!", '0' <= *ptr && *ptr <= '9');
    u16 result = 0;
    while ('0' <= *ptr && *ptr <= '9')
      result = result * (u16)10 + (u16)(*ptr++ - '0');
    return result;
  }

  /// @brief Parses the minor version of '{}.{}.{}.{}'.
  /// This should only used to parse from string macros at compile-time.
  /// @param ptr The string from which to parse
  /// @return The parsed version
  consteval u16 parse_minor(const char* ptr)
  {
    assert_true("Invalid format!", '0' <= *ptr && *ptr <= '9');
    while (*ptr && *ptr != '.')
      ++ptr;
    assert_true("Invalid format!", *ptr == '.');
    ++ptr;
    assert_true("Invalid format!", '0' <= *ptr && *ptr <= '9');
    u16 result = 0;
    while ('0' <= *ptr && *ptr <= '9')
      result = result * (u16)10 + (u16)(*ptr++ - '0');
    return result;
  }

  /// @brief Parses the patch version of '{}.{}.{}.{}'.
  /// This should only used to parse from string macros at compile-time.
  /// @param ptr The string from which to parse
  /// @return The parsed version
  consteval u16 parse_patch(const char* ptr)
  {
    u16 result;
    size_t count = 2;
    while (count != 0)
    {
      result = 0;
      while (*ptr && *ptr != '.')
        ++ptr;
      assert_true("Invalid format!", *ptr == '.');
      ++ptr;
      assert_true("Invalid format!", '0' <= *ptr && *ptr <= '9');
      while ('0' <= *ptr && *ptr <= '9')
        result = result * (u16)10 + (u16)(*ptr++ - '0');
      --count;
    }
    return result;
  }
  
  /// @brief Parses the tweak version of '{}.{}.{}.{}'.
  /// This should only used to parse from string macros at compile-time.
  /// @param ptr The string from which to parse
  /// @return The parsed version
  consteval u16 parse_tweak(const char* ptr)
  {
    u16 result;
    size_t count = 3;
    while (count != 0)
    {
      result = 0;
      while (*ptr && *ptr != '.')
        ++ptr;
      assert_true("Invalid format!", *ptr == '.');
      ++ptr;
      assert_true("Invalid format!", '0' <= *ptr && *ptr <= '9');
      while ('0' <= *ptr && *ptr <= '9')
        result = result * (u16)10 + (u16)(*ptr++ - '0');
      --count;
    }
    return result;
  }

  /// @brief Represents a project version
  struct Version
  {
    /// @brief The Major version
    u16 major = 0;
    /// @brief The Minor version
    u16 minor = 0;
    /// @brief The patch version
    u16 patch = 0;
    /// @brief The tweak version
    u16 tweak = 0;
  };

  /// @brief ColtCpp version
  static constexpr Version ColtCppVersion = {
      COLT_VERSION_MAJOR, COLT_VERSION_MINOR, COLT_VERSION_PATCH,
      COLT_VERSION_TWEAK};
  /// @brief {fmt} version
  static constexpr Version FmtVersion = {
      FMT_VERSION / 10'000, (FMT_VERSION % 10'000) / 100, FMT_VERSION % 100};
  /// @brief simdutf version
  static constexpr Version SimdUtfVersion = {
      parse_major(SIMDUTF_VERSION), parse_minor(SIMDUTF_VERSION),
      parse_patch(SIMDUTF_VERSION)};
  /// @brief uni-algo version
  static constexpr Version UniAlgoVersion = {
      (UNI_ALGO_CPP_LIB_VERSION / 1000000), (UNI_ALGO_CPP_LIB_VERSION / 1000 % 1000),
      (UNI_ALGO_CPP_LIB_VERSION % 1000)};
  /// @brief Unicode version
  static constexpr Version UnicodeVersion = {
      (UNI_ALGO_UNICODE_VERSION / 1000000), (UNI_ALGO_UNICODE_VERSION / 1000 % 1000),
      (UNI_ALGO_UNICODE_VERSION % 1000)};
  static constexpr Version ScnVersion = {
      (SCN_VERSION / 10000000), (SCN_VERSION % 10000 / 10000),
      (SCN_VERSION % 10000)};

} // namespace clt::vers

template<>
struct fmt::formatter<clt::vers::Version>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    // TODO: add format option
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const clt::vers::Version& version, FormatContext& ctx) const
  {
    if (version.tweak == 0)
      return fmt::format_to(
          ctx.out(), "{}.{}.{}", version.major, version.minor, version.patch);
    return fmt::format_to(
        ctx.out(), "{}.{}.{}.{}", version.major, version.minor, version.patch,
        version.tweak);
  }
};

#endif // !HG_COLT_VERSIONS
