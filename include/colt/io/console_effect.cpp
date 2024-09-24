#include "colt/macro/config.h"
#include "console_effect.h"
#include <thread>

#ifndef COLT_WINDOWS
  #include <termios.h>
  #include <unistd.h>
  #include <sys/ioctl.h>
  #include <fcntl.h>
#else
  #define NOMINMAX
  #include <Windows.h>
  #include <conio.h>
#endif //COLT_WINDOWS

#include <cstdio>

namespace clt::io
{
  void toggle_echo() noexcept
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
  
  void terminal_size(int& width, int& height) noexcept
  {
#if defined(COLT_WINDOWS)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width  = (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
    height = (int)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
#else
    struct winsize w;
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
    width  = (int)(w.ws_col);
    height = (int)(w.ws_row);
#endif // Windows/Linux
  }
  
  void wait_kbhit() noexcept
  {
    using namespace std::chrono_literals;
#ifdef COLT_WINDOWS
    while (!_kbhit())
      std::this_thread::sleep_for(10ms);
#else
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
      ungetc(ch, stdin);
#endif // COLT_WINDOWS
  }
} // namespace clt::io