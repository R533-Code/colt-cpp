#ifndef HG_COLT_PARSE
#define HG_COLT_PARSE

#include <charconv>
#include <concepts>

#include "../libraries/fast_float/include/fast_float/fast_float.h"

#include "../structs/string_view.h"
#include "../util/params.h"
#include "../util/typedefs.h"
#include "../refl/enum.h"

DECLARE_ENUM_WITH_TYPE(u8, clt, ParseErrorCode, SUCCESS, INVALID_FMT, OUT_OF_RANGE);

namespace clt
{
  /// @brief Result of parsing a string
  struct ParseResult
  {
    /// @brief Pointer to the character after the last parsed
    const char* end;
    /// @brief Code representing the result status
    ParseErrorCode code;
  };

  namespace details
  {
    constexpr ParseErrorCode errc_to_parse_code(std::errc code) noexcept
    {
      switch (code)
      {
      case std::errc::invalid_argument:
        return ParseErrorCode::INVALID_FMT;
      case std::errc::result_out_of_range:
        return ParseErrorCode::OUT_OF_RANGE;
      default:
        colt_unreachable("Invalid std::errc!");
      }
    }
  }

  template<typename T>
  struct parser {};

  template<meta::Integral T>
  /// @brief Overload for integrals
  struct parser<T>
  {
    ParseResult operator()(maybe_out<T> value, StringView to_parse) const noexcept
    {
      T int_result;
      auto [ptr, errc] = std::from_chars(to_parse.begin(), to_parse.end(), int_result);
      if (errc == std::errc{})
      {
        value.construct(int_result);
        return ParseResult{ ptr, ParseErrorCode::SUCCESS };
      }
      else
        return ParseResult{ ptr, details::errc_to_parse_code(errc) };
    }
  };

  template<meta::FloatingPoint T>
  /// @brief Overload for floating-points
  struct parser<T>
  {
    ParseResult operator()(maybe_out<T> value, StringView to_parse) const noexcept
    {
      T fp_result;
      auto [ptr, errc] = fast_float::from_chars(to_parse.begin(), to_parse.end(), fp_result);
      if (errc == std::errc{})
      {
        value.construct(fp_result);
        return ParseResult{ ptr, ParseErrorCode::SUCCESS };
      }
      else
        return ParseResult{ ptr, details::errc_to_parse_code(errc) };
    }
  };

  template<>
  /// @brief Overload for booleans ([TtFf01]|true|false)
  struct parser<bool>
  {
    ParseResult operator()(maybe_out<bool> value, StringView to_parse) const noexcept
    {
      using enum clt::ParseErrorCode;
      
      if (to_parse.is_empty())
        return ParseResult{ to_parse.end(), INVALID_FMT };
      if (to_parse.size() == 1)
      {
        if (auto lower = static_cast<char>(std::tolower(to_parse.front()));
          lower == 't' || lower == '1')
        {
          value.construct(true);
          return ParseResult{ to_parse.end(), SUCCESS };
        }
        else if (lower == 'f' || lower == '0')
        {
          value.construct(false);
          return ParseResult{ to_parse.end(), SUCCESS };
        }
        return ParseResult{ to_parse.end(), INVALID_FMT };
      }
      else if (to_parse == "true")
      {
        value.construct(true);
        return ParseResult{ to_parse.end(), SUCCESS };
      }
      else if (to_parse == "false")
      {
        value.construct(false);
        return ParseResult{ to_parse.end(), SUCCESS };
      }
      return ParseResult{ to_parse.end(), INVALID_FMT };
    }
  };

  namespace meta
  {
    template<typename T>
    /// @brief Check if type has a parse<> specialization
    concept Parsable = requires (StringView strv, maybe_out<T> out)
    {
      { parser<T>{}(out, strv)}->std::same_as<ParseResult>;
    };
  }
}

#endif //!HG_COLT_PARSE