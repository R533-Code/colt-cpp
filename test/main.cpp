#define _CRT_SECURE_NO_WARNINGS
#include "util/benchmark.h"

using namespace std::chrono_literals;

int main(int argc, int argv)
{
  std::atexit([]() { clt::bench::save_tracing_to("Test.json"); });

  COLT_PROFILE_FN();
  std::this_thread::sleep_for(10ms);
  {
    COLT_PROFILE_SCOPE("main scope");
    std::this_thread::sleep_for(15ms);
  }
  std::this_thread::sleep_for(10ms);
}