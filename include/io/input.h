#ifndef HG_COLT_INPUT
#define HG_COLT_INPUT

#ifndef _WIN32
  #include <termios.h>
  #include <unistd.h>
#else
  #include <Windows.h>
#endif //COLT_WINDOWS

#include <utility>
#include "./print.h"
#include "../util/contracts.h"

namespace clt::io
{
  enum class IOError
  {
    NO_ERR,
    FILE_EOF
  };

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
    tcgetattr(fileno(f), &ts);
    ts.c_lflag &= ~ECHO;
    tcsetattr(fileno(f), TCSANOW, &ts);

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

  
  std::pair<u32, IOError> gets_no_echo(char* buffer, u32 n) noexcept
  COLT_PRE(n != 0)
  {
    toggle_echo();
    char chr = getchar();
    if (chr == EOF)
      return { 0, IOError::FILE_EOF };
    buffer[0] = chr;
    size_t i = 1;
    for (; i < n; i++)
    {
      chr = getchar();
      if (chr == EOF || chr == '\n')
        break;
      buffer[i] = chr;
    }
    toggle_echo();
    return { i, IOError::NO_ERR };
  }
  COLT_POST()

  std::pair<u32, IOError> gets(char* buffer, u32 n) noexcept
  COLT_PRE(n != 0)
  {
    char chr = getchar();
    if (chr == EOF)
      return { 0, IOError::FILE_EOF };
    buffer[0] = chr;
    size_t i = 1;
    for (; i < n; i++)
    {
      chr = getchar();
      if (chr == EOF || chr == '\n')
        break;
      buffer[i] = chr;
    }
    return { i, IOError::NO_ERR };
  }
  COLT_POST()
}

#endif //!HG_COLT_INPUT