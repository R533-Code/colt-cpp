/*****************************************************************/ /**
 * @file   test_run.cpp
 * @brief  Contains the `main` function responsible for running Catch2.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include <catch2/catch_session.hpp>
#include <colt/macro/config.h>
#include <colt/bit/detect_simd.h>
#include <colt/io/print.h>
#include <colt/versions.h>

/// @brief Catch2 starting point
/// @param argc The number of arguments
/// @param argv The command line arguments
/// @return 0 if no errors were encountered
int main(int argc, const char** argv)
{
  using namespace clt;
  using namespace clt::io;

  fmt::println(
      stderr,
      "Running tests on {}{} {}{}{} ({}).\nThis is a {}-endian system.\nSupported "
      "SIMD instructions: {}{}{}.\n"
      "simdutf v{}, scn v{}, fmt v{}, uni-algo v{}, coltcpp v{}\n",
      CyanF, COLT_OS_STRING, MagentaF, COLT_ARCH_STRING, Reset, COLT_CONFIG_STRING,
#ifdef COLT_LITTLE_ENDIAN
      "little",
#else
      "big",
#endif // COLT_LITTLE_ENDIAN
      BrightBlueF, bit::detect_supported_architectures(), Reset,
      vers::SimdUtfVersion, vers::ScnVersion, vers::FmtVersion, vers::UniAlgoVersion,
      vers::ColtCppVersion);
  return Catch::Session().run(argc, argv);
}