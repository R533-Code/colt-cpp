/*****************************************************************/ /**
 * @file   test_option.cpp
 * @brief  Unit tests for `Option`.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "../includes.h"
#include <string_view>
#include <colt/dsa/expect.h>

TEST_CASE("Expect")
{
  using namespace clt;

  Expect<u32, const char*> a;
  SECTION("constructor")
  {
    REQUIRE(a.is_expect());
    a = 10;
    REQUIRE(a.value() == 10);
  }
  SECTION("map")
  {
    a = Expect<u32, const char*>{Error, ""};
    REQUIRE(a.map([](u32 value) { return Expect<i32, const char*>{i32(value)}; })
                .is_error());
    a = 10;
    REQUIRE(a.map([](u32 value) { return i32(value + 200); }).value() == 210);
    REQUIRE(
        a.map([](u32 value) { return i32(value + 200); })
            .map([](i32 value) { return value - 200; })
            .value()
        == 10);
  }
  SECTION("value_or")
  {
    a = Expect<u32, const char*>{Error, ""};
    REQUIRE(a.value_or(10) == 10);
    a = 12;
    REQUIRE(a.value_or(10) == 12);
    REQUIRE(a.value_or([]() { return 10; }) == 12);
    a = Expect<u32, const char*>{Error, ""};
    REQUIRE(a.value_or([]() { return 10; }) == 10);
  }
  SECTION("or_else")
  {
    a = 12;
    REQUIRE(
        a.or_else([](const char*) { return Expect<int, const char*>{10}; }).value()
        == 12);
    a = Expect<u32, const char*>{Error, ""};
    REQUIRE(
        a.or_else([](const char*) { return Expect<int, const char*>{10}; }).value()
        == 10);
  }
  SECTION("and_then")
  {
    a = 12;
    REQUIRE(
        a.and_then(
             [](u32 a)
             {
               return a > 10 ? Expect<char, const char*>{'a'}
                             : Expect<char, const char*>{Error, "Error!"};
             })
            .value()
        == 'a');
    a = Expect<u32, const char*>{Error, ""};
    REQUIRE(a.and_then(
                 [](u32 a)
                 {
                   return a > 10 ? Expect<char, const char*>{'a'}
                                 : Expect<char, const char*>{Error, "Error!"};
                 })
                .is_error());
  }
  SECTION("serialize")
  {
    Expect<u32, u8> b = 12;
    // Create both a vector of bytes, input and output archives.
    auto [data, in, out] = zpp::bits::data_in_out();
    out(b).or_throw();
    Expect<u32, u8> value = 0;
    in(value).or_throw();
    REQUIRE(value.value_or(0) == 12);
  }
}
