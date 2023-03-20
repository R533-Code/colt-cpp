#ifndef HG_COLT_INPUT
#define HG_COLT_INPUT

#ifndef _WIN32
  #include <termios.h>
  #include <unistd.h>
#else
  #include <conio.h>
#endif //COLT_WINDOWS

#include "./print.h"

namespace clt::io
{
  /// @brief Prints 'Press any key to continue...' and waits for any key input.
  static void press_to_continue() noexcept
  {
    fputs("Press any key to continue...\n", stdout);
#ifndef _WIN32
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    (void)getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);    
#else
    (void)_getch();
#endif //!COLT_WINDOWS
  }
}

#endif //!HG_COLT_INPUT