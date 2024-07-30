#include <catch2/catch_all.hpp>
#include <colt/math/math.h>

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