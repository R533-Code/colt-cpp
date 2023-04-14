#define _CRT_SECURE_NO_WARNINGS
#include "util/params.h"
#include "util/benchmark.h"
#include "io/print.h"
#include "meta/type_list.h"
#include "structs/vector.h"
#include "structs/expect.h"
#include "structs/option.h"
#include "refl/refl.h"

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

  uninit<AB> ab;
  init_AB(ab);
  {
    COLT_PROFILE_SCOPE("fmt::print");
    print_message("Hello: {}\n{}", clt::refl<const u8* const>::str(),
      ab.data());
  }
  auto vec = Vector<int>(10ULL);
  vec.push_back(10);
  vec.push_back(13);
  vec.push_back(15);
  print("Vector: {:h}", vec);
  print("Option test: {:empty option}", div_option(10, 0));
  print("Expect test: {}", div_expect(10, 0));
}