#include <catch2/catch_all.hpp>
#include <colt/dsa/unicode.h>

TEST_CASE("Unicode Indexing", "[index_back index_front]")
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

TEST_CASE("Unicode Length", "[strlen]")
{
  using namespace clt;
  using namespace clt::uni;
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

TEST_CASE("Unicode Size", "[unitlen]")
{
  using namespace clt;
  using namespace clt::uni;
  using enum StringEncoding;

#define TEST_UNITLEN(value) unitlen(value) == ((sizeof value) / sizeof(value[0]) - 1)

  SECTION("UTF8")
  {
    REQUIRE(TEST_UNITLEN(u8"\u000D"));
    REQUIRE(TEST_UNITLEN(u8"\u00b1"));
    REQUIRE(TEST_UNITLEN(u8"\u03b1"));
    REQUIRE(TEST_UNITLEN(u8"\u03BA\u1F79\u03C3\u03BC\u03B5"));
    REQUIRE(TEST_UNITLEN(u8"\u0288\u0119\u0835\u0E34\u021B"));
  }
  SECTION("UTF16")
  {
    REQUIRE(TEST_UNITLEN(u"\u000D"));
    REQUIRE(TEST_UNITLEN(u"\u00b1"));
    REQUIRE(TEST_UNITLEN(u"\u03b1"));
    REQUIRE(TEST_UNITLEN(u"\u03BA\u1F79\u03C3\u03BC\u03B5"));
    REQUIRE(TEST_UNITLEN(u"\u0288\u0119\u0835\u0E34\u021B"));
  }
  SECTION("UTF32")
  {
    REQUIRE(TEST_UNITLEN(U"\u000D"));
    REQUIRE(TEST_UNITLEN(U"\u00b1"));
    REQUIRE(TEST_UNITLEN(U"\u03b1"));
    REQUIRE(TEST_UNITLEN(U"\u03BA\u1F79\u03C3\u03BC\u03B5"));
    REQUIRE(TEST_UNITLEN(U"\u0288\u0119\u0835\u0E34\u021B"));
  }

#undef TEST_UNITLEN
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

TEST_CASE("Unicode SIMD unitlen", "[unitlen16,unitlen32]")
{
  using namespace clt;
  using namespace clt::uni;
#define TEST_UNITLEN16(value) \
  unitlen16(value) == ((sizeof value) / sizeof(value[0]) - 1)
#define TEST_UNITLEN32(value) \
  unitlen32(value) == ((sizeof value) / sizeof(value[0]) - 1)

  SECTION("UTF16")
  {
    REQUIRE(TEST_UNITLEN16(u""));
    REQUIRE(TEST_UNITLEN16(u"0"));
    REQUIRE(TEST_UNITLEN16(u"01"));
    REQUIRE(TEST_UNITLEN16(u"012"));
    REQUIRE(TEST_UNITLEN16(u"0123"));
    REQUIRE(TEST_UNITLEN16(u"01234"));
    REQUIRE(TEST_UNITLEN16(u"012345"));
    REQUIRE(TEST_UNITLEN16(u"0123456"));
    REQUIRE(TEST_UNITLEN16(u"01234567"));
    REQUIRE(TEST_UNITLEN16(u"012345678"));
    REQUIRE(TEST_UNITLEN16(u"0123456789"));
    // Size greater than 32
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890"));
    REQUIRE(TEST_UNITLEN16(u"12345678901234567890123456789012345678900"));
    REQUIRE(TEST_UNITLEN16(u"123456789012345678901234567890123456789001"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890012"));
    REQUIRE(TEST_UNITLEN16(u"12345678901234567890123456789012345678900123"));
    REQUIRE(TEST_UNITLEN16(u"123456789012345678901234567890123456789001234"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890012345"));
    REQUIRE(TEST_UNITLEN16(u"12345678901234567890123456789012345678900123456"));
    REQUIRE(TEST_UNITLEN16(u"123456789012345678901234567890123456789001234567"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890012345678"));
    REQUIRE(TEST_UNITLEN16(u"12345678901234567890123456789012345678900123456789"));
    // Size greater than 64
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890123456789012345"
                           u"6789012345678901234567890"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890123456789012345"
                           u"67890123456789012345678900"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890123456789012345"
                           u"678901234567890123456789001"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890123456789012345"
                           u"6789012345678901234567890012"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890123456789012345"
                           u"67890123456789012345678900123"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890123456789012345"
                           u"678901234567890123456789001234"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890123456789012345"
                           u"6789012345678901234567890012345"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890123456789012345"
                           u"67890123456789012345678900123456"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890123456789012345"
                           u"678901234567890123456789001234567"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890123456789012345"
                           u"6789012345678901234567890012345678"));
    REQUIRE(TEST_UNITLEN16(u"1234567890123456789012345678901234567890123456789012345"
                           u"67890123456789012345678900123456789"));
    REQUIRE(TEST_UNITLEN16(u"This is a small test!"));
    REQUIRE(TEST_UNITLEN16(
        u"\u65E0\u53EF\u5426\u8BA4\uFF0C\u5F53\u8BFB\u8005\u5728\u6D4F\u89C8\u4E00"
        u"\u4E2A\u9875\u9762\u7684\u6392\u7248\u65F6\uFF0C\u96BE\u514D\u4F1A\u88AB"
        u"\u53EF\u9605\u8BFB\u7684\u5185\u5BB9\u6240\u5206\u6563\u6CE8\u610F\u529B"
        u"\u3002Lorem "
        u"Ipsum\u7684\u76EE\u7684\u5C31\u662F\u4E3A\u4E86\u4FDD\u6301\u5B57\u6BCD"
        u"\u591A\u591A\u5C11\u5C11\u6807\u51C6\u53CA\u5E73\u5747\u7684\u5206\u914D"
        u"\uFF0C\u800C\u4E0D\u662F\u201C\u6B64\u5904\u6709\u6587\u672C\uFF0C\u6B64"
        u"\u5904\u6709\u6587\u672C\u201D\uFF0C\u4ECE\u800C\u8BA9\u5185\u5BB9\u66F4"
        u"\u50CF\u53EF\u8BFB\u7684\u82F1\u8BED\u3002\u5982\u4ECA\uFF0C\u5F88\u591A"
        u"\u684C\u9762\u6392\u7248\u8F6F\u4EF6\u4EE5\u53CA\u7F51\u9875\u7F16\u8F91"
        u"\u7528Lorem "
        u"Ipsum\u4F5C\u4E3A\u9ED8\u8BA4\u7684\u793A\u8303\u6587\u672C\uFF0C\u641C"
        u"\u4E00\u641C\u201CLorem "
        u"Ipsum\u201D\u5C31\u80FD\u627E\u5230\u8FD9\u4E9B\u7F51\u7AD9\u7684\u96CF"
        u"\u5F62\u3002\u8FD9\u4E9B\u5E74\u6765Lorem "
        u"Ipsum\u6F14\u53D8\u51FA\u4E86\u5404\u5F0F\u5404\u6837\u7684\u7248\u672C"
        u"\uFF0C\u6709\u4E9B\u51FA\u4E8E\u5076\u7136\uFF0C\u6709\u4E9B\u5219\u662F"
        u"\u6545\u610F\u7684\uFF08\u523B\u610F\u7684\u5E7D\u9ED8\u4E4B\u7C7B\u7684"
        u"\uFF09\u3002"));
    REQUIRE(TEST_UNITLEN16(
        u"Sed ut perspiciatis unde omnis iste natus error sit voluptatem "
        u"accusantium "
        u"doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo "
        u"inventore "
        u"veritatis et quasi architecto beatae vitae dicta sunt explicabo. Nemo "
        u"enim "
        u"ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed "
        u"quia "
        u"consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt. "
        u"Neque "
        u"porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, "
        u"adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore "
        u"et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, "
        u"quis "
        u"nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut "
        u"aliquid "
        u"ex ea commodi consequatur? Quis autem vel eum iure reprehenderit qui in "
        u"ea "
        u"voluptate velit esse quam nihil molestiae consequatur, vel illum qui "
        u"dolorem eum fugiat quo voluptas nulla pariatur?"));
  }
  SECTION("UTF32")
  {
    REQUIRE(TEST_UNITLEN32(U""));
    REQUIRE(TEST_UNITLEN32(U"0"));
    REQUIRE(TEST_UNITLEN32(U"01"));
    REQUIRE(TEST_UNITLEN32(U"012"));
    REQUIRE(TEST_UNITLEN32(U"0123"));
    REQUIRE(TEST_UNITLEN32(U"01234"));
    REQUIRE(TEST_UNITLEN32(U"012345"));
    REQUIRE(TEST_UNITLEN32(U"0123456"));
    REQUIRE(TEST_UNITLEN32(U"01234567"));
    REQUIRE(TEST_UNITLEN32(U"012345678"));
    REQUIRE(TEST_UNITLEN32(U"0123456789"));
    REQUIRE(TEST_UNITLEN32(U"This is a small test!"));
    REQUIRE(TEST_UNITLEN32(
        U"\u65E0\u53EF\u5426\u8BA4\uFF0C\u5F53\u8BFB\u8005\u5728\u6D4F\u89C8\u4E00"
        U"\u4E2A\u9875\u9762\u7684\u6392\u7248\u65F6\uFF0C\u96BE\u514D\u4F1A\u88AB"
        U"\u53EF\u9605\u8BFB\u7684\u5185\u5BB9\u6240\u5206\u6563\u6CE8\u610F\u529B"
        U"\u3002Lorem "
        U"Ipsum\u7684\u76EE\u7684\u5C31\u662F\u4E3A\u4E86\u4FDD\u6301\u5B57\u6BCD"
        U"\u591A\u591A\u5C11\u5C11\u6807\u51C6\u53CA\u5E73\u5747\u7684\u5206\u914D"
        U"\uFF0C\u800C\u4E0D\u662F\u201C\u6B64\u5904\u6709\u6587\u672C\uFF0C\u6B64"
        U"\u5904\u6709\u6587\u672C\u201D\uFF0C\u4ECE\u800C\u8BA9\u5185\u5BB9\u66F4"
        U"\u50CF\u53EF\u8BFB\u7684\u82F1\u8BED\u3002\u5982\u4ECA\uFF0C\u5F88\u591A"
        U"\u684C\u9762\u6392\u7248\u8F6F\u4EF6\u4EE5\u53CA\u7F51\u9875\u7F16\u8F91"
        U"\u7528Lorem "
        U"Ipsum\u4F5C\u4E3A\u9ED8\u8BA4\u7684\u793A\u8303\u6587\u672C\uFF0C\u641C"
        U"\u4E00\u641C\u201CLorem "
        U"Ipsum\u201D\u5C31\u80FD\u627E\u5230\u8FD9\u4E9B\u7F51\u7AD9\u7684\u96CF"
        U"\u5F62\u3002\u8FD9\u4E9B\u5E74\u6765Lorem "
        U"Ipsum\u6F14\u53D8\u51FA\u4E86\u5404\u5F0F\u5404\u6837\u7684\u7248\u672C"
        U"\uFF0C\u6709\u4E9B\u51FA\u4E8E\u5076\u7136\uFF0C\u6709\u4E9B\u5219\u662F"
        U"\u6545\u610F\u7684\uFF08\u523B\u610F\u7684\u5E7D\u9ED8\u4E4B\u7C7B\u7684"
        U"\uFF09\u3002"));
    REQUIRE(TEST_UNITLEN32(
        U"Sed ut perspiciatis unde omnis iste natus error sit voluptatem "
        U"accusantium "
        U"doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo "
        U"inventore "
        U"veritatis et quasi architecto beatae vitae dicta sunt explicabo. Nemo "
        U"enim "
        U"ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed "
        U"quia "
        U"consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt. "
        U"Neque "
        U"porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, "
        U"adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore "
        U"et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, "
        U"quis "
        U"nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut "
        U"aliquid "
        U"ex ea commodi consequatur? Quis autem vel eum iure reprehenderit qui in "
        U"ea "
        U"voluptate velit esse quam nihil molestiae consequatur, vel illum qui "
        U"dolorem eum fugiat quo voluptas nulla pariatur?"));
  }

#undef TEST_UNITLEN16
#undef TEST_UNITLEN32
}