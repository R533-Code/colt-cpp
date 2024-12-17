#include "file.h"

#ifdef COLT_WINDOWS
  #include <io.h>
  #include <fcntl.h>
  #define NOMINMAX
  #include <Windows.h>

namespace clt
{
  int File::fileno() const noexcept
  {
    return handle;
  }

  void File::close() noexcept
  {
    _close(handle);
    handle = -1;
  }

  bool File::is_open() const noexcept
  {
    return handle != -1;
  }

  static int to_fileno(FILE* file)
  {
    int value = _fileno(file);
    if (value == -2)
      return -1;
    return value;
  }

  File& File::get_stdin() noexcept
  {
    static auto FILE = File{to_fileno(stdin)};
    return FILE;
  }
  File& File::get_stdout() noexcept
  {
    static auto FILE = File{to_fileno(stdout)};
    return FILE;
  }
  File& File::get_stderr() noexcept
  {
    static auto FILE = File{to_fileno(stderr)};
    return FILE;
  }

  bool File::is_stdout() const noexcept
  {
    return false;
  }

  bool File::is_stderr() const noexcept
  {
    return false;
  }

  bool File::is_stdin() const noexcept
  {
    return false;
  }
  
  bool File::is_terminal() const noexcept
  {
    return is_open() && _isatty(this->fileno());
  }

  Option<std::chrono::file_clock::time_point> File::creation_time() const noexcept
  {
    using namespace std::chrono;

    FILETIME ret;
    if (!is_open()
        || GetFileTime(
               (HANDLE)_get_osfhandle(this->fileno()), &ret, nullptr, nullptr)
               == 0)
      return None;
    file_clock::duration d{
        (static_cast<int64_t>(ret.dwHighDateTime) << 32) | ret.dwLowDateTime};
    file_clock::time_point tp{d};
    return Option<file_clock::time_point>(tp);
  }

  Option<std::chrono::file_clock::time_point> File::access_time() const noexcept
  {
    using namespace std::chrono;

    FILETIME ret;
    if (!is_open()
        || GetFileTime(
               (HANDLE)_get_osfhandle(this->fileno()), nullptr, &ret, nullptr)
               == 0)
      return None;
    file_clock::duration d{
        (static_cast<int64_t>(ret.dwHighDateTime) << 32) | ret.dwLowDateTime};
    file_clock::time_point tp{d};
    return Option<file_clock::time_point>(tp);
  }

  Option<std::chrono::file_clock::time_point> File::write_time() const noexcept
  {
    using namespace std::chrono;

    FILETIME ret;
    if (!is_open()
        || GetFileTime(
               (HANDLE)_get_osfhandle(this->fileno()), nullptr, nullptr, &ret)
               == 0)
      return None;
    file_clock::duration d{
        (static_cast<int64_t>(ret.dwHighDateTime) << 32) | ret.dwLowDateTime};
    file_clock::time_point tp{d};
    return Option<file_clock::time_point>(tp);
  }

  Option<bytes> File::file_size() const noexcept
  {
    if (!is_open())
      return None;
    return Option<bytes>(bytes{(size_t)_filelengthi64(this->fileno())});
  }

  static auto convert_access(File::FileAccess access, bool text_mode) noexcept
  {

    auto ret = text_mode ? _O_TEXT : _O_BINARY;
    switch_no_default (access)
    {
    case clt::File::FileAccess::Read:
      return ret | _O_RDONLY;
    case clt::File::FileAccess::Write:
      return ret | _O_WRONLY | O_CREAT;
    case clt::File::FileAccess::Append:
      return ret | _O_WRONLY | O_APPEND | O_CREAT;
    case clt::File::FileAccess::Create:
      return ret | _O_WRONLY | _O_EXCL | _O_CREAT;
    }
  }

  Option<File> File::open(
      const char* const path, FileAccess access, bool text_mode) noexcept
  {
    auto handle = _open(path, convert_access(access, text_mode));
    if (handle == -1)
      return None;
    return Option<File>(handle);
  }

} // namespace clt

#endif // COLT_WINDOWS
