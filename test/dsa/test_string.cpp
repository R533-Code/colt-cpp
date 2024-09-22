/*****************************************************************/ /**
 * @file   test_string.cpp
 * @brief  Unit tests for `String`.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "../includes.h"
#include <colt/dsa/string.h>

TEST_CASE("String Indexing")
{
  using namespace clt;

  SECTION("UTF8")
  {
    u8StringView a               = ptr_to<const Char8*>(u8"10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC10\u03BC\u00BC");
    u8String<mem::Mallocator> ab = {mem::Mallocator{}, a};
    REQUIRE(ab.front() == U'1');
    REQUIRE(ab[0] == U'1');
    REQUIRE(ab[1] == U'0');
    REQUIRE(ab[2] == U'\u03BC');
    REQUIRE(ab[3] == U'\u00BC');
    REQUIRE(ab.back() == U'\u00BC');
  }
}