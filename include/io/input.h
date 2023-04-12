#ifndef HG_COLT_INPUT
#define HG_COLT_INPUT

#ifndef _WIN32
  #include <termios.h>
  #include <unistd.h>
#else
  #define NOMINMAX
  #include <Windows.h>
#endif //COLT_WINDOWS

#include <utility>
#include "./print.h"
#include "../util/contracts.h"
#include "../refl/enum.h"
#include "../structs/span.h"
#include "../structs/expect.h"

DECLARE_ENUM_WITH_TYPE(u8, clt::io, IOError, FILE_EOF, FILE_ERROR, INVALID_FMT, RANGE_ERROR);

namespace clt::io
{
  static void toggle_echo() noexcept
  {
#ifdef _WIN32
    HANDLE h;
    DWORD mode;

    h = GetStdHandle(STD_INPUT_HANDLE);
    if (GetConsoleMode(h, &mode))
    {
      mode &= ~ENABLE_ECHO_INPUT;
      SetConsoleMode(h, mode);
    }
#else
    struct termios ts;
    tcgetattr(fileno(stdin), &ts);
    ts.c_lflag &= ~ECHO;
    tcsetattr(fileno(stdin), TCSANOW, &ts);

#endif
  }

  /// @brief Prints 'Press any key to continue...' and waits for any key input.
  static void press_to_continue() noexcept
  {
    std::fputs("Press any key to continue...", stdout);
    toggle_echo();
    (void)getchar();
    toggle_echo();
    std::fputc('\n', stdout);
  }

  
  Expect<u32, IOError> gets_no_echo(clt::Span<char> span) noexcept
    COLT_PRE(!span.is_empty())
  {
    auto n = static_cast<u32>(span.size());
    char* buffer = span.data();
    
    toggle_echo();
    char chr = getchar();
    if (chr == EOF)
      return { Error, IOError::FILE_EOF };
    buffer[0] = chr;
    u32 i = 1;
    for (; i < n; i++)
    {
      chr = getchar();
      if (chr == EOF || chr == '\n')
        break;
      buffer[i] = chr;
    }
    toggle_echo();
    return { i };
  }
  COLT_POST()

  Expect<u32, IOError> gets(Span<char> span) noexcept
    COLT_PRE(!span.is_empty())
  {
    auto n = static_cast<u32>(span.size());
    char* buffer = span.data();

    int chr = getchar();
    if (chr == EOF)
    {
      if (feof(stdin))
        return { Error, IOError::FILE_EOF };
      else
        return { Error, IOError::FILE_ERROR };
    }
    buffer[0] = static_cast<char>(chr);
    u32 i = 1;
    for (; i < n; i++)
    {
      chr = getchar();
      if (chr == EOF || chr == '\n')
        break;
      buffer[i] = static_cast<char>(chr);
    }
    return { i };
  }
  COLT_POST()

  template<typename T, meta::StringLiteral endl = "\n", typename... Args> requires (Formatable<Args> && ...)
  inline Expect<T, IOError> input(std::FILE* file, fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    print<endl>(fmt, std::forward<Args>(args)...);
    int chr;
    //Consume spaces
    while ((chr = std::fgetc(file)) != EOF)
      if (!std::isspace(chr))
        break;
    char buffer[128];
    size_t size = 0;
    while ((chr = std::fgetc(file)) != EOF)
      buffer[size++] = static_cast<char>(chr);
    clt::parse<T>(StringView{ buffer, size });
  }

  template<typename T, meta::StringLiteral endl = "\n", typename... Args> requires (Formatable<Args> && ...)
  inline Expect<T, IOError> input(fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    return input(stdin, fmt, std::forward<Args>(args)...);
  }
}

#endif //!HG_COLT_INPUT