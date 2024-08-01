#include <catch2/catch_all.hpp>
#include <colt/bit/bitfields.h>

TEST_CASE("Bitfields", "[bitfields]")
{
  using namespace clt::bit;
  using namespace clt;

  using Type = Bitfields<u8, Bitfield<0, 5>, Bitfield<1, 2>, Bitfield<2, 1>>;

  auto a = Type(InPlace, 0b10100, 0b11, 0b1);
  REQUIRE(a.value() == 0b10100'11'1);
  
  a.set<0>(0);
  REQUIRE(a.value() == 0b00000'11'1);
  REQUIRE(a.get<0>() == 00000);
}