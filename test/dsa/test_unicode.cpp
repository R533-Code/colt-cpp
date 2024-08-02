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