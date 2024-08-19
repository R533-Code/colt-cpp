/*****************************************************************/ /**
 * @file   test_string_view.cpp
 * @brief  Unit tests for `StringView`.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "../includes.h"
#include <colt/dsa/string_view.h>

TEST_CASE("StringView Indexing")
{
  using namespace clt;
  SECTION("UTF8")
  {
    u8StringView a = ptr_to<const Char8*>(u8"10\u03BC\u00BC");
    REQUIRE(a.front() == U'1');
    REQUIRE(a[0] == U'1');
    REQUIRE(a[1] == U'0');
    REQUIRE(a[2] == U'\u03BC');
    REQUIRE(a[3] == U'\u00BC');
    REQUIRE(a.back() == U'\u00BC');

    a.pop_front();
    REQUIRE(a.front() == U'0');
    REQUIRE(a[0] == U'0');

    a.pop_back();
    REQUIRE(a.back() == U'\u03BC');
    REQUIRE(a[1] == U'\u03BC');
  }
  SECTION("UTF16")
  {
    u16StringView a = ptr_to<const Char16*>(u"10\u03BC\u00BC");
    REQUIRE(a.front() == U'1');
    REQUIRE(a[0] == U'1');
    REQUIRE(a[1] == U'0');
    REQUIRE(a[2] == U'\u03BC');
    REQUIRE(a[3] == U'\u00BC');
    REQUIRE(a.back() == U'\u00BC');

    a.pop_front();
    REQUIRE(a.front() == U'0');
    REQUIRE(a[0] == U'0');

    a.pop_back();
    REQUIRE(a.back() == U'\u03BC');
    REQUIRE(a[1] == U'\u03BC');
  }
  SECTION("UTF32")
  {
    u32StringView a = ptr_to<const Char32*>(U"10\u03BC\u00BC");
    REQUIRE(a.front() == U'1');
    REQUIRE(a[0] == U'1');
    REQUIRE(a[1] == U'0');
    REQUIRE(a[2] == U'\u03BC');
    REQUIRE(a[3] == U'\u00BC');
    REQUIRE(a.back() == U'\u00BC');

    a.pop_front();
    REQUIRE(a.front() == U'0');
    REQUIRE(a[0] == U'0');

    a.pop_back();
    REQUIRE(a.back() == U'\u03BC');
    REQUIRE(a[1] == U'\u03BC');
  }
}

TEST_CASE("StringView Serialization")
{
  using namespace clt;

  // Create both a vector of bytes, input and output archives.
  auto [data, in, out] = zpp::bits::data_in_out();

  out(StringView{"Hello World!"}).or_throw();
  StringView value;
  in(value).or_throw();
  REQUIRE(value == "Hello World!");
}
