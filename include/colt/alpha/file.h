#ifndef HG_COLT_FILE
#define HG_COLT_FILE

#include <colt/typedefs.h>
#include <colt/dsa/option.h>
#include <colt/alpha/units.h>
#include <chrono>

namespace clt
{
  /// @brief Represents a file handle
  class File
  {
    /// @brief The file number
    int handle;

    /// @brief Constructor
    /// @param handle The file handle or -1 for errors
    File(int handle) noexcept
        : handle(handle)
    {
    }
  
  public:
    /// @brief The file access when opening
    enum class FileAccess
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

    /// @brief Returns the file number
    /// @return The file number or -1 if not open
    int fileno() const noexcept;
    /// @brief Check if the current file is opened
    /// @return True if opened
    bool is_open() const noexcept;
    
    /// @brief Closes the file descriptor
    void close() noexcept;

    /// @brief Check if the current file is stdout
    /// @return True if stdout
    bool is_stdout() const noexcept;
    /// @brief Check if the current file is stderr
    /// @return True if stderr
    bool is_stderr() const noexcept;
    /// @brief Check if the current file is stdin
    /// @return True if stdin
    bool is_stdin() const noexcept;

    /// @brief True if the current file is associated with a terminal.
    /// As an example if stdout is not redirected, it is associated with
    /// a terminal. This is the same as `isatty`.
    /// @return True if associated with a terminal
    bool is_terminal() const noexcept;

    /// @brief Returns the creation time of the current file.
    /// @return If not open or the operation fail returns None, else the creation time
    Option<std::chrono::file_clock::time_point> creation_time() const noexcept;
    /// @brief Returns the last access time of the current file.
    /// @return If not open or the operation fail returns None, else the last access time
    Option<std::chrono::file_clock::time_point> access_time() const noexcept;
    /// /// @brief Returns the last write time of the current file.
    /// @return If not open or the operation fail returns None, else the last write time
    Option<std::chrono::file_clock::time_point> write_time() const noexcept;

    /// @brief Returns the file size in bytes
    /// @return The file size in bytes or None if not open or on errors
    Option<bytes> file_size() const noexcept;

    /// @brief Creates a file
    /// @param path The path of the file
    /// @param access The access mode of the file
    /// @param text_mode True if text mode false if binary (only affects Windows)
    /// @return None on errors else opened handle
    static Option<File> open(
        const char* const path, FileAccess access, bool text_mode = false) noexcept;

    /// @brief Returns the stdin handle
    /// @return stdin handle
    static File& get_stdin() noexcept;
    /// @brief Returns the stdout handle
    /// @return stdout handle
    static File& get_stdout() noexcept;
    /// @brief Returns the stderr handle
    /// @return stderr handle
    static File& get_stderr() noexcept;
  };
}

#endif // !HG_COLT_FILE
