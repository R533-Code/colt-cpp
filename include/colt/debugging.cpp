/*****************************************************************/ /**
 * @file   debugging.cpp
 * @brief  Contains utilities for programmatically adding breakpoints.
 * 
 * @author RPC
 * @date   January 2025
 *********************************************************************/
#include "debugging.h"

namespace clt
{
  void breakpoint() noexcept
  {
#if defined(_MSC_VER)
    __debugbreak();
#elif defined(__clang__)
    __builtin_debugtrap();
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
    __asm__ volatile("int $0x03");
#elif defined(__GNUC__) && defined(__thumb__)
    __asm__ volatile(".inst 0xde01");
#elif defined(__GNUC__) && defined(__arm__) && !defined(__thumb__)
    __asm__ volatile(".inst 0xe7f001f0");
#else
    // ADD OTHER PLATFORMS...
#endif
  }

  void breakpoint_if_debugging() noexcept
  {
    if (clt::is_debugger_present())
      clt::breakpoint();
  }
} // namespace clt

#if defined(_MSC_VER) || defined(__MINGW32__)

extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();

bool clt::is_debugger_present() noexcept
{
  return IsDebuggerPresent() != 0;
}

#else

bool clt::is_debugger_present() noexcept
{
  return false;
}
#endif