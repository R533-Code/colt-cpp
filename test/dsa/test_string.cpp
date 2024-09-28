/*****************************************************************/ /**
 * @file   test_string.cpp
 * @brief  Unit tests for `String`.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "../includes.h"
#include <colt/dsa/string.h>

using namespace clt;

using AllStrings = std::tuple<
    BasicString<
        clt::meta::StringCustomization{StringEncoding::UTF8, 24, true, true, true},
        mem::Mallocator>,
    BasicString<
        clt::meta::StringCustomization{StringEncoding::UTF8, 16, true, true, true},
        mem::Mallocator>,
    BasicString<
        clt::meta::StringCustomization{StringEncoding::UTF8, 8, true, true, true},
        mem::Mallocator>,
    BasicString<
        clt::meta::StringCustomization{StringEncoding::UTF8, 0, true, true, true},
        mem::Mallocator>>;

TEMPLATE_LIST_TEST_CASE("BasicString", "[BasicString]", AllStrings)
{
  using namespace clt;

  using StringType = TestType;

  SECTION("Index long")
  {
    u8StringView a = "10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC"
                     "\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC"_UTF8;
    StringType ab  = {mem::Mallocator{}, a};
    REQUIRE(ab.front() == U'1');
    REQUIRE(ab[0] == U'1');
    REQUIRE(ab[1] == U'0');
    REQUIRE(ab[2] == U'\u03BC');
    REQUIRE(ab[3] == U'\u00BC');
    REQUIRE(ab.back() == U'\u00BC');
  }
  SECTION("Index short")
  {
    u8StringView a = "10\u03BC\u00BC"_UTF8;
    StringType ab  = {mem::Mallocator{}, a};
    REQUIRE(ab.front() == U'1');
    REQUIRE(ab[0] == U'1');
    REQUIRE(ab[1] == U'0');
    REQUIRE(ab[2] == U'\u03BC');
    REQUIRE(ab[3] == U'\u00BC');
    REQUIRE(ab.back() == U'\u00BC');
  }
  SECTION("Add long")
  {
    u8StringView a = "10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC"
                     "\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC"_UTF8;
    StringType ab  = {mem::Mallocator{}, a};
    ab += "Hello World!"_UTF8;
    REQUIRE(ab.front() == U'1');
    REQUIRE(ab[0] == U'1');
    REQUIRE(ab[1] == U'0');
    REQUIRE(ab[2] == U'\u03BC');
    REQUIRE(ab[3] == U'\u00BC');
    REQUIRE(ab.back() == U'!');
    REQUIRE(ab.index_back(1) == U'd');
  }
  SECTION("Add short")
  {
    u8StringView a = "10\u03BC\u00BC"_UTF8;
    StringType ab  = {mem::Mallocator{}, a};
    ab += "d!"_UTF8;
    REQUIRE(ab.front() == U'1');
    REQUIRE(ab[0] == U'1');
    REQUIRE(ab[1] == U'0');
    REQUIRE(ab[2] == U'\u03BC');
    REQUIRE(ab[3] == U'\u00BC');
    REQUIRE(ab.back() == U'!');
    REQUIRE(ab.index_back(1) == U'd');
  }
  SECTION("Pop back short")
  {
    StringType ab = {mem::Mallocator{}, "10\u03BC\u00BC"_UTF8};
    ab.pop_back();
    REQUIRE(ab.front() == U'1');
    REQUIRE(ab[0] == U'1');
    REQUIRE(ab[1] == U'0');
    REQUIRE(ab.size() == 3);
    ab.pop_back_n(2);
    REQUIRE(ab[0] == U'1');
    REQUIRE(ab.size() == 1);
    REQUIRE(ab == "1"_UTF8);
  }
  SECTION("Pop back long")
  {
    StringType ab = {
        mem::Mallocator{},
        "10\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC\u00BC10\u03BC"
        "\u00BC10\u03BC\u00BC10\u03BC\u00BC\u00BC10\u03BC\u00BC10\u03BC"
        "\u00BC10\u03BC\u00BC10\u03BC\u00BC"_UTF8};
    ab.pop_back();
    REQUIRE(ab.front() == U'1');
    REQUIRE(ab[0] == U'1');
    REQUIRE(ab[1] == U'0');
    ab.pop_back_n(2);
    REQUIRE(ab[0] == U'1');
  }
}
