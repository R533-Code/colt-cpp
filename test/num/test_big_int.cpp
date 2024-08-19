/*****************************************************************/ /**
 * @file   test_big_int.cpp
 * @brief  Unit tests for `BigInt` and `BigRational`.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "../includes.h"
#include <colt/num/big_int.h>
#include <colt/num/big_rational.h>

TEST_CASE("BigInt")
{
  using namespace clt;
  using namespace clt::num;

  SECTION("Addition")
  {
    SECTION("Positive numbers")
    {
      auto a = *BigInt::from("12345678901234567890");
      auto b = *BigInt::from("98765432109876543210");
      REQUIRE((a + b) == *BigInt::from("111 111 111 011 111 111 100"));
    }

    SECTION("Negative numbers")
    {
      auto a = *BigInt::from("-12345678901234567890");
      auto b = *BigInt::from("-98765432109876543210");
      REQUIRE((a + b) == *BigInt::from("-111 111 111 011 111 111 100"));
    }

    SECTION("Mixed signs")
    {
      auto a = *BigInt::from("-12345678901234567890");
      auto b = *BigInt::from("98765432109876543210");
      REQUIRE((a + b) == *BigInt::from("86 419 753 208 641 975 320"));
    }

    SECTION("Zero")
    {
      auto a = BigInt{0};
      auto b = *BigInt::from("12345678901234567890");
      REQUIRE((a + b) == b);
    }
  }
  SECTION("Subtraction")
  {
    SECTION("Positive numbers")
    {
      auto a = *BigInt::from("12345678901234567890");
      auto b = *BigInt::from("98765432109876543210");
      REQUIRE((a - b) == *BigInt::from("-86 419 753 208 641 975 320"));
    }

    SECTION("Negative numbers")
    {
      auto a = *BigInt::from("-12345678901234567890");
      auto b = *BigInt::from("-98765432109876543210");
      REQUIRE((a - b) == *BigInt::from("86 419 753 208 641 975 320"));
    }

    SECTION("Mixed signs")
    {
      auto a = *BigInt::from("-12345678901234567890");
      auto b = *BigInt::from("98765432109876543210");
      REQUIRE((a - b) == *BigInt::from("-111 111 111 011 111 111 100"));
    }

    SECTION("Zero")
    {
      auto a = BigInt{0};
      auto b = *BigInt::from("12345678901234567890");
      REQUIRE((a - b) == *BigInt::from("-12345678901234567890"));
      REQUIRE((b - a) == b);
    }
  }
  SECTION("Multiplication")
  {
    SECTION("Positive numbers")
    {
      auto a = *BigInt::from("12345678901234567890");
      auto b = *BigInt::from("98765432109876543210");
      REQUIRE(
          (a * b)
          == *BigInt::from("1 219 326 311 370 217 952 237 463 801 111 263 526 900"));
    }

    SECTION("Negative numbers")
    {
      auto a = *BigInt::from("-12345678901234567890");
      auto b = *BigInt::from("-98765432109876543210");
      REQUIRE(
          (a * b)
          == *BigInt::from("1 219 326 311 370 217 952 237 463 801 111 263 526 900"));
    }

    SECTION("Mixed signs")
    {
      auto a = *BigInt::from("-12345678901234567890");
      auto b = *BigInt::from("98765432109876543210");
      REQUIRE(
          (a * b)
          == *BigInt::from(
              "-1 219 326 311 370 217 952 237 463 801 111 263 526 900"));
    }

    SECTION("Zero")
    {
      auto a = BigInt{0};
      auto b = *BigInt::from("12345678901234567890");
      REQUIRE((a * b) == BigInt{0});
      REQUIRE((b * a) == BigInt{0});
    }
  }
  SECTION("Division")
  {
    SECTION("Positive numbers")
    {
      auto a = *BigInt::from("122345678901234567890");
      auto b = *BigInt::from("98765432109876543210");
      REQUIRE((a / b) == BigInt{1});
    }

    SECTION("Negative numbers")
    {
      auto a = *BigInt::from("-122345678901234567890");
      auto b = *BigInt::from("-98765432109876543210");
      REQUIRE((a / b) == BigInt{1});
    }

    SECTION("Mixed signs")
    {
      auto a = *BigInt::from("-122345678901234567890");
      auto b = *BigInt::from("98765432109876543210");
      REQUIRE((a / b) == BigInt{-1});
    }

    SECTION("Zero")
    {
      auto a = BigInt{0};
      auto b = *BigInt::from("12345678901234567890");
      REQUIRE((a / b) == BigInt{0});
    }
  }
  SECTION("Modulo")
  {
    SECTION("Positive numbers")
    {
      auto a = *BigInt::from("12345678901234567890");
      auto b = *BigInt::from("98765432109876543210");
      REQUIRE((a % b) == *BigInt::from("12345678901234567890"));
    }

    SECTION("Negative numbers")
    {
      auto a = *BigInt::from("-12345678901234567890");
      auto b = *BigInt::from("-98765432109876543210");
      REQUIRE((a % b) == *BigInt::from("86419753208641975320"));
    }

    SECTION("Mixed signs")
    {
      auto a = *BigInt::from("-12345678901234567890");
      auto b = *BigInt::from("98765432109876543210");
      REQUIRE((a % b) == *BigInt::from("86419753208641975320"));
    }

    SECTION("Zero")
    {
      auto a = BigInt{0};
      auto b = *BigInt::from("12345678901234567890");
      REQUIRE((a % b) == BigInt{0});
    }
  }
}