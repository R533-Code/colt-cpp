/*****************************************************************/ /**
 * @file   dynamic_lib.cpp
 * @brief  Contains the implementation of `dynamic_lib.h`.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "dynamic_lib.h"

#ifdef COLT_WINDOWS
  #define NOMINMAX
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>
#elif defined(COLT_LINUX) || defined(COLT_APPLE)
  #include <dlfcn.h>
#endif // COLT_WINDOWS

namespace clt::os
{
#ifdef COLT_WINDOWS
  void DynamicLib::close()
  {
    FreeLibrary((HMODULE)_handle);
  }

  void* DynamicLib::find_symbol(const char* name) const
  {
    if (_handle == nullptr)
      return nullptr;
    return GetProcAddress((HMODULE)_handle, name);
  }

  Option<DynamicLib> DynamicLib::open() noexcept
  {
    auto ptr = GetModuleHandle(nullptr);
    if (ptr)
      return DynamicLib((void*)ptr);
    return None;
  }

  Option<DynamicLib> DynamicLib::open(const char* path) noexcept
  {
    assert_true("Path must not be NUL!", path != nullptr);
    auto ptr = LoadLibrary(path);
    if (ptr)
      return DynamicLib((void*)ptr);
    return None;
  }
#else
  void DynamicLib::close()
  {
    dlclose(_handle);
  }

  void* DynamicLib::find_symbol(const char* name) const
  {
    if (_handle == nullptr)
      return nullptr;
    return dlsym(_handle, name);
  }

  Option<DynamicLib> DynamicLib::open() noexcept
  {
    auto ptr = dlopen(nullptr, RTLD_LAZY);
    if (ptr)
      return DynamicLib((void*)ptr);
    return None;
  }

  Option<DynamicLib> DynamicLib::open(const char* path) noexcept
  {
    assert_true("Path must not be null!", path != nullptr);
    auto ptr = dlopen(path, RTLD_LAZY);
    if (ptr)
      return DynamicLib((void*)ptr);
    return None;
  }
#endif // COLT_WINDOWS
} // namespace clt::os
