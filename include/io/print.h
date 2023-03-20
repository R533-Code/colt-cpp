#ifndef HG_COLT_PRINT
#define HG_COLT_PRINT

#include <utility>

#define FMT_HEADER_ONLY
#include "../libraries/fmt/include/fmt/format-inl.h"
#include "../meta/string_literal.h"

namespace clt
{
  template<typename T>
  concept Formatable = fmt::is_formattable<T>::value;

  template<typename... Args>
  using fmt_str = fmt::format_string<Args...>;

  template<meta::StringLiteral endl = "\n", typename... Args> requires (Formatable<Args> && ...)
  constexpr void print(fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    fmt::basic_memory_buffer<char, 4096> buffer;
    fmt::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
    if constexpr (endl.size() != 0)
      fmt::format_to(std::back_inserter(buffer), "{}", fmt::string_view{ endl.value, endl.size() });
    fmt::print("{}", fmt::string_view{ buffer.data(), buffer.size() });
  }

  template<meta::StringLiteral endl = "\n", typename... Args> requires (Formatable<Args> && ...)
  constexpr void print_error(fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    fmt::basic_memory_buffer<char, 4096> buffer;
    fmt::format_to(std::back_inserter(buffer), "Error: ");
    fmt::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
    if constexpr (endl.size() != 0)
      fmt::format_to(std::back_inserter(buffer), "{}", fmt::string_view{ endl.value, endl.size() });
    fmt::print("{}", fmt::string_view{ buffer.data(), buffer.size() });
  }

  template<meta::StringLiteral endl = "\n", typename... Args> requires (Formatable<Args> && ...)
  constexpr void print_warn(fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    fmt::basic_memory_buffer<char, 4096> buffer;
    fmt::format_to(std::back_inserter(buffer), "Warning: ");
    fmt::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
    if constexpr (endl.size() != 0)
      fmt::format_to(std::back_inserter(buffer), "{}", fmt::string_view{ endl.value, endl.size() });
    fmt::print("{}", fmt::string_view{ buffer.data(), buffer.size() });
  }

  template<meta::StringLiteral endl = "\n", typename... Args> requires (Formatable<Args> && ...)
  constexpr void print_message(fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    fmt::basic_memory_buffer<char, 4096> buffer;
    fmt::format_to(std::back_inserter(buffer), "Message: ");
    fmt::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
    if constexpr (endl.size() != 0)
      fmt::format_to(std::back_inserter(buffer), "{}", fmt::string_view{ endl.value, endl.size() });
    fmt::print("{}", fmt::string_view{ buffer.data(), buffer.size() });
  }

  template<meta::StringLiteral endl = "\n", typename... Args> requires (Formatable<Args> && ...)
  constexpr void print_fatal(fmt::format_string<Args...> fmt, Args && ...args)
  {
    fmt::basic_memory_buffer<char, 4096> buffer;
    fmt::format_to(std::back_inserter(buffer), "FATAL: ");
    fmt::format_to(std::back_inserter(buffer), fmt, std::forward<Args>(args)...);
    if constexpr (endl.size() != 0)
      fmt::format_to(std::back_inserter(buffer), "{}", fmt::string_view{ endl.value, endl.size() });
    fmt::print("{}", fmt::string_view{ buffer.data(), buffer.size() });
  }
}

#endif //!HG_COLT_PRINT