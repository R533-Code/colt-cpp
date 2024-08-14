#include <catch2/catch_session.hpp>
#include <colt/macro/config.h>
#include <colt/bit/detect_simd.h>
#include <colt/io/print.h>

int main(int argc, char* argv[])
{
  using namespace clt;
  using namespace clt::io;  
  
  auto result = Catch::Session().run(argc, argv);
  fmt::println(stderr,
      "Running tests on {}{} {}{}{} ({}).\nThis is a {}-endian system.\nSupported "
      "SIMD instructions: {}{}{}.\n",
      CyanF, COLT_OS_STRING, MagentaF, COLT_ARCH_STRING, Reset, COLT_CONFIG_STRING,
#ifdef COLT_LITTLE_ENDIAN
      "little",
#else
      "big",
#endif // COLT_LITTLE_ENDIAN
      BrightBlueF, bit::detect_supported_architectures(), Reset);
  return result;
}