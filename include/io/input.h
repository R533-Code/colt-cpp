/** @file input.h
* Contains input<>() functions for getting input from the console.
*/

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

#include "../str/parse.h"
#include "./print.h"
#include "../util/contracts.h"
#include "../refl/enum.h"
#include "../structs/string.h"

namespace clt::io
{
  inline void toggle_echo() noexcept
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
  inline void press_to_continue() noexcept
  {
    std::fputs("Press any key to continue...", stdout);
    toggle_echo();
    (void)getchar();
    toggle_echo();
    std::fputc('\n', stdout);
  }  

  template<typename T = String, typename... Args>
  inline Expect<T, ParsingResult> input(std::FILE* file, fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    //Print the message...
    print<"">(fmt, std::forward<Args>(args)...);
    //Ask for input...
    auto str = String::getLine(file);
    if (str.is_error()) //FILE_EOF or FILE_ERROR or INVALID_ENCODING
      return { Error, clt::details::IOError_to_ParsingResult(str.error()) };

    T ret;
    StringView strv = *str;
    auto result = scn::scan_default(strv.strip(), ret);
    if (result)
    {
      if (!result.empty())
        return { Error, ParsingResult{ ParsingCode::NON_EMPTY_REM, "Not all characters were consumed!" } };
      return std::move(ret);
    }
    return { Error, clt::details::scn_error_to_ParsingResult(result.error()) };
  }

  template<typename T = String, meta::StringLiteral endl = "", typename... Args>
  inline Expect<T, ParsingResult> input(fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    return input<T>(stdin, fmt, std::forward<Args>(args)...);
  }
}

#endif //!HG_COLT_INPUT