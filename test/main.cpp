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
    reflect<refl::EntityKind*>::str(),
    refl::EntityKind::IS_BUILTIN
  );
  press_to_continue();
}
