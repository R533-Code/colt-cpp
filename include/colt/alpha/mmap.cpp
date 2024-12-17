#include "mmap.h"
#include <Windows.h>

namespace clt
{
#ifdef COLT_WINDOWS

  static auto convert_access(VirtualPage::PageAccess access) noexcept
  {
    switch_no_default(access)
    {
    case clt::VirtualPage::PageAccess::None:
      return PAGE_NOACCESS;
    case clt::VirtualPage::PageAccess::ReadExecute:
      return PAGE_EXECUTE_READ;
    case clt::VirtualPage::PageAccess::WriteExecute:
      return PAGE_EXECUTE_READWRITE;
    case clt::VirtualPage::PageAccess::ReadWrite:
      return PAGE_READWRITE;
    case clt::VirtualPage::PageAccess::ReadOnly:
      return PAGE_READONLY
    }
  }

  VirtualPage VirtualPage::allocate(bytes byte, PageAccess access, void* hint) noexcept
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

#endif // COLT_WINDOWS
} // namespace clt
