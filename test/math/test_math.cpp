#include <catch2/catch_all.hpp>
#include <colt/num/math.h>

TEST_CASE("Absolute Values", "[absolute_value]")
{
  REQUIRE(clt::math::abs(0) == 0);
  REQUIRE(clt::math::abs(-0) == 0);
  
  REQUIRE(clt::math::abs(-1) == 1);
  REQUIRE(clt::math::abs(1) == 1);
  
  REQUIRE(clt::math::abs(1.0) == 1.0);
  REQUIRE(clt::math::abs(-1.0) == 1.0);
}

TEST_CASE("Square Root", "[square_root]")
{
  REQUIRE(clt::math::sqrt(1) == 1);
  REQUIRE(clt::math::sqrt(1.0) == 1.0);

  REQUIRE(clt::math::sqrt(0) == 0);
  REQUIRE(clt::math::sqrt(4) == 2);
  
  REQUIRE(clt::math::sqrt(0.0) == 0.0);
  REQUIRE(clt::math::sqrt(16.0) == 4.0);
}

TEST_CASE("Power", "[power]")
{
  REQUIRE(clt::math::pow(1, 1) == 1);
  REQUIRE(clt::math::pow(9, 0) == 1);

  REQUIRE(clt::math::pow(2, 4) == 16);
  REQUIRE(clt::math::pow(3, 3) == 27);

  REQUIRE(clt::math::pow(4.0, 0.5) == 2.0);
  REQUIRE(clt::math::pow(2.0, 8.0) == 256.0);
}