#include "../includes.h"
#include <colt/num/big_int.h>
#include <colt/num/big_rational.h>

TEST_CASE("BigInt")
{
  using namespace clt;
  using namespace clt::num;

  auto a = BigInt{1000000};
}

TEST_CASE("BigRational")
{
  using namespace clt;
  using namespace clt::num;
  
  auto b = BigRational{1000000};
}