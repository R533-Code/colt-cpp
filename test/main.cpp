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

int main(int argc, char** argv)
{
  std::atexit([]() { clt::bench::save_tracing_to("Test.json"); });

  for (;;)
  {
    auto result1 = input("Enter your name: ");
    if (result1.is_error())
      break;
    StringView strv = *result1;  
    print("Your name is {}!\nRequired transformation to \"Raphael\": {}",
      strv, str::levenshtein_distance("Raphael", strv));
  }
}