/*****************************************************************/ /**
 * @file   test_unicode.cpp
 * @brief  Unit tests for Unicode related functions.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "../includes.h"
#include <colt/unicode/unicode.h>

// Using COLT_FOR_EACH, we can generate a test for each of the strings below.
// COLT_CONCAT(x, ...) is used to concatenate the string literal (u8, u, U).
// Each test string must be followed by ', \'.
// Poor compilers...
// clang-format off
#define TEST_STRING(x) \
  COLT_CONCAT(x, "\u000D"), \
  COLT_CONCAT(x, "\u00b1"), \
  COLT_CONCAT(x, "\u03b1"), \
  COLT_CONCAT(x, "\u03BA\u1F79\u03C3\u03BC\u03B5"), \
  COLT_CONCAT(x, "\u0288\u0119\u0835\u0E34\u021B"), \
  COLT_CONCAT(x, ""), \
  COLT_CONCAT(x, "0"), \
  COLT_CONCAT(x, "01"), \
  COLT_CONCAT(x, "012"), \
  COLT_CONCAT(x, "0123"), \
  COLT_CONCAT(x, "01234"), \
  COLT_CONCAT(x, "012345"), \
  COLT_CONCAT(x, "0123456"), \
  COLT_CONCAT(x, "01234567"), \
  COLT_CONCAT(x, "012345678"), \
  COLT_CONCAT(x, "0123456789"), \
  COLT_CONCAT(x, "1234567890123456789012345678901234567890"), \
  COLT_CONCAT(x, "12345678901234567890123456789012345678900"), \
  COLT_CONCAT(x, "123456789012345678901234567890123456789001"), \
  COLT_CONCAT(x, "1234567890123456789012345678901234567890012"), \
  COLT_CONCAT(x, "12345678901234567890123456789012345678900123"), \
  COLT_CONCAT(x, "123456789012345678901234567890123456789001234"), \
  COLT_CONCAT(x, "1234567890123456789012345678901234567890012345"), \
  COLT_CONCAT(x, "12345678901234567890123456789012345678900123456"), \
  COLT_CONCAT(x, "123456789012345678901234567890123456789001234567"), \
  COLT_CONCAT(x, "1234567890123456789012345678901234567890012345678"), \
  COLT_CONCAT(x, "12345678901234567890123456789012345678900123456789"), \
  COLT_CONCAT(x, "12345678901234567890123456789012345678901234567890123456789012345678901234567890"), \
  COLT_CONCAT(x, "1234567890123456789012345678901234567890123456789012345678901234567890123456789001234567890012345678900123456789001234567890012345678900123456789001234567890"), \
  COLT_CONCAT(x, "\u65E0\u53EF\u5426\u8BA4\uFF0C\u5F53\u8BFB\u8005\u5728\u6D4F\u89C8\u4E00\u4E2A\u9875\u9762\u7684\u6392\u7248\u65F6\uFF0C\u96BE\u514D\u4F1A\u88AB\u53EF\u9605\u8BFB\u7684\u5185\u5BB9\u6240\u5206\u6563\u6CE8\u610F\u529B\u3002Lorem Ipsum\u7684\u76EE\u7684\u5C31\u662F\u4E3A\u4E86\u4FDD\u6301\u5B57\u6BCD\u591A\u591A\u5C11\u5C11\u6807\u51C6\u53CA\u5E73\u5747\u7684\u5206\u914D\uFF0C\u800C\u4E0D\u662F\u201C\u6B64\u5904\u6709\u6587\u672C\uFF0C\u6B64\u5904\u6709\u6587\u672C\u201D\uFF0C\u4ECE\u800C\u8BA9\u5185\u5BB9\u66F4\u50CF\u53EF\u8BFB\u7684\u82F1\u8BED\u3002\u5982\u4ECA\uFF0C\u5F88\u591A\u684C\u9762\u6392\u7248\u8F6F\u4EF6\u4EE5\u53CA\u7F51\u9875\u7F16\u8F91\u7528Lorem Ipsum\u4F5C\u4E3A\u9ED8\u8BA4\u7684\u793A\u8303\u6587\u672C\uFF0C\u641C\u4E00\u641C\u201CLorem Ipsum\u201D\u5C31\u80FD\u627E\u5230\u8FD9\u4E9B\u7F51\u7AD9\u7684\u96CF\u5F62\u3002\u8FD9\u4E9B\u5E74\u6765Lorem Ipsum\u6F14\u53D8\u51FA\u4E86\u5404\u5F0F\u5404\u6837\u7684\u7248\u672C\uFF0C\u6709\u4E9B\u51FA\u4E8E\u5076\u7136\uFF0C\u6709\u4E9B\u5219\u662F\u6545\u610F\u7684\uFF08\u523B\u610F\u7684\u5E7D\u9ED8\u4E4B\u7C7B\u7684\uFF09\u3002"), \
  COLT_CONCAT(x, "\u8033 \u51FA \u53BB \uFF0C\u53EF \u300D \u89BD. \u300D \u9952\u723E\u53BB\u7F77\u300D \u8033 \uFF0C\u6108\u807D\u6108\u60F1 \u4E5F\u61CA\u6094\u4E0D\u4E86 \u77E3 \u53BB \u89BD \u66F0\uFF1A \u4E8B \u6B64\u662F\u5F8C\u8A71. \u8033 \u53BB \u610F \u77E3 \u95DC\u96CE \u89BD \u4E8B. \u4E8B \u7B2C\u4E09\u56DE \u7B2C\u4E94\u56DE \u7B2C\u4E00\u56DE \u77E3 \uFF0C\u53EF \u9A5A\u7570 \u8033 \u76F8\u57DF \u8AA8. \u610F \uFF0C\u53EF \u77E3 \u66F0\uFF1A \u53BB \u89BD \u8033. \u77E3 \u4E8B \u53BB \u8033 \u610F \uFF0C\u6108\u807D\u6108\u60F1 \u300D \u9952\u723E\u53BB\u7F77\u300D \u4E5F\u61CA\u6094\u4E0D\u4E86 \u6B64\u662F\u5F8C\u8A71. \uFEFF\u767D\u572D\u5FD7 \u8033 \u300D \u51FA \u5DF1\u8F49\u8EAB \u4EE5\u6E2C\u6A5F \u77E3. \u77E3 \u4E8B \u8AA8 \u53BB \u95DC\u96CE \u8033. \u300D \u53BB \u4E8B \u89BD \u66F0\uFF1A \uFF0C\u53EF. \u66F0\uFF1A \u610F \u300D \u77E3 \u95DC\u96CE \u53BB \u4E8B. \u66F0\uFF1A \uFF0C\u53EF \u300D \u8AA8 \u95DC\u96CE \u8033 \u4E8B \u77E3. \u6C57\u6D41\u5982\u96E8 \u5192\u8A8D\u6536\u4E86 \u7236\u89AA\u56DE\u8859 \u5409\u5B89\u800C\u4F86. \u8AA8 \u610F \u300D \u4E8B \u51FA \u77E3 \u66F0\uFF1A \u89BD. \u4E0D\u984C \u7B2C\u4E00\u56DE \u7B2C\u4E5D\u56DE \u9A5A\u7570 \u7B2C\u4E94\u56DE \u7B2C\u516D\u56DE. \u5206\u5F97\u610F \u7B2C\u5341\u4E00\u56DE \u5F8C\u7ACA\u807D \u53BB \u77E3 \u300D \u8033 \u95DC\u96CE \u5EFA\u7AE0\u66F0\uFF1A."), \
  COLT_CONCAT(x, "End of tests!")
// clang-format on

struct MAKE_BE{};

template<size_t N>
const clt::Char16Other* operator+(MAKE_BE, const char16_t (&str)[N])
{
  auto ret = new char16_t[N];
  for (size_t i = 0; i < N; i++)
    ret[i] = clt::byteswap(str[i]);
  return reinterpret_cast<clt::Char16Other*>(ret);
}

#define TEST_STRING8  TEST_STRING(u8)
#define TEST_STRING16 TEST_STRING(u)
#define TEST_STRING32 TEST_STRING(U)

TEST_CASE("Unicode Length")
{
  using namespace clt;
  using namespace clt::uni;
  using enum StringEncoding;

#define TEST_STRLEN8(value)   \
  REQUIRE(                    \
      strlen(value)           \
      == simdutf::count_utf8( \
          (const char*)value, ((sizeof value) / sizeof(value[0]) - 1)));

#define TEST_STRLEN16(value) \
  REQUIRE(                   \
      strlen(value)          \
      == simdutf::count_utf16(value, ((sizeof value) / sizeof(value[0]) - 1)));
#define TEST_STRLEN16OTHER(value) \
  REQUIRE(                   \
      strlen(MAKE_BE{} + value) \
      == simdutf::count_utf16(value, ((sizeof value) / sizeof(value[0]) - 1)));

#define TEST_STRLEN32(value) \
  REQUIRE(strlen(value) == (sizeof value) / sizeof(value[0]) - 1);

  SECTION("UTF8")
  {
    COLT_FOR_EACH(TEST_STRLEN8, TEST_STRING8);
  }
  SECTION("UTF16")
  {
    COLT_FOR_EACH(TEST_STRLEN16, TEST_STRING16);
  }
  SECTION("UTF16 Other")
  {
    COLT_FOR_EACH(TEST_STRLEN16OTHER, TEST_STRING16);
  }
  SECTION("UTF32")
  {
    COLT_FOR_EACH(TEST_STRLEN32, TEST_STRING32);
  }

#undef TEST_STRLEN8
#undef TEST_STRLEN16
#undef TEST_STRLEN32
}

TEST_CASE("Unicode Count and Middle")
{
  using namespace clt;
  using namespace clt::uni;

#define TEST_MIDDLE(str, middle) \
  REQUIRE(                       \
      count_and_middle(str, ((sizeof str) / sizeof(str[0]) - 1)).second == middle)
#define TEST_COUNT(value)                                                    \
  REQUIRE(                                                                   \
      count_and_middle(value, ((sizeof value) / sizeof(value[0]) - 1)).first \
      == simdutf::count_utf8(                                                \
          (const char*)value, ((sizeof value) / sizeof(value[0]) - 1)));

  SECTION("UTF8")
  {
    COLT_FOR_EACH(TEST_COUNT, TEST_STRING8);
    TEST_MIDDLE(u8"", 0);
    TEST_MIDDLE(u8"1", 0);
    TEST_MIDDLE(u8"10", 0);
    TEST_MIDDLE(u8"100", 1);
    TEST_MIDDLE(u8"1000", 1);
    TEST_MIDDLE(u8"10000", 2);
    TEST_MIDDLE(u8"100000", 2);
    TEST_MIDDLE(u8"1000000", 3);
    TEST_MIDDLE(u8"10000000", 3);
    TEST_MIDDLE(u8"100000000", 4);
    TEST_MIDDLE(u8"1000000000", 4);
    TEST_MIDDLE(u8"\u0100ab", 2);
    TEST_MIDDLE(u8"\u1100ab", 3);
    TEST_MIDDLE(u8"\u1100\u1100\u1100ab", 6);
    TEST_MIDDLE(u8"\u1100\u1100\u1100abc", 6);
    TEST_MIDDLE(u8"ab\u1100\u1100\u1100", 5);
    TEST_MIDDLE(u8"abc\u1100\u1100\u1100", 2);
    TEST_MIDDLE(u8"\u1100\u1100\u1100abcd", 9);
    TEST_MIDDLE(u8"\u1100\u1100\u1100abcdf", 9);
  }

#undef TEST_MIDDLE
#undef TEST_COUNT
}

TEST_CASE("Unicode Size")
{
  using namespace clt;
  using namespace clt::uni;
  using enum StringEncoding;

#define TEST_UNITLEN(value) \
  REQUIRE(unitlen(value) == ((sizeof value) / sizeof(value[0]) - 1));
#define TEST_UNITLEN16OTHER(value) \
  REQUIRE(unitlen(MAKE_BE{} + value) == ((sizeof value) / sizeof(value[0]) - 1));

  SECTION("UTF8")
  {
    COLT_FOR_EACH(TEST_UNITLEN, TEST_STRING8);
  }
  SECTION("UTF16")
  {
    COLT_FOR_EACH(TEST_UNITLEN, TEST_STRING16);
  }
  SECTION("UTF16 Other")
  {
    COLT_FOR_EACH(TEST_UNITLEN16OTHER, TEST_STRING16);
  }
  SECTION("UTF32")
  {
    COLT_FOR_EACH(TEST_UNITLEN, TEST_STRING32);
  }
#undef TEST_UNITLEN
}

TEST_CASE("Unicode SIMD unitlen")
{
  using namespace clt;
  using namespace clt::uni;
#define TEST_UNITLEN16(value) \
  REQUIRE(uni::details::unitlen16(value) == ((sizeof value) / sizeof(value[0]) - 1));
#define TEST_UNITLEN32(value) \
  REQUIRE(uni::details::unitlen32(value) == ((sizeof value) / sizeof(value[0]) - 1));

  SECTION("UTF16")
  {
    COLT_FOR_EACH(TEST_UNITLEN16, TEST_STRING16);
  }
  SECTION("UTF32")
  {
    COLT_FOR_EACH(TEST_UNITLEN32, TEST_STRING32);
  }

#undef TEST_UNITLEN16
#undef TEST_UNITLEN32
}

TEST_CASE("Unicode SIMD strlen")
{
  using namespace clt;
  using namespace clt::uni;
#define TEST_STRLEN8(value)        \
  REQUIRE(                         \
      uni::details::len8(value).strlen \
      == simdutf::count_utf8(      \
          (const char*)value, ((sizeof value) / sizeof(value[0]) - 1)));

#define TEST_STRLEN16(value)        \
  REQUIRE(                          \
      uni::details::len16(value).strlen \
      == simdutf::count_utf16(value, ((sizeof value) / sizeof(value[0]) - 1)));

  SECTION("UTF8")
  {
    COLT_FOR_EACH(TEST_STRLEN8, TEST_STRING8);
  }
  SECTION("UTF16")
  {
    COLT_FOR_EACH(TEST_STRLEN16, TEST_STRING16);
  }

#undef TEST_STRLEN8
#undef TEST_STRLEN16
}

TEST_CASE("Unicode UTF32 to UTF8")
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
      A[i] = byteswap((u32)A[i]);

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
      A[i] = byteswap((u32)A[i]);

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
      A[i] = byteswap((u32)A[i]);

    auto from = ptr_to<const Char32Other*>(&A[0]);
    REQUIRE(to_utf8(from, 6, result, 8) == ConvError::NOT_ENOUGH_SPACE);
  }
}

TEST_CASE("Unicode Indexing")
{
  using namespace clt;
  using namespace clt::uni;
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