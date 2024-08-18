#include "../includes.h"
#include <colt/meta/reflect.h>

struct AB
{
  int _ab;
  float _abc;
  
  constexpr auto operator<=>(const AB&) const = default;
  
  COLT_ENABLE_REFLECTION();
};

COLT_DECLARE_TYPE(AB, _ab, _abc);

TEST_CASE("reflect", "[reflect::str]")
{
  using namespace clt;
  using namespace clt::meta;  
  REQUIRE(reflect<i32>::str() == "i32");
  REQUIRE(reflect<i32*>::str() == "i32*");
  REQUIRE(reflect<i32&>::str() == "i32&");
  REQUIRE(reflect<i32&&>::str() == "i32&&");
  REQUIRE(reflect<const i32>::str() == "const i32");
  REQUIRE(reflect<const i32*>::str() == "const i32*");
  REQUIRE(reflect<const i32&>::str() == "const i32&");
  REQUIRE(reflect<const i32&&>::str() == "const i32&&");
  REQUIRE(reflect<volatile i32>::str() == "volatile i32");
  REQUIRE(reflect<volatile i32*>::str() == "volatile i32*");
  REQUIRE(reflect<volatile i32&>::str() == "volatile i32&");
  REQUIRE(reflect<volatile i32&&>::str() == "volatile i32&&");
  REQUIRE(reflect<const volatile i32>::str() == "const volatile i32");
  REQUIRE(reflect<const volatile i32*>::str() == "const volatile i32*");
  REQUIRE(reflect<const volatile i32&>::str() == "const volatile i32&");
  REQUIRE(reflect<const volatile i32&&>::str() == "const volatile i32&&");  
}

TEST_CASE("reflect", "[serialize]")
{
  // Create both a vector of bytes, input and output archives.
  auto [data, in, out] = zpp::bits::data_in_out();
  out(AB{10, 20}).or_throw();
  AB value = {0, 0};
  in(value).or_throw();
  REQUIRE(value == AB{10, 20});
}
