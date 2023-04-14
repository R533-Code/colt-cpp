#define _CRT_SECURE_NO_WARNINGS
#include "util/params.h"
#include "util/benchmark.h"
#include "io/print.h"
#include "meta/type_list.h"
#include "structs/string.h"
#include "structs/vector.h"
#include "structs/expect.h"
#include "structs/option.h"
#include "refl/refl.h"
#include "io/input.h"

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

struct Test
{
  u64 a;
  u64 b;
};
COLT_DECLARE_TYPE(Test, a, b);

struct AB
{
  Test a;
  u64 b;
  Test c;
};
COLT_DECLARE_TYPE(AB, a, b, c);

void init_AB(out<AB> param)
{
  param.construct(AB{ {20, 10 }, 10, { 30, 20} });
}

int main(int argc, char** argv)
{
  std::atexit([]() { clt::bench::save_tracing_to("Test.json"); });

  auto result = input<u64>("Enter your age: ");
  while (result.is_error())
  {
    if (result.error() == IOError::FILE_EOF)
    {
      print_fatal("EOF detected!");
      std::exit(1);
    }
    result = input<u64>("Invalid value ({})! Please enter a valid number: ",
      refl<IOError>::str(result.error()));
  }
  print("Your age is {}!", *result);
}