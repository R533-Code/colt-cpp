#include <catch2/catch_all.hpp>
#include <colt/bit/endian.h>

TEST_CASE("Endianness Conversions", "[convert_endian]")
{
  using namespace clt::bit;
  if constexpr (TargetEndian::native == TargetEndian::little)
  {
    u16 host = 0x80'00;

    REQUIRE(htol(host) == 0x80'00);
    REQUIRE(htob(host) == 0x00'80);
    REQUIRE(byteswap(host) == 0x00'80);
  }
  else // big endian
  {
    u16 host = 0x00'80;

    REQUIRE(htol(host) == 0x80'00);
    REQUIRE(htob(host) == 0x00'80);
    REQUIRE(byteswap(host) == 0x80'00);
  }
}