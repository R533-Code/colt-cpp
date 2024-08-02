/*****************************************************************/ /**
 * @file   ansi_color.h
 * @brief  Contains ANSIEffect.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_IO_ANSI_COLOR
#define HG_IO_ANSI_COLOR

#include <array>
#include <cstdint>
#include <fmt/format.h>

namespace clt::io
{
  namespace details
  {
    /// @brief Array of effects
    static constexpr std::array CONSOLE_EFFECTS = {
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

  /// @brief If true, then colored output is used
  inline thread_local bool OutputColor = true;

  /// @brief Represents a Console ANSIEffect
  struct ANSIEffect
  {
    /// @brief Index into CONSOLE_COLOR array
    uint32_t index;
  };

  /// @brief Black foreground
  static constexpr ANSIEffect BlackF = ANSIEffect{1};
  /// @brief Red foreground
  static constexpr ANSIEffect RedF = ANSIEffect{2};
  /// @brief Green foreground
  static constexpr ANSIEffect GreenF = ANSIEffect{3};
  /// @brief Yellow foreground
  static constexpr ANSIEffect YellowF = ANSIEffect{4};
  /// @brief Blue foreground
  static constexpr ANSIEffect BlueF = ANSIEffect{5};
  /// @brief Magenta foreground
  static constexpr ANSIEffect MagentaF = ANSIEffect{6};
  /// @brief Cyan foreground
  static constexpr ANSIEffect CyanF = ANSIEffect{7};
  /// @brief White foreground (usually the default)
  static constexpr ANSIEffect WhiteF = ANSIEffect{8};

  /// @brief Bright Black foreground
  static constexpr ANSIEffect BrightBlackF = ANSIEffect{9};
  /// @brief Bright Red foreground
  static constexpr ANSIEffect BrightRedF = ANSIEffect{10};
  /// @brief Bright Green foreground
  static constexpr ANSIEffect BrightGreenF = ANSIEffect{11};
  /// @brief Bright Yellow foreground
  static constexpr ANSIEffect BrightYellowF = ANSIEffect{12};
  /// @brief Bright Blue foreground
  static constexpr ANSIEffect BrightBlueF = ANSIEffect{13};
  /// @brief Bright Magenta foreground
  static constexpr ANSIEffect BrightMagentaF = ANSIEffect{14};
  /// @brief Bright Cyan foreground
  static constexpr ANSIEffect BrightCyanF = ANSIEffect{15};
  /// @brief Bright White foreground
  static constexpr ANSIEffect BrightWhiteF = ANSIEffect{16};

  /// @brief Black background
  static constexpr ANSIEffect BlackB = ANSIEffect{17};
  /// @brief Red background
  static constexpr ANSIEffect RedB = ANSIEffect{18};
  /// @brief Green background
  static constexpr ANSIEffect GreenB = ANSIEffect{19};
  /// @brief Yellow background
  static constexpr ANSIEffect YellowB = ANSIEffect{20};
  /// @brief Blue background
  static constexpr ANSIEffect BlueB = ANSIEffect{21};
  /// @brief Magenta background
  static constexpr ANSIEffect MagentaB = ANSIEffect{22};
  /// @brief Cyan background
  static constexpr ANSIEffect CyanB = ANSIEffect{23};
  /// @brief White background
  static constexpr ANSIEffect WhiteB = ANSIEffect{24};

  /// @brief Bright Black background
  static constexpr ANSIEffect BrightBlackB = ANSIEffect{25};
  /// @brief Bright Red background
  static constexpr ANSIEffect BrightRedB = ANSIEffect{26};
  /// @brief Bright Green background
  static constexpr ANSIEffect BrightGreenB = ANSIEffect{27};
  /// @brief Bright Yellow background
  static constexpr ANSIEffect BrightYellowB = ANSIEffect{28};
  /// @brief Bright Blue background
  static constexpr ANSIEffect BrightBlueB = ANSIEffect{29};
  /// @brief Bright Magenta background
  static constexpr ANSIEffect BrightMagentaB = ANSIEffect{30};
  /// @brief Bright Cyan background
  static constexpr ANSIEffect BrightCyanB = ANSIEffect{31};
  /// @brief Bright White background
  static constexpr ANSIEffect BrightWhiteB = ANSIEffect{32};

  /// @brief Reset foreground and background color to default
  static constexpr ANSIEffect Reset = ANSIEffect{33};
  /// @brief Bold font
  static constexpr ANSIEffect Bold = ANSIEffect{34};
  /// @brief Underline
  static constexpr ANSIEffect Underline = ANSIEffect{35};
  /// @brief Flicker
  static constexpr ANSIEffect Flicker = ANSIEffect{36};
  /// @brief Switch foreground and background color
  static constexpr ANSIEffect SwitchFB = ANSIEffect{37};
} // namespace clt::io

template<>
/// @brief {fmt} specialization of ANSIEffect
struct fmt::formatter<clt::io::ANSIEffect>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const clt::io::ANSIEffect& op, FormatContext& ctx) const
  {
    // If OutputColor is false, we write an empty string "".
    return fmt::format_to(
        ctx.out(), "{}",
        clt::io::details::CONSOLE_EFFECTS
            [op.index * static_cast<uint64_t>(clt::io::OutputColor)]);
  }
};

#endif // !HG_IO_ANSI_COLOR