#ifndef HG_COLT_FILE
#define HG_COLT_FILE

#include <colt/typedefs.h>
#include <colt/dsa/option.h>
#include <chrono>

namespace clt
{
  /// @brief Represents a file handle
  class File
  {
  public:
    /// @brief The file access when opening
    enum class FileAccess : u8
    {
      /// @brief "r", fail if !exist, read from start
      Read,
      /// @brief "w", overwrite if exist, create if !exist, write from start
      Write,
      /// @brief "a", appends content if exist, create if !exist, write to end
      Append,
      /// @brief "wx", fail if exist, create if !exist, write from start
      Create,
    };
    using enum FileAccess;

  private:
    /// @brief The file number
    int handle;
    /// @brief The file access
    FileAccess access;

    /// @brief Constructor
    /// @param handle The file handle or -1 for errors
    File(int handle, FileAccess access) noexcept
        : handle(handle)
        , access(access)
    {
    }

  public:
    /// @brief Returns the file number
    /// @return The file number or -1 if not open
    [[nodiscard]] int fileno() const noexcept { return handle; }
    /// @brief Check if the current file is opened
    /// @return True if opened
    [[nodiscard]] bool is_open() const noexcept { return handle != -1; }

    /// @brief Closes the file descriptor
    void close() noexcept;

    /// @brief Check if the current file is stdin
    /// @return True if stdin
    [[nodiscard]] bool is_stdin() const noexcept { return handle == 0; }
    /// @brief Check if the current file is stdout
    /// @return True if stdout
    [[nodiscard]] bool is_stdout() const noexcept { return handle == 1; }
    /// @brief Check if the current file is stderr
    /// @return True if stderr
    [[nodiscard]] bool is_stderr() const noexcept { return handle == 2; }    

    /// @brief Returns the file access with which a file was opened.
    /// @return The file access
    [[nodiscard]] FileAccess file_access() const noexcept { return access; }

    /// @brief True if the current file is associated with a terminal.
    /// As an example if stdout is not redirected, it is associated with
    /// a terminal. This is the same as `isatty`.
    /// @return True if associated with a terminal
    [[nodiscard]] COLTCPP_EXPORT bool is_terminal() const noexcept;

    /// @brief Returns the creation time of the current file.
    /// @return If not open or the operation fail returns None, else the creation time
    [[nodiscard]] COLTCPP_EXPORT Option<std::chrono::file_clock::time_point> creation_time()
        const noexcept;
    /// @brief Returns the last access time of the current file.
    /// @return If not open or the operation fail returns None, else the last access time
    [[nodiscard]] COLTCPP_EXPORT Option<std::chrono::file_clock::time_point> access_time()
        const noexcept;
    /// /// @brief Returns the last write time of the current file.
    /// @return If not open or the operation fail returns None, else the last write time
    [[nodiscard]] COLTCPP_EXPORT Option<std::chrono::file_clock::time_point> write_time()
        const noexcept;

    /// @brief Returns the file size in bytes
    /// @return The file size in bytes or None if not open or on errors
    [[nodiscard]] COLTCPP_EXPORT Option<bytes> file_size() const noexcept;

    /// @brief Reads a single byte from the file.
    /// If the file is not opened as read, returns None.
    /// If the file is not opened, returns None.
    /// @return None on errors or EOF else the read value
    [[nodiscard]] COLTCPP_EXPORT Option<u8> read() noexcept;

    /// @brief Writes a single byte to a file.
    /// If the file is opened as read, returns error().
    /// If the file is not opened, returns error().
    /// @param out The byte to write
    /// @return success() if the byte was written
    [[nodiscard]] COLTCPP_EXPORT ErrorFlag write(u8 out) noexcept;

    /// @brief Reads multiple bytes from a file
    /// @param out Where to write the read bytes
    /// @return None on errors or the number of bytes read
    [[nodiscard]] COLTCPP_EXPORT Option<size_t> read(Span<u8> out) noexcept;

    /// @brief Writes multiple bytes to a file.
    /// If the file is opened as read, returns None.
    /// If the file is not opened, returns None.
    /// @param out The bytes to write
    /// @return None on errors or the number of bytes written
    [[nodiscard]] COLTCPP_EXPORT Option<size_t> write(View<u8> out) noexcept;

    /// @brief Check if the current file has hit the EOF.
    /// If the file is not open, returns false.
    /// @return True if the end of file was hit
    [[nodiscard]] COLTCPP_EXPORT bool is_eof() const noexcept;

    /// @brief Creates a file
    /// @param path The path of the file
    /// @param access The access mode of the file
    /// @param text_mode True if text mode false if binary (only affects Windows)
    /// @return None on errors else opened handle
    [[nodiscard]] COLTCPP_EXPORT static Option<File> open(
        const char* const path, FileAccess access, bool text_mode = false) noexcept;

    /// @brief Returns the stdin handle
    /// @return stdin handle
    [[nodiscard]] COLTCPP_EXPORT static File& get_stdin() noexcept;
    /// @brief Returns the stdout handle
    /// @return stdout handle
    [[nodiscard]] COLTCPP_EXPORT static File& get_stdout() noexcept;
    /// @brief Returns the stderr handle
    /// @return stderr handle
    [[nodiscard]] COLTCPP_EXPORT static File& get_stderr() noexcept;
    /// @brief Returns the null device handle.
    /// All writes to this file are discarded.
    /// @return null device handle handle
    [[nodiscard]] COLTCPP_EXPORT static File& get_null_device() noexcept;
  };
} // namespace clt

#endif // !HG_COLT_FILE
