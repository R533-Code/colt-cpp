#ifndef HG_COLT_SUBPROCESS
#define HG_COLT_SUBPROCESS

#include <exception> // for std::terminate
#include <colt/dsa/option.h>
#include <colt/io/file.h>
#include <subprocess.h/subprocess.h>

namespace clt
{
  /// @brief Represents a subprocess
  class Subprocess
  {
    /// @brief The process structure
    subprocess_s process_;
    /// @brief If zero the process was not joined
    u8 joined = 0;

    /// @brief Constructor
    /// @param to_copy The process structure
    Subprocess(const subprocess_s& to_copy) noexcept
        : process_(to_copy)
    {
    }

    template<typename>
    friend class Option; // for in place construction

  public:
    /// @brief Move constructor
    /// @param sub The value to move
    Subprocess(Subprocess&& sub)
        : process_(sub.process_)
        , joined(std::exchange(sub.joined, 4))
    {
    }
    /// @brief Move assignment operator
    /// @param sub The value to move
    /// @return This
    Subprocess& operator=(Subprocess&& sub) noexcept
    {
      subprocess_s temp = sub.process_;
      sub.process_      = process_;
      process_          = temp;
      std::swap(sub.joined, joined);
      return *this;
    }
    Subprocess(const Subprocess&) = delete;

    /// @brief The subprocess options
    enum class SubprocessOption : u8
    {
      /// @brief Nothing
      NO_OPTIONS = 0,
      /// @brief stdout and stderr of the subprocess are the same file
      COMBINE_STDOUT_STDERR = subprocess_option_combined_stdout_stderr,
      /// @brief The child process should inherit the environment variables of the parent.
      INHERIT_ENV = subprocess_option_inherit_environment,
      /// @brief Enable asynchronous reading of stdout/stderr before it has completed.
      ENABLE_ASYNC = subprocess_option_enable_async,
      /// @brief Enable the child process to be spawned with no window visible if supported
      /// by the platform.
      NO_WINDOW = subprocess_option_no_window,
      /// @brief Search for program names in the PATH variable. Always enabled on Windows.
      /// Note: this will NOT search for paths in any provided custom environment
      /// and instead uses the PATH of the spawning process.
      SEARCH_USER_PATH = subprocess_option_search_user_path,
    };
    using enum SubprocessOption;

    friend SubprocessOption operator|(SubprocessOption a, SubprocessOption b)
    {
      return static_cast<SubprocessOption>(
          static_cast<int>(a) | static_cast<int>(b));
    }

    /// @brief Check if the subprocess is alive
    /// @return True if alive
    [[nodiscard]] COLTCPP_EXPORT bool is_alive() const noexcept;

    /// @brief Return the stdin handle of the subprocess
    /// @return The stdin handle of the process
    [[nodiscard]] COLTCPP_EXPORT File get_stdin() const noexcept;

    /// @brief Return the stdout handle of the subprocess
    /// @return The stdout handle of the process
    [[nodiscard]] COLTCPP_EXPORT File get_stdout() const noexcept;

    /// @brief Return the stderr handle of the subprocess
    /// @return The stderr handle of the process
    [[nodiscard]]
    COLTCPP_EXPORT File get_stderr() const noexcept;

    /// @brief Waits for the subprocess to finish, and returns its error code.
    /// Joining will close the stdin handle.
    /// This function must only be called once and if neither terminate
    /// nor detach were called.
    /// @return None on errors, else the error code of the subprocess
    [[nodiscard]]
    COLTCPP_EXPORT Option<int> join() noexcept;

    /// @brief Check if the subprocess is joinable.
    /// @return !detached, !terminated, !join
    [[nodiscard]] bool joinable() const noexcept { return joined == 0; }

    /// @brief Detaches the subprocess from the current process.
    /// The subprocess can thereafter outlive the current process.
    /// This function must only be called once and if neither terminate
    /// nor join were called.
    /// @return Flag representing the success
    [[nodiscard]]
    COLTCPP_EXPORT ErrorFlag detach() noexcept;

    /// @brief Terminates the subprocess
    /// This function must only be called once and if neither detach
    /// nor join were called.
    /// @return Flag representing the success
    [[nodiscard]]
    COLTCPP_EXPORT ErrorFlag terminate() noexcept;

    /// @brief The subprocess must be joined/detach/terminated before
    /// the destructor is run
    ~Subprocess() noexcept
    {
      assert_true(
          "Subprocess was not joined or terminated before destructor!", !joinable());
      if (joinable())
      {
        terminate().discard();
        std::terminate();
      }
    }

    /// @brief Empty environment
    static constexpr std::array<const char*, 1> EMPTY_ENV = {nullptr};

    /// @brief Starts a new subprocess.
    /// @param command_line The array of commands (starting with the executable, and ending with a nullptr)
    /// @param opt The options for opening the subprocess
    /// @param env The env (of the form A=B, and ending with a nullptr)
    /// @return None on errors, else the opened subprocess
    [[nodiscard]]
    COLTCPP_EXPORT static Option<Subprocess> open(
        View<const char*> command_line, SubprocessOption opt = NO_OPTIONS,
        View<const char*> env = EMPTY_ENV) noexcept;
  };
} // namespace clt

#endif // !HG_COLT_SUBPROCESS
