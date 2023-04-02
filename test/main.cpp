#define _CRT_SECURE_NO_WARNINGS
#include "util/benchmark.h"
#include "io/print.h"
#include "io/input.h"
#include "structs/vector.h"
#include "structs/expect.h"
#include "structs/option.h"
#include "refl/enum.h"

using namespace std::chrono_literals;
using namespace clt;
using namespace clt::io;
using namespace clt::mem;

DECLARE_ENUM_WITH_TYPE(clt::u8, OS, Windows, Linux, MacOS);

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

int main(int argc, int argv)
{
  std::atexit([]() { clt::bench::save_tracing_to("Test.json"); });
  
  for (auto en : refl<OS>::iter())
    print("{}", en);

  {
    COLT_PROFILE_SCOPE("fmt::print");
    print_message("Hello {}", "World!");
  }

  auto vec = Vector<int>(10ULL);
  vec.push_back(10);
  vec.push_back(13);
  vec.push_back(15);
  vec.pop_back_n(2);
  print("{}, {}", div_expect(10, 2), div_expect(19, 0));
  print("{:test}, {:test}", div_option(10, 2), div_option(19, 0));
}