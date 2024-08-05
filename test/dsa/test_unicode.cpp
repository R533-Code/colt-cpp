#include <catch2/catch_all.hpp>
#include <colt/dsa/unicode.h>

TEST_CASE("Unicode Length", "[strlen]")
{
  using namespace clt;
  using enum StringEncoding;

  SECTION("UTF8")
  {
    REQUIRE(strlen(u8"\u000D") == 1);
    REQUIRE(strlen(u8"\u00b1") == 1);
    REQUIRE(strlen(u8"\u03b1") == 1);
    REQUIRE(strlen(u8"\u03BA\u1F79\u03C3\u03BC\u03B5") == 5);
    REQUIRE(strlen(u8"\u0288\u0119\u0835\u0E34\u021B") == 5);
  }
  SECTION("UTF16")
  {
    REQUIRE(strlen(u"\u000D") == 1);
    REQUIRE(strlen(u"\u00b1") == 1);
    REQUIRE(strlen(u"\u03b1") == 1);
    REQUIRE(strlen(u"\u03BA\u1F79\u03C3\u03BC\u03B5") == 5);
    REQUIRE(strlen(u"\u0288\u0119\u0835\u0E34\u021B") == 5);
  }
  SECTION("UTF32")
  {
    REQUIRE(strlen(U"\u000D") == 1);
    REQUIRE(strlen(U"\u00b1") == 1);
    REQUIRE(strlen(U"\u03b1") == 1);
    REQUIRE(strlen(U"\u03BA\u1F79\u03C3\u03BC\u03B5") == 5);
    REQUIRE(strlen(U"\u0288\u0119\u0835\u0E34\u021B") == 5);
  }
}

TEST_CASE("Unicode Size", "[bytelen]")
{
  using namespace clt;
  using enum StringEncoding;

#define TEST_BYTELEN(value) bytelen(value) == (sizeof value - sizeof(value[0]))

  SECTION("UTF8")
  {
    REQUIRE(TEST_BYTELEN(u8"\u000D"));
    REQUIRE(TEST_BYTELEN(u8"\u00b1"));
    REQUIRE(TEST_BYTELEN(u8"\u03b1"));
    REQUIRE(TEST_BYTELEN(u8"\u03BA\u1F79\u03C3\u03BC\u03B5"));
    REQUIRE(TEST_BYTELEN(u8"\u0288\u0119\u0835\u0E34\u021B"));
  }
  SECTION("UTF16")
  {
    REQUIRE(TEST_BYTELEN(u"\u000D"));
    REQUIRE(TEST_BYTELEN(u"\u00b1"));
    REQUIRE(TEST_BYTELEN(u"\u03b1"));
    REQUIRE(TEST_BYTELEN(u"\u03BA\u1F79\u03C3\u03BC\u03B5"));
    REQUIRE(TEST_BYTELEN(u"\u0288\u0119\u0835\u0E34\u021B"));
  }
  SECTION("UTF32")
  {
    REQUIRE(TEST_BYTELEN(U"\u000D"));
    REQUIRE(TEST_BYTELEN(U"\u00b1"));
    REQUIRE(TEST_BYTELEN(U"\u03b1"));
    REQUIRE(TEST_BYTELEN(U"\u03BA\u1F79\u03C3\u03BC\u03B5"));
    REQUIRE(TEST_BYTELEN(U"\u0288\u0119\u0835\u0E34\u021B"));
  }

#undef TEST_BYTELEN
}

TEST_CASE("Unicode UTF32 to UTF8", "[utf32_to_utf8]")
{
  using namespace clt;
  using namespace clt::uni;
  using enum StringEncoding;

  char32_t A[]          = U"\u0288\u0119\u0835\u0E34\u021B";
  const size_t sizeofA  = sizeof A / sizeof(char32_t);
  char8_t A8[]          = u8"\u0288\u0119\u0835\u0E34\u021B";
  const size_t sizeofA8 = sizeof A8 / sizeof(char8_t);
  char8_t buffer[256]   = {0};
  auto result           = &buffer[0];

  SECTION("UTF32 HOST VALID")
  {
    auto from = ptr_to<const Char32*>(&A[0]);
    REQUIRE(to_utf8(from, 6, result, 256) == ConvError::NO_ERROR);
    REQUIRE(
        std::memcmp(
            ptr_to<const void*>(A8), ptr_to<const void*>(buffer),
            result - &buffer[0])
        == 0);
  }

  SECTION("UTF32 HOST INVALID")
  {
    for (size_t i = 0; i < sizeofA; i++)
      A[i] = bit::byteswap((u32)A[i]);
    
    // The reversed bytes will be invalid unicode
    auto from = ptr_to<const Char32*>(&A[0]);
    REQUIRE(to_utf8(from, 6, result, 256) == ConvError::INVALID_INPUT);
  }

  SECTION("UTF32 HOST NOT ENOUGH MEMORY")
  {
    auto from = ptr_to<const Char32*>(&A[0]);
    REQUIRE(to_utf8(from, 6, result, 8) == ConvError::NOT_ENOUGH_SPACE);
  }

  SECTION("UTF32 BYTESWAP VALID")
  {
    for (size_t i = 0; i < sizeofA; i++)
      A[i] = bit::byteswap((u32)A[i]);

    auto from = ptr_to<const Char32Other*>(&A[0]);
    REQUIRE(to_utf8(from, 6, result, 256) == ConvError::NO_ERROR);
    REQUIRE(
        std::memcmp(
            ptr_to<const void*>(A8), ptr_to<const void*>(buffer),
            result - &buffer[0])
        == 0);
  }

  SECTION("UTF32 BYTESWAP INVALID")
  {
    // The reversed bytes will be invalid unicode
    auto from = ptr_to<const Char32Other*>(&A[0]);
    REQUIRE(to_utf8(from, 6, result, 256) == ConvError::INVALID_INPUT);
  }

  SECTION("UTF32 BYTESWAP NOT ENOUGH MEMORY")
  {
    for (size_t i = 0; i < sizeofA; i++)
      A[i] = bit::byteswap((u32)A[i]);

    auto from = ptr_to<const Char32Other*>(&A[0]);
    REQUIRE(to_utf8(from, 6, result, 8) == ConvError::NOT_ENOUGH_SPACE);
  }
}