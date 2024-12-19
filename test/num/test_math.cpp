/*****************************************************************/ /**
 * @file   test_math.cpp
 * @brief  Unit tests for `math` functions.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "../includes.h"
#include <colt/num/math.h>

TEST_CASE("Mathematical Functions")
{
  REQUIRE(clt::abs(0) == 0);
  REQUIRE(clt::abs(-0) == 0);

  REQUIRE(clt::abs(-1) == 1);
  REQUIRE(clt::abs(1) == 1);

  REQUIRE(clt::abs(1.0) == 1.0);
  REQUIRE(clt::abs(-1.0) == 1.0);

  REQUIRE(clt::sqrt(1) == 1);
  REQUIRE(clt::sqrt(1.0) == 1.0);

  REQUIRE(clt::sqrt(0) == 0);
  REQUIRE(clt::sqrt(4) == 2);

  REQUIRE(clt::sqrt(0.0) == 0.0);
  REQUIRE(clt::sqrt(16.0) == 4.0);

  REQUIRE(clt::pow(1, 1) == 1);
  REQUIRE(clt::pow(9, 0) == 1);

  REQUIRE(clt::pow(2, 4) == 16);
  REQUIRE(clt::pow(3, 3) == 27);

  REQUIRE(clt::pow(4.0, 0.5) == 2.0);
  REQUIRE(clt::pow(2.0, 8.0) == 256.0);
}