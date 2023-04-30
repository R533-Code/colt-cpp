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

  
  inline Expect<u32, IOError> gets_no_echo(clt::Span<char> span) noexcept
    COLT_PRE(!span.is_empty())
  {
    auto n = static_cast<u32>(span.size());
    char* buffer = span.data();
    
    toggle_echo();
    int chr = getchar();
    if (chr == EOF)
      return { Error, IOError::FILE_EOF };
    buffer[0] = chr;
    u32 i = 1;
    for (; i < n; i++)
    {
      chr = getchar();
      if (chr == EOF || chr == '\n')
        break;
      buffer[i] = static_cast<char>(chr);
    }
    toggle_echo();
    return { i };
  }
  COLT_POST()

  inline Expect<u32, IOError> gets(Span<char> span) noexcept
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

  template<meta::Inputable T = String, meta::StringLiteral endl = "", typename... Args>
  inline Expect<T, IOError> input(std::FILE* file, fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    //Print the message...
    print<endl>(fmt, std::forward<Args>(args)...);
    //Ask for input...
    auto str = String::getLine(file, str::recommended_size<T>(),
      !meta::NoStripParsable<T>);
    if (str.is_error()) //FILE_EOF or FILE_ERROR
      return { Error, str.error() };
    
    //The string to parse.
    //As String::getLine strips the spaces from the front,
    //we only strip the spaces in the back.
    auto strv = StringView{ str->begin(), str->end() };
    if constexpr (!meta::NoStripParsable<T>)
      strv.strip_suffix();
    
    //The variable in which to store the result
    uninit<T> result;
    
    //Parse the line
    auto [ptr, err] = clt::str::parser<T>{}(result, strv);
    if (err != ParseErrorCode::SUCCESS)
    {
      if (err == ParseErrorCode::INVALID_FMT)
        return { Error, IOError::INVALID_FMT };
      else if (err == ParseErrorCode::OUT_OF_RANGE)
        return { Error, IOError::OUT_OF_RANGE };
      //Check if a new IOError was added...
      colt_unreachable("Invalid ParseErrorCode!");
    }
    //err == SUCCESS, the object was constructed,
    //We need to destruct it.
    ON_SCOPE_EXIT{
      result.destruct();
    };
    
    //There was more characters that did not match:
    // "10 x" is not a valid integer as the " x" won't be accessible.
    if (ptr != strv.end())
      return { Error, IOError::INVALID_FMT };
    return { InPlace, std::move(result.data()) };
  }

  template<meta::Inputable T = String, meta::StringLiteral endl = "", typename... Args>
  inline Expect<T, IOError> input(fmt_str<Args...> fmt, Args&&... args) noexcept
  {
    return input<T>(stdin, fmt, std::forward<Args>(args)...);
  }
}

#endif //!HG_COLT_INPUT