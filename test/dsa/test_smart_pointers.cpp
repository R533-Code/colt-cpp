/*****************************************************************/ /**
 * @file   test_smart_pointers.cpp
 * @brief  Unit tests for `UniquePtr`.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "../includes.h"
#include <colt/dsa/smart_pointers.h>

TEST_CASE("UniquePtr")
{
  using namespace clt;

  mem::StackAllocator<128> alloc;

  auto ptr = make_unique<int>(10);
  REQUIRE(!ptr.is_null());
  REQUIRE(*ptr == 10);
  *ptr = 11;
  REQUIRE(*ptr == 11);
  ptr.reset();
  REQUIRE(ptr.is_null());
  auto lptr = make_local_unique<int>(alloc, 5);
  REQUIRE(!lptr.is_null());
  REQUIRE(*lptr == 5);
}