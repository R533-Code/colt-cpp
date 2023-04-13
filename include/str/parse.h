#ifndef HG_COLT_PARSE
#define HG_COLT_PARSE

#include <charconv>
#include <concepts>

#include "../util/typedefs.h"
#include "../refl/enum.h"

DECLARE_ENUM_WITH_TYPE(u8, clt, ParseErrorCode, NO_ERROR, INVALID_FMT, NOT_REPRESENTABLE);

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

  template<typename T>
  struct parse {};

  template<meta::isIntegral T>
  struct parse
  {
    ParseResult operator()(T value) const noexcept
    {

    }
  };
}

#endif //!HG_COLT_PARSE