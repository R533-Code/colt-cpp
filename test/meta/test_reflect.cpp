#include "../includes.h"
#include <colt/meta/reflect.h>

TEST_CASE("reflect", "[reflect::str]")
{
  using namespace clt;
  using namespace clt::meta;  
  REQUIRE(reflect<i32>::str() == "i32");
  REQUIRE(reflect<const i32>::str() == "const i32");
  REQUIRE(reflect<volatile i32>::str() == "volatile i32");
  REQUIRE(reflect<const volatile i32>::str() == "const volatile i32");
}
