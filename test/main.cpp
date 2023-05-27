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
  
  io::print("{}, {}",
    AB{ 10, 10, D{ 1.2} },
    refl::EntityKind::IS_BUILTIN
  );
  press_to_continue();
}
