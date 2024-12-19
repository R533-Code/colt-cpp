#include "mmap.h"

#ifdef COLT_WINDOWS
  #define NOMINMAX
  #define WIN32_LEAN_AND_MEAN
  #include <Windows.h>
#elif defined(COLT_LINUX) || defined(COLT_APPLE)
  #include <sys/mman.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
#endif // COLT_WINDOWS

#ifdef COLT_WINDOWS

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

  void VirtualPage::flush_icache(const void* start, size_t offset) noexcept
  {
    assert_true(
        "If start is not null, offset must not be 0!",
        implies(start != nullptr, offset != 0));
    FlushInstructionCache(GetCurrentProcess(), start, offset);
    // use __builtin___clear_cache() for linux
  }

} // namespace clt

#else // !COLT_WINDOWS

#endif // COLT_WINDOWS

#include "mmap_file.h"

namespace clt
{
#ifdef COLT_WINDOWS
  bool ViewOfFile::is_closed() const noexcept
  {
    return file_handle == nullptr && mapping_handle == nullptr
           && view_map == nullptr;
  }

  void ViewOfFile::close()
  {
    if (view_map != nullptr)
    {
      UnmapViewOfFile(view_map);
      view_map = nullptr;
    }
    if (mapping_handle != nullptr)
    {
      CloseHandle((HANDLE)mapping_handle);
      mapping_handle = nullptr;
    }
    CloseHandle((HANDLE)file_handle);
    file_handle = nullptr;
  }

  Option<View<u8>> ViewOfFile::view() const noexcept
  {
    if (file_handle == nullptr)
      return View<u8>{(const u8*)nullptr, (size_t)0};
    LARGE_INTEGER value;
    if (GetFileSizeEx(file_handle, &value) == 0)
      return None;
    return View<u8>{(const u8*)view_map, (size_t)value.QuadPart};
  }

  Option<ViewOfFile> ViewOfFile::open(const char* ptr)
  {
    auto handle = CreateFile(
        ptr, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, nullptr);
    if (handle == INVALID_HANDLE_VALUE)
      return None;
    LARGE_INTEGER file_size;
    if (GetFileSizeEx(handle, &file_size) && file_size.QuadPart == 0)
      return ViewOfFile((void*)handle, nullptr, nullptr);

    auto map = CreateFileMapping(handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (map == nullptr)
    {
      CloseHandle(handle);
      return None;
    }

    auto view = MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
    if (view == nullptr)
    {
      CloseHandle(map);
      CloseHandle(handle);
      return None;
    }
    return ViewOfFile((void*)handle, (void*)map, view);
  }
#else
  bool ViewOfFile::is_closed() const noexcept
  {
    return mmap_handle == nullptr;
  }

  void ViewOfFile::close()
  {
    munmap(mmap_handle, (off_t)file_size);
    mmap_handle = nullptr;
  }

  Option<View<u8>> ViewOfFile::view() const noexcept
  {
    return View<u8>{(const u8*)mmap_handle, file_size * (size_t)(mmap_handle != 0)};
  }

  Option<ViewOfFile> ViewOfFile::open(const char* ptr)
  {
    int fd = ::open(ptr, O_RDWR);
    if (fd == -1)
      return None;

    struct stat sb;
    if (fstat(fd, &sb) != 0)
    {
      ::close(fd);
      return None;
    }
    off_t file_size = sb.st_size;
    void* addr      = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED)
    {
      ::close(fd);
      return None;
    }
    ::close(fd);
    return ViewOfFile{addr, (size_t)file_size};
  }
#endif // COLT_WINDOWS
} // namespace clt
