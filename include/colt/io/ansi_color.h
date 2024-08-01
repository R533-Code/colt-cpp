#ifndef HG_IO_ANSI_COLOR
#define HG_IO_ANSI_COLOR

#include <array>
#include <cstdint>
#include <fmt/format.h>

namespace clt::io
{
  namespace details
  {
    /// @brief Array of colors
    static constexpr std::array CONSOLE_COLORS = {
        "",         //EMPTY
        "\x1B[30m", //CONSOLE_FOREGROUND_BLACK
        "\x1B[31m", //CONSOLE_FOREGROUND_RED
        "\x1B[32m", //CONSOLE_FOREGROUND_GREEN
        "\x1B[33m", //CONSOLE_FOREGROUND_YELLOW
        "\x1B[34m", //CONSOLE_FOREGROUND_BLUE
        "\x1B[35m", //CONSOLE_FOREGROUND_MAGENTA
        "\x1B[36m", //CONSOLE_FOREGROUND_CYAN
        "\x1B[37m", //CONSOLE_FOREGROUND_WHITE

        "\x1B[90m", //CONSOLE_FOREGROUND_BRIGHT_BLACK
        "\x1B[91m", //CONSOLE_FOREGROUND_BRIGHT_RED
        "\x1B[92m", //CONSOLE_FOREGROUND_BRIGHT_GREEN
        "\x1B[93m", //CONSOLE_FOREGROUND_BRIGHT_YELLOW
        "\x1B[94m", //CONSOLE_FOREGROUND_BRIGHT_BLUE
        "\x1B[95m", //CONSOLE_FOREGROUND_BRIGHT_MAGENTA
        "\x1B[96m", //CONSOLE_FOREGROUND_BRIGHT_CYAN
        "\x1B[97m", //CONSOLE_FOREGROUND_BRIGHT_WHITE

        "\x1B[40m", //CONSOLE_BACKGROUND_BLACK
        "\x1B[41m", //CONSOLE_BACKGROUND_RED
        "\x1B[42m", //CONSOLE_BACKGROUND_GREEN
        "\x1B[43m", //CONSOLE_BACKGROUND_YELLOW
        "\x1B[44m", //CONSOLE_BACKGROUND_BLUE
        "\x1B[45m", //CONSOLE_BACKGROUND_MAGENTA
        "\x1B[46m", //CONSOLE_BACKGROUND_CYAN
        "\x1B[47m", //CONSOLE_BACKGROUND_WHITE

        "\x1B[100m", //CONSOLE_BACKGROUND_BRIGHT_BLACK
        "\x1B[101m", //CONSOLE_BACKGROUND_BRIGHT_RED
        "\x1B[102m", //CONSOLE_BACKGROUND_BRIGHT_GREEN
        "\x1B[103m", //CONSOLE_BACKGROUND_BRIGHT_YELLOW
        "\x1B[104m", //CONSOLE_BACKGROUND_BRIGHT_BLUE
        "\x1B[105m", //CONSOLE_BACKGROUND_BRIGHT_MAGENTA
        "\x1B[106m", //CONSOLE_BACKGROUND_BRIGHT_CYAN
        "\x1B[107m", //CONSOLE_BACKGROUND_BRIGHT_WHITE

        "\x1B[0m", //CONSOLE_COLOR_RESET
        "\x1B[2m", //CONSOLE_FONT_BOLD
        "\x1B[4m", //CONSOLE_FONT_UNDERLINE
        "\x1B[5m", //CONSOLE_FONT_FLICKER
        "\x1B[7m", //CONSOLE_COLOR_REVERSE
    };
  } // namespace details

  /// @brief Represents a Console ANSIColor
  struct ANSIColor
  {
    /// @brief Index into CONSOLE_COLOR array
    uint32_t index;
  };

  /// @brief Black foreground
  static constexpr ANSIColor BlackF = ANSIColor{1};
  /// @brief Red foreground
  static constexpr ANSIColor RedF = ANSIColor{2};
  /// @brief Green foreground
  static constexpr ANSIColor GreenF = ANSIColor{3};
  /// @brief Yellow foreground
  static constexpr ANSIColor YellowF = ANSIColor{4};
  /// @brief Blue foreground
  static constexpr ANSIColor BlueF = ANSIColor{5};
  /// @brief Magenta foreground
  static constexpr ANSIColor MagentaF = ANSIColor{6};
  /// @brief Cyan foreground
  static constexpr ANSIColor CyanF = ANSIColor{7};
  /// @brief White foreground (usually the default)
  static constexpr ANSIColor WhiteF = ANSIColor{8};

  /// @brief Bright Black foreground
  static constexpr ANSIColor BrightBlackF = ANSIColor{9};
  /// @brief Bright Red foreground
  static constexpr ANSIColor BrightRedF = ANSIColor{10};
  /// @brief Bright Green foreground
  static constexpr ANSIColor BrightGreenF = ANSIColor{11};
  /// @brief Bright Yellow foreground
  static constexpr ANSIColor BrightYellowF = ANSIColor{12};
  /// @brief Bright Blue foreground
  static constexpr ANSIColor BrightBlueF = ANSIColor{13};
  /// @brief Bright Magenta foreground
  static constexpr ANSIColor BrightMagentaF = ANSIColor{14};
  /// @brief Bright Cyan foreground
  static constexpr ANSIColor BrightCyanF = ANSIColor{15};
  /// @brief Bright White foreground
  static constexpr ANSIColor BrightWhiteF = ANSIColor{16};

  /// @brief Black background
  static constexpr ANSIColor BlackB = ANSIColor{17};
  /// @brief Red background
  static constexpr ANSIColor RedB = ANSIColor{18};
  /// @brief Green background
  static constexpr ANSIColor GreenB = ANSIColor{19};
  /// @brief Yellow background
  static constexpr ANSIColor YellowB = ANSIColor{20};
  /// @brief Blue background
  static constexpr ANSIColor BlueB = ANSIColor{21};
  /// @brief Magenta background
  static constexpr ANSIColor MagentaB = ANSIColor{22};
  /// @brief Cyan background
  static constexpr ANSIColor CyanB = ANSIColor{23};
  /// @brief White background
  static constexpr ANSIColor WhiteB = ANSIColor{24};

  /// @brief Bright Black background
  static constexpr ANSIColor BrightBlackB = ANSIColor{25};
  /// @brief Bright Red background
  static constexpr ANSIColor BrightRedB = ANSIColor{26};
  /// @brief Bright Green background
  static constexpr ANSIColor BrightGreenB = ANSIColor{27};
  /// @brief Bright Yellow background
  static constexpr ANSIColor BrightYellowB = ANSIColor{28};
  /// @brief Bright Blue background
  static constexpr ANSIColor BrightBlueB = ANSIColor{29};
  /// @brief Bright Magenta background
  static constexpr ANSIColor BrightMagentaB = ANSIColor{30};
  /// @brief Bright Cyan background
  static constexpr ANSIColor BrightCyanB = ANSIColor{31};
  /// @brief Bright White background
  static constexpr ANSIColor BrightWhiteB = ANSIColor{32};

  /// @brief Reset foreground and background color to default
  static constexpr ANSIColor Reset = ANSIColor{33};
  /// @brief Bold font
  static constexpr ANSIColor Bold = ANSIColor{34};
  /// @brief Underline
  static constexpr ANSIColor Underline = ANSIColor{35};
  /// @brief Flicker
  static constexpr ANSIColor Flicker = ANSIColor{36};
  /// @brief Switch foreground and background color
  static constexpr ANSIColor SwitchFB = ANSIColor{37};
}

#endif // !HG_IO_ANSI_COLOR