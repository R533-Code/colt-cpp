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

bool WaitForUserInput = true;

using CMDs = meta::type_list<
    cl::Opt<"nocolor", cl::desc<"Turns off colored output">,// cl::value_desc<"Waw!">,
      cl::callback<[]() { clt::io::OutputColor = false; }>>,
    
    cl::Opt<"nowait", cl::desc<"Do not wait for user input">,
      cl::callback<[]() { WaitForUserInput = false; }>>,

    cl::Opt<"v", cl::desc<"Prints the version of the compiler">, cl::alias<"ve">,
      cl::callback<[]() { io::print("COLT"); std::exit(0); }>>
  >;

int main(int argc, char** argv)
{
  clt::install_colt_handlers();
  cl::parse_command_line_options<CMDs>(argc, argv);
  
  clt::Map<u64, StringView> map;
  map.insert(0, "Zero");
  map.insert(1, "One");
  map.insert(2, "Two");
  map.insert(3, "Three");
  map.erase(2);

  io::print_message("Map: {}", map);

  while (true)
  {
    auto Err = io::input<FlatList<u32>>("Enter a list: ");
    if (Err.is_error())
    {
      if (Err.error() == ParsingCode::FILE_EOF)
        break;
      io::print_error("{}.", Err.error());
      continue;
    }

    io::print_message("The list is: {}\n{}, {}",
      *Err,
      AB{ 10, 10, D{ 1.2} },
      refl::EntityKind::IS_BUILTIN
    );
  }

  press_to_continue();
}
