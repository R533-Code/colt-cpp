/*****************************************************************//**
 * @file   mmap_file.cpp
 * @brief  Contains the implementation of `mmap_file.h`.
 * As with all OS related helpers, no include is done in the header file
 * to avoid cluttering the global namespace.
 * This does mean storing storing void pointers rather than actual
 * handles in ViewOfFile.
 * 
 * @author RPC
 * @date   September 2024
 *********************************************************************/
#include "mmap_file.h"

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

namespace clt::os
{
#ifdef COLT_WINDOWS
  bool ViewOfFile::is_closed() const noexcept
  {
    return file_handle == nullptr && mapping_handle == nullptr
           && view_map == nullptr;
  }

  void ViewOfFile::close()
  {
    UnmapViewOfFile(view_map);
    CloseHandle((HANDLE)mapping_handle);
    CloseHandle((HANDLE)file_handle);
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
} // namespace clt::os