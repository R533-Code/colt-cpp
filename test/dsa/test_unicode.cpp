#include <catch2/catch_all.hpp>
#include <colt/dsa/unicode.h>

TEST_CASE("Unicode Indexing", "[index_back index_front]")
{
  using namespace clt;
  using enum StringEncoding;

  // As sizeof includes NUL terminator, we need to decrement by one.

#define CREATE_STR(literal, var_name)                                          \
  using COLT_CONCAT(var_name, _t) =                                            \
      meta::cppchar_to_char_t<std::remove_cvref_t<decltype(literal[0])>>;      \
  auto var_name =                                                              \
      ptr_to<std::add_pointer_t<std::add_const_t<COLT_CONCAT(var_name, _t)>>>( \
          literal);                                                            \
  auto COLT_CONCAT(var_name, _front) = var_name;                               \
  auto COLT_CONCAT(var_name, _back) =                                          \
      var_name + (sizeof literal) / sizeof(COLT_CONCAT(var_name, _t)) - 2

  CREATE_STR(u8"\u000D\u1F79\u03C3\u03BC\u03B5", chr8);
  CREATE_STR(u"\u000D\u1F79\u03C3\u03BC\u03B5", chr16);

  SECTION("UTF8 front")
  {
    REQUIRE(uni::index_front(chr8_front, 0) == U'\u000D');
    REQUIRE(uni::index_front(chr8_front, 1) == U'\u1F79');
    REQUIRE(uni::index_front(chr8_front, 2) == U'\u03C3');
    REQUIRE(uni::index_front(chr8_front, 3) == U'\u03BC');
    REQUIRE(uni::index_front(chr8_front, 4) == U'\u03B5');
  }
  SECTION("UTF8 back")
  {
    REQUIRE(uni::index_back(chr8_back, 0) == U'\u03B5');
    REQUIRE(uni::index_back(chr8_back, 1) == U'\u03BC');
    REQUIRE(uni::index_back(chr8_back, 2) == U'\u03C3');
    REQUIRE(uni::index_back(chr8_back, 3) == U'\u1F79');
    REQUIRE(uni::index_back(chr8_back, 4) == U'\u000D');
  }
  SECTION("UTF16 front")
  {
    REQUIRE(uni::index_front(chr16_front, 0) == U'\u000D');
    REQUIRE(uni::index_front(chr16_front, 1) == U'\u1F79');
    REQUIRE(uni::index_front(chr16_front, 2) == U'\u03C3');
    REQUIRE(uni::index_front(chr16_front, 3) == U'\u03BC');
    REQUIRE(uni::index_front(chr16_front, 4) == U'\u03B5');
  }
  SECTION("UTF16 back")
  {
    REQUIRE(uni::index_back(chr16_back, 0) == U'\u03B5');
    REQUIRE(uni::index_back(chr16_back, 1) == U'\u03BC');
    REQUIRE(uni::index_back(chr16_back, 2) == U'\u03C3');
    REQUIRE(uni::index_back(chr16_back, 3) == U'\u1F79');
    REQUIRE(uni::index_back(chr16_back, 4) == U'\u000D');
  }

#undef CREATE_STR
}

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