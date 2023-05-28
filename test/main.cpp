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

  auto Err = io::input<StableSet<u32>>("Enter a set of numbers: ");
  if (Err.is_error())
    std::exit(0);
  StableSet<u32> a = std::move(*Err);
  a.insert(1);
  a.insert(2);
  a.insert(3);

  io::print("The set is: {}\n{}, {}",
    a,
    AB{ 10, 10, D{ 1.2} },
    refl::EntityKind::IS_BUILTIN
  );

  press_to_continue();
}
