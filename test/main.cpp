#include "coltcpp.h"

using namespace std::chrono_literals;
using namespace clt;
using namespace clt::io;
using namespace clt::mem;

Expect<int, const char*> div_expect(int a, int b)
{
  if (b != 0)
    return a / b;
  return { Error, "Division by zero is prohibited!" };
}

Option<int> div_option(int a, int b)
{
  if (b != 0)
    return a / b;
  return { None };
}

StringView file_out = "Default.txt";
StringView cmp_strv = "Raphael";
StringView input_file = "";

using CMDs = meta::type_list<
  cl::Opt<"color", cl::location<io::OutputColor>,
  cl::desc<"Turns colored output on/off.">, cl::value_desc<"true/false">>,

  cl::Opt<"o", cl::location<file_out>,
  cl::desc<"Specify output filename">, cl::value_desc<"filename">>,

  cl::Opt<"str", cl::location<cmp_strv>, cl::alias<"s">,
  cl::value_desc<"str">, cl::desc<"Specify string to compare against">>,

  cl::OptPos<"input_file2", cl::location<input_file>>
  >;

int main(int argc, char** argv)
{
  clt::install_colt_handlers();
  cl::parse_command_line_options<CMDs>(argc, argv,
    "test", "Helper to test Colt utilities!");
  
  io::print("{} {} {} {}",
    GetHash(Vector<u64>{}), GetHash(StaticVector<u64, 4>{}), GetHash(UniquePtr<u64>{}), GetHash(StringView{ "a" }));

  for (;;)
  {
    auto result1 = input("Enter your name: ");
    if (result1.is_error())
    {
      print_warn("EOF detected!");
      break;
    }
    StringView strv = *result1;
    print_message("Your name is {}!\nRequired transformation to \"{}\": {}",
      strv, cmp_strv, str::levenshtein_distance(cmp_strv, strv));
  }
  press_to_continue();
}
