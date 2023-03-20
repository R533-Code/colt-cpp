#define _CRT_SECURE_NO_WARNINGS
#include "util/benchmark.h"
#include "io/print.h"

using namespace std::chrono_literals;
using namespace clt;
using namespace clt::io;

int main(int argc, int argv)
{
  std::atexit([]() { clt::bench::save_tracing_to("Test.json"); });

  {
    COLT_PROFILE_SCOPE("fmt::print");
    print_message("Hello {}", "World!");
  }
}