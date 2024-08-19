/*****************************************************************/ /**
 * @file   test_option.cpp
 * @brief  Unit tests for `Option`.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "../includes.h"
#include <colt/dsa/option.h>

TEST_CASE("Option")
{
  using namespace clt;

  Option<u32> a;
  SECTION("constructor")
  {
    REQUIRE(a.is_none());
    a = 10;
    REQUIRE(a.value() == 10);
    a.reset();
  }
  SECTION("map")
  {
    REQUIRE(a.map([](u32 value) { return i32(value); }).is_none());
    a = 10;
    REQUIRE(a.map([](u32 value) { return i32(value + 200); }).value() == 210);
    REQUIRE(
        a.map([](u32 value) { return i32(value + 200); })
            .map([](i32 value) { return value - 200; })
            .value()
        == 10);
    a.reset();
  }
  SECTION("value_or")
  {
    REQUIRE(a.value_or(10) == 10);
    a = 12;
    REQUIRE(a.value_or(10) == 12);
    REQUIRE(a.value_or([]() { return 10; }) == 12);
    a = None;
    REQUIRE(a.value_or([]() { return 10; }) == 10);
    a.reset();
  }
  SECTION("or_else")
  {
    a = 12;
    REQUIRE(a.or_else([]() { return 10; }).value() == 12);
    a = None;
    REQUIRE(a.or_else([]() { return 10; }).value() == 10);
    a.reset();
  }
  SECTION("and_then")
  {
    a = 12;
    REQUIRE(
        a.and_then([](u32 a) { return a > 10 ? Option{'a'} : None; }).value()
        == 'a');
    a = None;
    REQUIRE(a.and_then([](u32 a) { return a > 10 ? Option{'a'} : None; }).is_none());
    a.reset();
  }
  SECTION("serialize")
  {
    a = 12;
    // Create both a vector of bytes, input and output archives.
    auto [data, in, out] = zpp::bits::data_in_out();
    out(a).or_throw();
    Option<u32> value = 0;
    in(value).or_throw();
    REQUIRE(value.value_or(0) == 12);
  }
}
