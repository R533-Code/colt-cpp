/*****************************************************************//**
 * @file   print.h
 * @brief  Contains utilities to write to stdout (or any file).
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_IO_PRINT
#define HG_IO_PRINT

#include <iterator>
#include <utility>
#include <cstdio>

#include "colt/io/file.h"
#include "colt/meta/string_literal.h"
#include "console_effect.h"

namespace clt
{
  template<typename... Args>
  /// @brief Shorthand for fmt::format_string
  /// @tparam ...Args The types to be formatted to the string
  using fmt_str = fmt::format_string<Args...>;

  /// @brief Prints 'Press any key to continue...' and waits for any key input.
  inline void press_to_continue() noexcept
  {
    std::fputs("Press any key to continue...\n", stdout);
    io::wait_kbhit();
  }

  template<typename OutputIt, typename... Args>
  constexpr auto format_to(OutputIt&& it, fmt_str<Args...> fmt, Args&&... args) -> OutputIt
  {
    return fmt::format_to(std::forward<OutputIt>(it), std::forward<Args>(args)...);
  }
  
  template<typename OutputIt, typename... Args>
  constexpr auto format_to_n(OutputIt&& it, size_t n, fmt_str<Args...> fmt, Args&&... args) -> OutputIt
  {
    return fmt::format_to_n(std::forward<OutputIt>(it), n, std::forward<Args>(args)...);
  }

  template<meta::StringLiteral endl = "\n", typename... Args>
  /// @brief Formats and prints a string to 'file'
  /// @tparam ...Args The types of the arguments to format
  /// @param file The file where to write the output
  /// @param fmt The format string
  /// @param ...args The arguments to format
  constexpr Option<size_t> print(
      File& file, fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    using namespace clt::io;

    fmt::basic_memory_buffer<char, 4096> buffer;
    fmt::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
      
    if constexpr (endl.size() != 0)
    {
      fmt::format_to(
          std::back_inserter(buffer), "{}",
          fmt::string_view{endl.value, endl.size()});
    }
    return file.write({reinterpret_cast<u8*>(buffer.data()), buffer.size()});
  }

  template<meta::StringLiteral endl = "\n", typename... Args>
  /// @brief Formats and prints a string to 'file', prepending "Error: "
  /// @tparam ...Args The types of the arguments to format
  /// @param file The file where to write the output
  /// @param fmt The format string
  /// @param ...args The arguments to format
  constexpr Option<size_t> print_error(
      File& file, fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    using namespace clt::io;

    fmt::basic_memory_buffer<char, 4096> buffer;
    fmt::format_to(std::back_inserter(buffer), "{}Error:{} ", BrightRedF, Reset);
    fmt::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
    if constexpr (endl.size() != 0)
      fmt::format_to(
          std::back_inserter(buffer), "{}",
          fmt::string_view{endl.value, endl.size()});
    return file.write({reinterpret_cast<u8*>(buffer.data()), buffer.size()});
  }

  template<meta::StringLiteral endl = "\n", typename... Args>
  /// @brief Formats and prints a string to 'file', prepending "Warning: "
  /// @tparam ...Args The types of the arguments to format
  /// @param file The file where to write the output
  /// @param fmt The format string
  /// @param ...args The arguments to format
  constexpr Option<size_t> print_warn(
      File& file, fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    using namespace clt::io;

    fmt::basic_memory_buffer<char, 4096> buffer;
    fmt::format_to(
        std::back_inserter(buffer), "{}Warning:{} ", BrightYellowF, Reset);
    fmt::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
    if constexpr (endl.size() != 0)
      fmt::format_to(
          std::back_inserter(buffer), "{}",
          fmt::string_view{endl.value, endl.size()});
    return file.write({reinterpret_cast<u8*>(buffer.data()), buffer.size()});
  }

  template<meta::StringLiteral endl = "\n", typename... Args>
  /// @brief Formats and prints a string to 'file', prepending "Message: "
  /// @tparam ...Args The types of the arguments to format
  /// @param file The file where to write the output
  /// @param fmt The format string
  /// @param ...args The arguments to format
  constexpr Option<size_t> print_message(
      File& file, fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    using namespace clt::io;

    fmt::basic_memory_buffer<char, 4096> buffer;
    fmt::format_to(std::back_inserter(buffer), "{}Message:{} ", BrightBlueF, Reset);
    fmt::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
    if constexpr (endl.size() != 0)
      fmt::format_to(
          std::back_inserter(buffer), "{}",
          fmt::string_view{endl.value, endl.size()});
    return file.write({reinterpret_cast<u8*>(buffer.data()), buffer.size()});
  }

  template<meta::StringLiteral endl = "\n", typename... Args>
  /// @brief Formats and prints a string to 'file', prepending "FATAL: "
  /// @tparam ...Args The types of the arguments to format
  /// @param file The file where to write the output
  /// @param fmt The format string
  /// @param ...args The arguments to format
  constexpr Option<size_t> print_fatal(
      File& file, fmt::format_string<Args...> fmt, Args&&... args)
  {
    fmt::basic_memory_buffer<char, 4096> buffer;
    fmt::format_to(
        std::back_inserter(buffer), "{}FATAL:{} {}", RedB, Reset, BrightRedF);
    fmt::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
    if constexpr (endl.size() != 0)
      fmt::format_to(
          std::back_inserter(buffer), "{}",
          fmt::string_view{endl.value, endl.size()});
    return file.write({reinterpret_cast<u8*>(buffer.data()), buffer.size()});
  }

  template<meta::StringLiteral endl = "\n", typename... Args>
  /// @brief Formats and prints a string to 'stdout'
  /// @tparam ...Args The types of the arguments to format
  /// @param fmt The format string
  /// @param ...args The arguments to format
  constexpr auto print(fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    return print<endl>(File::get_stdout(), fmt, std::forward<Args>(args)...);
  }

  template<meta::StringLiteral endl = "\n", typename... Args>
  /// @brief Formats and prints a string to 'stdout', prepending "Error: "
  /// @tparam ...Args The types of the arguments to format
  /// @param fmt The format string
  /// @param ...args The arguments to format
  constexpr auto print_error(fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    return print_error<endl>(File::get_stdout(), fmt, std::forward<Args>(args)...);
  }

  template<meta::StringLiteral endl = "\n", typename... Args>
  /// @brief Formats and prints a string to 'stdout', prepending "Warning: "
  /// @tparam ...Args The types of the arguments to format
  /// @param fmt The format string
  /// @param ...args The arguments to format
  constexpr auto print_warn(fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    return print_warn<endl>(File::get_stdout(), fmt, std::forward<Args>(args)...);
  }

  template<meta::StringLiteral endl = "\n", typename... Args>
  /// @brief Formats and prints a string to 'stdout', prepending "Message: "
  /// @tparam ...Args The types of the arguments to format
  /// @param fmt The format string
  /// @param ...args The arguments to format
  constexpr auto print_message(fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    return print_message<endl>(File::get_stdout(), fmt, std::forward<Args>(args)...);
  }

  template<meta::StringLiteral endl = "\n", typename... Args>
  /// @brief Formats and prints a string to 'stderr', prepending "FATAL: "
  /// @tparam ...Args The types of the arguments to format
  /// @param fmt The format string
  /// @param ...args The arguments to format
  constexpr auto print_fatal(fmt::format_string<Args...> fmt, Args&&... args)
  {
    return print_fatal<endl>(File::get_stderr(), fmt, std::forward<Args>(args)...);
  }
}

#endif // !HG_IO_PRINT
