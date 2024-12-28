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

  Option<File> File::from(FILE* from, FileAccess access) noexcept
  {
    if (from == nullptr)
      return None;
    auto val = to_fileno(from);
    if (val == -1)
      return None;
    return File{val, access};
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
    static auto FILE =
        File::open("nul", FileAccess::Write).value_or(File(-1, FileAccess::Write));
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
    auto write = _write(this->fileno(), out.data(), (unsigned int)out.size_bytes());
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
    switch_no_default(access)
    {
    case clt::File::FileAccess::Read:
      return ret | _O_RDONLY;
    case clt::File::FileAccess::Write:
      return ret | _O_WRONLY | _O_CREAT;
    case clt::File::FileAccess::Append:
      return ret | _O_WRONLY | _O_APPEND | _O_CREAT;
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

#else // linux

  #include <sys/stat.h>
  #include <unistd.h>
  #include <fcntl.h>

namespace clt
{
  void File::close() noexcept
  {
    ::close(handle);
    handle = -1;
  }

  File& File::get_stdin() noexcept
  {
    static auto FILE = File{0, FileAccess::Read};
    return FILE;
  }
  File& File::get_stdout() noexcept
  {
    static auto FILE = File{1, FileAccess::Write};
    return FILE;
  }
  File& File::get_stderr() noexcept
  {
    static auto FILE = File{2, FileAccess::Write};
    return FILE;
  }

  File& File::get_null_device() noexcept
  {
    static auto FILE = File::open("/dev/null", FileAccess::Write)
                           .value_or(File(-1, FileAccess::Write));
    return FILE;
  }

  bool File::is_terminal() const noexcept
  {
    return is_open() && isatty(this->fileno());
  }

  Option<std::chrono::file_clock::time_point> File::creation_time() const noexcept
  {
    using namespace std::chrono;

    struct statx stx;
    if (statx(this->fileno(), "", AT_EMPTY_PATH, STATX_BTIME, &stx) != 0)
      return None;

    if (!(stx.stx_mask & STATX_BTIME))
      return None;

    auto system_time = system_clock::time_point{
        seconds(stx.stx_btime.tv_sec) + nanoseconds(stx.stx_btime.tv_nsec)};
    return clock_cast<file_clock>(system_time);
  }

  Option<std::chrono::file_clock::time_point> File::access_time() const noexcept
  {
    using namespace std::chrono;

    struct statx stx;
    if (statx(this->fileno(), "", AT_EMPTY_PATH, STATX_ATIME, &stx) != 0)
      return None;

    if (!(stx.stx_mask & STATX_ATIME))
      return None;

    auto system_time = system_clock::time_point{
        seconds(stx.stx_atime.tv_sec) + nanoseconds(stx.stx_atime.tv_nsec)};
    return clock_cast<file_clock>(system_time);
  }

  Option<std::chrono::file_clock::time_point> File::write_time() const noexcept
  {
    using namespace std::chrono;

    struct statx stx;
    if (statx(this->fileno(), "", AT_EMPTY_PATH, STATX_MTIME, &stx) != 0)
      return None;

    if (!(stx.stx_mask & STATX_MTIME))
      return None;

    auto system_time = system_clock::time_point{
        seconds(stx.stx_mtime.tv_sec) + nanoseconds(stx.stx_mtime.tv_nsec)};
    return clock_cast<file_clock>(system_time);
  }

  Option<bytes> File::file_size() const noexcept
  {
    if (!is_open())
      return None;
    struct statx stx;
    if (statx(this->fileno(), "", AT_EMPTY_PATH, STATX_SIZE, &stx) != 0)
      return None;
    if (!(stx.stx_mask & STATX_SIZE))
      return None;
    return bytes{stx.stx_size};
  }

  ErrorFlag File::flush() noexcept
  {
    if (!is_open() || ::fsync(this->fileno()) == -1)
      return ErrorFlag::error();
    return ErrorFlag::success();
  }

  Option<u8> File::read() noexcept
  {
    if (!is_open() || access != FileAccess::Read)
      return None;
    if (u8 ret; ::read(this->fileno(), &ret, 1) == 1)
      return Option<u8>(ret);
    return None;
  }

  Option<size_t> File::write(u8 out) noexcept
  {
    if (!is_open() || access == FileAccess::Read)
      return None;
    if (::write(this->fileno(), &out, 1) == 1)
      return 1;
    return None;
  }

  Option<size_t> File::read(Span<u8> out) noexcept
  {
    if (!is_open() || access != FileAccess::Read)
      return None;
    // TODO: overflow check
    auto read = ::read(this->fileno(), out.data(), out.size_bytes());
    if (read < 0)
      return None;
    return Option<size_t>((size_t)read);
  }

  Option<size_t> File::write(View<u8> out) noexcept
  {
    if (!is_open() || access == FileAccess::Read)
      return None;
    // TODO: overflow check
    auto write = ::write(this->fileno(), &out, out.size_bytes());
    if (write < 0)
      return None;
    return Option<size_t>((size_t)write);
  }

  bool File::is_eof() const noexcept
  {
    if (!is_open())
      return false;
    off_t current_pos = ::lseek(this->fileno(), 0, SEEK_CUR);
    if (current_pos == -1)
      return false;

    struct stat file_stat;
    if (fstat(this->fileno(), &file_stat) == -1)
      return false;
    return current_pos >= file_stat.st_size;
  }

  static auto convert_access(File::FileAccess access) noexcept
  {
    switch_no_default(access)
    {
    case clt::File::FileAccess::Read:
      return O_RDONLY;
    case clt::File::FileAccess::Write:
      return O_WRONLY | O_CREAT;
    case clt::File::FileAccess::Append:
      return O_WRONLY | O_APPEND | O_CREAT;
    case clt::File::FileAccess::Create:
      return O_WRONLY | O_EXCL | O_CREAT;
    }
  }
  
  static ::mode_t convert_access_mode(File::FileAccess access) noexcept
  {
    switch_no_default(access)
    {
    case clt::File::FileAccess::Read:
      return S_IRUSR;
    case clt::File::FileAccess::Write:
      return S_IRUSR | S_IWUSR;
    case clt::File::FileAccess::Append:
      return S_IRUSR | S_IWUSR;
    case clt::File::FileAccess::Create:
      return S_IRUSR | S_IWUSR;
    }
  }

  Option<File> File::open(
      const char* const path, FileAccess access, bool) noexcept
  {
    auto handle = ::open(path, convert_access(access), convert_access_mode(access));
    if (handle == -1)
      return None;
    return Option<File>(File(handle, access));
  }

  Option<File> File::from(FILE* from, FileAccess access) noexcept
  {
    if (from == nullptr)
      return None;
    auto val = ::fileno(from);
    if (val == -1)
      return None;
    return File{val, access};
  }
} // namespace clt

#endif // COLT_WINDOWS
