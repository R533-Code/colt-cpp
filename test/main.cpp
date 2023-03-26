#define _CRT_SECURE_NO_WARNINGS
#include "util/benchmark.h"
#include "io/print.h"
#include "io/input.h"
#include "mem/composable_alloc.h"

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

  MemCorruptDetector<Mallocator, 1024_B, 0xFD> alloc;
  auto blk = alloc.alloc(16_B);
  alloc.dealloc(blk);
}