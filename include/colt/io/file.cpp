#include "file.h"

#ifdef COLT_WINDOWS
  #include <io.h>
  #include <fcntl.h>
  #define NOMINMAX
  #include <Windows.h>

namespace clt
{
  void File::close() noexcept
  {
    _close(handle);
    handle = -1;
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
    static auto FILE = File{to_fileno(stdin), FileAccess::Read};
    return FILE;
  }
  File& File::get_stdout() noexcept
  {
    static auto FILE = File{to_fileno(stdout), FileAccess::Write};
    return FILE;
  }
  File& File::get_stderr() noexcept
  {
    static auto FILE = File{to_fileno(stderr), FileAccess::Write};
    return FILE;
  }

  File& File::get_null_device() noexcept
  {
    // TODO: checks
    static auto FILE = *File::open("nul", FileAccess::Write);
    return FILE;
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

  ErrorFlag File::flush() noexcept
  {
    if (!is_open() || _commit(this->fileno()) == -1)
      return ErrorFlag::error();
    return ErrorFlag::success();
  }

  Option<u8> File::read() noexcept
  {
    if (!is_open() || access != FileAccess::Read)
      return None;
    if (u8 ret; _read(this->fileno(), &ret, 1) == 1)
      return Option<u8>(ret);
    return None;
  }

  Option<size_t> File::write(u8 out) noexcept
  {
    if (!is_open() || access == FileAccess::Read)
      return None;
    if (_write(this->fileno(), &out, 1) == 1)
      return 1;
    return None;
  }

  Option<size_t> File::read(Span<u8> out) noexcept
  {
    if (!is_open() || access != FileAccess::Read)
      return None;
    // TODO: overflow check
    auto read = _read(this->fileno(), out.data(), (unsigned int)out.size_bytes());
    if (read < 0)
      return None;
    return Option<size_t>((size_t)read);
  }

  Option<size_t> File::write(View<u8> out) noexcept
  {
    if (!is_open() || access == FileAccess::Read)
      return None;
    // TODO: overflow check
    auto write = _write(this->fileno(), &out, (unsigned int)out.size_bytes());
    if (write < 0)
      return None;
    return Option<size_t>((size_t)write);
  }

  bool File::is_eof() const noexcept
  {
    if (!is_open())
      return false;
    return _eof(this->fileno());
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
    return Option<File>(File(handle, access));
  }

} // namespace clt

#endif // COLT_WINDOWS
