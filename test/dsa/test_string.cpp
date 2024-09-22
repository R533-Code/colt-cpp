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
    u8StringView a =
        ptr_to<const Char8*>(u8"10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC"
                             u8"\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC");
    StringType ab = {mem::Mallocator{}, a};
    REQUIRE(ab.front() == U'1');
    REQUIRE(ab[0] == U'1');
    REQUIRE(ab[1] == U'0');
    REQUIRE(ab[2] == U'\u03BC');
    REQUIRE(ab[3] == U'\u00BC');
    REQUIRE(ab.back() == U'\u00BC');
  }
  SECTION("Index short")
  {
    u8StringView a =
        ptr_to<const Char8*>(u8"10\u03BC\u00BC");
    StringType ab = {mem::Mallocator{}, a};
    REQUIRE(ab.front() == U'1');
    REQUIRE(ab[0] == U'1');
    REQUIRE(ab[1] == U'0');
    REQUIRE(ab[2] == U'\u03BC');
    REQUIRE(ab[3] == U'\u00BC');
    REQUIRE(ab.back() == U'\u00BC');
  }
  SECTION("Add long")
  {
    u8StringView a =
        ptr_to<const Char8*>(u8"10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC"
                             u8"\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC");
    StringType ab = {mem::Mallocator{}, a};
    ab += u8StringView{ptr_to<const Char8*>(u8"Hello World!")};
    REQUIRE(ab.front() == U'1');
    REQUIRE(ab[0] == U'1');
    REQUIRE(ab[1] == U'0');
    REQUIRE(ab[2] == U'\u03BC');
    REQUIRE(ab[3] == U'\u00BC');
    REQUIRE(ab.back() == U'!');
    REQUIRE(ab.index_back(1) == U'd');
  }
  SECTION("Add long")
  {
    u8StringView a =
        ptr_to<const Char8*>(u8"10\u03BC\u00BC");
    StringType ab = {mem::Mallocator{}, a};
    ab += u8StringView{ptr_to<const Char8*>(u8"d!")};
    REQUIRE(ab.front() == U'1');
    REQUIRE(ab[0] == U'1');
    REQUIRE(ab[1] == U'0');
    REQUIRE(ab[2] == U'\u03BC');
    REQUIRE(ab[3] == U'\u00BC');
    REQUIRE(ab.back() == U'!');
    REQUIRE(ab.index_back(1) == U'd');
  }
}