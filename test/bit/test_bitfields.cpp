/*****************************************************************/ /**
 * @file   test_bitfields.cpp
 * @brief  Unit tests for `Bitfields`.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "../includes.h"
#include <colt/bit/bitfields.h>

TEST_CASE("Bitfields")
{
  using namespace clt::bit;
  using namespace clt;

  using Type = Bitfields<u8, Bitfield<0, 5>, Bitfield<1, 2>, Bitfield<2, 1>>;

  auto a = Type(InPlace, 0b10100, 0b11, 0b1);
  REQUIRE(a.value() == 0b10100'11'1);

  a.set<0>(0);
  REQUIRE(a.value() == 0b00000'11'1);
  REQUIRE(a.get<0>() == 0);
  a.set<1>(0);
  REQUIRE(a.value() == 0b00000'00'1);
  REQUIRE(a.get<1>() == 0);
}