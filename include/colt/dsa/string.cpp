/*****************************************************************/ /**
 * @file   string.cpp
 * @brief  Contains the implementation of `maybe_in_const_segment`.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "string.h"

#if defined(COLT_LINUX)
extern char etext, edata, end; // The symbols must have some type to avoid warnings
#elif defined(COLT_WINDOWS)
  #include <Windows.h>
  #include <Psapi.h>
#endif

bool clt::maybe_in_const_segment(const void* ptr) noexcept
{
  // Credits Morgan McGuire, Zander Majercik
  // https://www.youtube.com/watch?v=fglXeSWGVDc
#if defined(COLT_LINUX)
  return (ptr < &edata) && (ptr > &etext);
#elif defined(COLT_WINDOWS)
  // Initialization will be performed once (Meyer's Singleton)
  static MODULEINFO info;
  static const bool BOOL = GetModuleInformation(
      GetCurrentProcess(), GetModuleHandleA(nullptr), &info, sizeof(MODULEINFO));
  // To avoid unused warning
  (void)BOOL;
  return uintptr_t(info.EntryPoint) < uintptr_t(ptr)
         && uintptr_t(ptr)
                < uintptr_t(info.EntryPoint) + uintptr_t(info.SizeOfImage);
#else
  static const char* PROBE        = "___Colt_Internal_Str___";
  static const auto PROBE_ADDRESS = uintptr_t(PROBE);
  return math::abs(i64(uintptr_t(ptr) - PROBE_ADDRESS)) < 5'242'880;
#endif
}