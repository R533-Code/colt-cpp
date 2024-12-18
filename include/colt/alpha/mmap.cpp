#include "mmap.h"

#ifdef COLT_WINDOWS
#include <Windows.h>

namespace clt
{

  static auto convert_access(VirtualPage::PageAccess access) noexcept
  {
    using enum VirtualPage::PageAccess;
    switch_no_default(access)
    {
    case None:
      return PAGE_NOACCESS;
    case ReadExecute:
      return PAGE_EXECUTE_READ;
    case WriteExecute:
      return PAGE_EXECUTE_READWRITE;
    case ReadWrite:
      return PAGE_READWRITE;
    case ReadOnly:
      return PAGE_READONLY;
    }
  }

  VirtualPage VirtualPage::allocate(
      bytes byte, PageAccess access, void* hint) noexcept
  {
    auto ret = VirtualAlloc(
        hint, byte.size, MEM_COMMIT | MEM_RESERVE, convert_access(access));
    if (HEDLEY_UNLIKELY(ret == nullptr))
      return VirtualPage(nullptr, 0);
    return VirtualPage(ret, byte.size);
  }

  void VirtualPage::deallocate(const VirtualPage& page) noexcept
  {
    if (page.begin_ != nullptr)
      VirtualFree(page.begin_, 0, MEM_RELEASE);
  }

  bytes VirtualPage::page_size() noexcept
  {
    // cache the result of GetSystemInfo
    static size_t PAGE_SIZE = []()
    {
      SYSTEM_INFO sysInfo;
      GetSystemInfo(&sysInfo);
      return sysInfo.dwPageSize;
    }();
    return bytes{PAGE_SIZE};
  }

  void VirtualPage::flush_icache() noexcept
  {
    FlushInstructionCache(GetCurrentProcess(), this->ptr(), this->size());
    // use __builtin___clear_cache() for linux
  }

} // namespace clt

#else // !COLT_WINDOWS

#endif // COLT_WINDOWS
