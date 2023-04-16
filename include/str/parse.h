#ifndef HG_COLT_PARSE
#define HG_COLT_PARSE

#include <charconv>
#include <concepts>
#include <limits>

#include "../libraries/fast_float/include/fast_float/fast_float.h"

#include "../structs/string_view.h"
#include "../util/params.h"
#include "../util/typedefs.h"
#include "../refl/enum.h"

DECLARE_ENUM_WITH_TYPE(u8, clt, ParseErrorCode, SUCCESS, INVALID_FMT, OUT_OF_RANGE);

namespace clt::meta
{
  template<Integral T>
  inline constexpr u64 max_digits10_v = clt::ceil(clt::log10(std::numeric_limits<T>::max())) + std::is_signed_v<T>;
}

namespace clt::str
{
  /// @brief Inherit from this struct to signify that the StringView passed
  /// to parse should not strip spaces.
  struct DoNotStripSpaces {};

  template<u64 ESTIMATION>
  /// @brief Inherit from this struct to provide an estimation of the count
  /// of characters that getLine should reserve in 'input'.
  struct Recommended
  {
    static constexpr u64 size = ESTIMATION;
  };

  /// @brief Inherit from this struct to disable 'input' function use
  /// for that type. Useful for views, which do not own data, as returning
  /// them from 'input' will return a view over a now destructed variable.
  struct NotInputable {};

  /// @brief Inherit from this struct to mark the parser as accepting
  /// leading space.
  struct AcceptsLeadingSpaces {};

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
    : Recommended<meta::max_digits10_v<T>>
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
    : Recommended<std::numeric_limits<T>::max_digits10()>
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
    : Recommended<5>
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

  template<>
  /// @brief Overload for char
  struct parser<char>
    : DoNotStripSpaces, Recommended<1>
  {
    constexpr ParseResult operator()(maybe_out<char> chr, StringView to_parse) const noexcept
    {
      if (to_parse.size() != 1)
        return ParseResult{ to_parse.end(), ParseErrorCode::INVALID_FMT };
      chr.construct(to_parse.front());
      return ParseResult{ to_parse.end(), ParseErrorCode::SUCCESS };
    }
  };

  template<>
  /// @brief Overload for StringView
  struct parser<StringView>
    : NotInputable, AcceptsLeadingSpaces
  {
    constexpr ParseResult operator()(maybe_out<StringView> str, StringView to_parse) const noexcept
    {
      str.construct(to_parse);
      return ParseResult{ to_parse.end(), ParseErrorCode::SUCCESS };
    }
  };
}

namespace clt::meta
{
  template<typename T>
  /// @brief Check if type has a parse<> specialization
  concept Parsable = requires (StringView strv, maybe_out<T> out)
  {
    { str::parser<T>{}(out, strv)}->std::same_as<str::ParseResult>;
  };

  template<typename T>
  /// @brief Check if type has a parse<> specialization and support being returned from 'input'
  concept Inputable = Parsable<T> && (!std::is_base_of_v<str::NotInputable, T>);

  template<typename T>
  /// @brief Check if type is parsable and requires not to strip spaces
  concept NoStripParsable = Parsable<T> && std::is_base_of_v<str::DoNotStripSpaces, T>;

  template<typename T>
  /// @brief Check if type has a parse<> specialization and support being returned from 'input'
  concept RecommendedSizeParsable = Parsable<T> && std::same_as<std::decay_t<decltype(str::parser<T>::size)>, u64>;
}

namespace clt::str
{
  template<meta::Parsable T>
  constexpr u64 recommended_size() noexcept
  {
    if constexpr (meta::RecommendedSizeParsable<T>)
      return parser<T>::size == 0 ? 64 : parser<T>::size;
    return 64;
  }
}


#endif //!HG_COLT_PARSE