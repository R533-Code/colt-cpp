#define _CRT_SECURE_NO_WARNINGS
#include "util/benchmark.h"
#include "io/print.h"
#include "meta/type_list.h"
#include "structs/string.h"
#include "structs/vector.h"
#include "structs/expect.h"
#include "structs/option.h"
#include "str/distance.h"
#include "refl/refl.h"
#include "io/input.h"
#include "cmd/parse_args.h"

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

int main(int argc, char** argv)
{
  using CMDs = meta::type_list<
    cl::Opt<"o", cl::value_desc<"filename">, cl::desc<"Specify output filename">, cl::location<file_out>>,
    cl::Opt<"str", cl::value_desc<"str">, cl::desc<"Specify string to compare against">, cl::location<cmp_strv>>
  >;
  cl::parse_command_line_options<CMDs>(argc, argv);

  for (;;)
  {
    auto result1 = input("Enter your name : ");
    if (result1.is_error())
      break;
    StringView strv = *result1;  
    print("Your name is {}!\nRequired transformation to \"{}\": {}",
      strv, cmp_strv, str::levenshtein_distance(cmp_strv, strv));
  }
}