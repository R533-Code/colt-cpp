#define _CRT_SECURE_NO_WARNINGS
#include "util/benchmark.h"

using namespace std::chrono_literals;
using namespace clt;

int main(int argc, int argv)
{
  std::atexit([]() { clt::bench::save_tracing_to("Test.json"); });

  BYTE a = { 10 };
  a.chg_n(8, true);
}