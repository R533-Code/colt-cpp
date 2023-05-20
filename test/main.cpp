#include "coltcpp.h"

using namespace std::chrono_literals;
using namespace clt;
using namespace clt::io;
using namespace clt::mem;

struct D
{
  f64 a;
};
COLT_DECLARE_TYPE(D, a);

struct AB
{
  u32 a;
  u32 b;
  D d;
};
COLT_DECLARE_TYPE(AB, a, b, d);

int main(int argc, char** argv)
{
  clt::install_colt_handlers();
  
  io::print("{}, {}, {}, {}, {}\n{}",
    reflect<u32>::str(),
    reflect<const u32>::str(),
    reflect<const volatile u32>::str(),
    reflect<volatile PTR<const PTR<const volatile u32>>>::str(),
    reflect<const u32&>::str(),
    AB{ 10, 10, { 10.8 } }
  );
  press_to_continue();
}
