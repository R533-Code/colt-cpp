#ifndef HG_NUM_BIG_INT
#define HG_NUM_BIG_INT

#include "gmp.h"

namespace clt::num
{
  class BigInt
  {
    mpz_t storage{};

  public:
    BigInt() noexcept
    {
      mpz_init(storage);
    }

    ~BigInt() noexcept
    {
      mpz_clear(storage);
    }
  };
}

#endif // !HG_NUM_BIG_INT
