#include "../includes.h"
#include <colt/meta/map.h>

TEST_CASE("Map", "[meta_map]")
{
  using pair = std::pair<int, int>;
  auto map   = clt::meta::Map{std::array{pair{0, 0}, pair{1, 5}, pair{2, 10}}};
  auto _0    = map.find(0);
  auto _5    = map.find(1);
  auto _10   = map.find(2);
  auto _None = map.find(6);

  REQUIRE((_0.is_value() && *_0 == 0));
  REQUIRE((_5.is_value() && *_5 == 5));
  REQUIRE((_10.is_value() && *_10 == 10));
  REQUIRE(_None.is_none());
}

TEST_CASE("BiMap", "[meta_bimap]")
{
  using pair = std::pair<int, int>;
  auto map   = clt::meta::BiMap{std::array{pair{0, 0}, pair{1, 5}, pair{2, 10}}};
  auto _0    = map.find_value(0);
  auto _5    = map.find_value(1);
  auto _10   = map.find_value(2);
  auto _None = map.find_value(6);
  auto _k0    = map.find_key(0);
  auto _k5    = map.find_key(5);
  auto _k10   = map.find_key(10);
  auto _kNone = map.find_key(6);

  REQUIRE((_0.is_value() && *_0 == 0));
  REQUIRE((_5.is_value() && *_5 == 5));
  REQUIRE((_10.is_value() && *_10 == 10));
  REQUIRE(_None.is_none());
  REQUIRE((_k0.is_value() && *_k0 == 0));
  REQUIRE((_k5.is_value() && *_k5 == 1));
  REQUIRE((_k10.is_value() && *_k10 == 2));
  REQUIRE(_kNone.is_none());
}