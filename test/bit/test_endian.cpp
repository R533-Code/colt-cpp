/*****************************************************************/ /**
 * @file   test_endian.cpp
 * @brief  Unit tests for `byteswap` and other endianness related functions.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "../includes.h"
#include <colt/bit/endian.h>

TEST_CASE("Endianness Conversions")
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