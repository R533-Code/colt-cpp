#define _CRT_SECURE_NO_WARNINGS
#include "util/benchmark.h"
#include "io/print.h"
#include "io/input.h"
#include "structs/vector.h"
#include <vector>

using namespace std::chrono_literals;
using namespace clt;
using namespace clt::io;
using namespace clt::mem;

int main(int argc, int argv)
{
  std::atexit([]() { clt::bench::save_tracing_to("Test.json"); });
  
  {
    COLT_PROFILE_SCOPE("fmt::print");
    print_message("Hello {}", "World!");
  }
  auto vec = Vector<int>(10ULL);
  vec.push_back(10);
  vec.push_back(13);
  vec.push_back(15);
  vec.pop_back_n(10);
  print("Vector: {:h}", vec);
}