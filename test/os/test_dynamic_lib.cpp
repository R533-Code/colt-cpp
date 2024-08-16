#include <catch2/catch_all.hpp>
#include <colt/os/dynamic_lib.h>

extern "C" CLT_EXPORT int CLT_test_export()
{
  return 1029384756;
}

TEST_CASE("DynamicLib", "[dynamic_lib]")
{
  using namespace clt;
  using namespace clt::os;
  
  auto current = DynamicLib::open();
  auto invalid = DynamicLib::open("1234567890qwertyuiopasdfghjklzxcvbnm");
  REQUIRE(invalid.is_none());
  REQUIRE(current.is_value());
  REQUIRE(current->find_symbol("1234567890qwertyuiopasdfghjklzxcvbnm") == nullptr);
  REQUIRE(current->find_symbol("CLT_test_export") == &CLT_test_export);
  REQUIRE((*current->find<int (*)(void)>("CLT_test_export"))() == 1029384756);
}