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

  while (true)
  {
    auto Err = io::input<byte_size<Byte>>("Enter a byte size: ");
    if (Err.is_error())
    {
      if (Err.error() == ParsingResult::FILE_EOF)
        break;
      io::print_error("{}.", Err.error());
      continue;
    }

    io::print("The byte size is: {}\n{}, {}",
      *Err,
      AB{ 10, 10, D{ 1.2} },
      refl::EntityKind::IS_BUILTIN
    );
  }

  press_to_continue();
}
