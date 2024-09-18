/*****************************************************************/ /**
 * @file   unicode_types.h
 * @brief  Contains unicode types used throughout the library.
 * 
 * A Primer On Unicode:
 * Unicode defines a mapping between symbols and positive integers.
 * In a simplified way, Unicode defines that the character A is
 * represented as 65 (called its CODE POINT).
 * The different encoding types exist to encode that integer in
 * a smart way.
 * UTF8: variadic, storage type of 8 bits.
 * UTF16: variadic, storage type of 16 bits.
 * UTF32: fixed, storage type of 32 bits.
 * 
 * 'Variadic' means that a single code point may be represented
 * using a sequence of storage type:
 * As an example, 0x0101 is represented using 2 bytes in UTF8.
 * 
 * Computers also have a property called endianness:
 * The order of bytes representing an integer.
 * It usually comes in two flavors:
 * - Little: stores the least-significant byte first
 * - Big: stores the most-significant byte first
 * This means that for UTF16 and UTF32 the encoding might be
 * Little Endian (LE) or Big Endian (BE).
 * 
 * A GRAPHEME is a single unit seen by the user.
 * Take for example the female construction worker emoji.
 * This is perceived as a single character by the user.
 * But, it is represented by 3 code points:
 * {Construction Worker}{ZWJ}{Female Sign}.
 * 
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_UNI_UNICODE_TYPES
#define HG_UNI_UNICODE_TYPES

#include <uni_algo/all.h>
#include <simdutf.h>
#include <cstring>
#include <utility>
#include <ranges>

#include "colt/typedefs.h"
#include "colt/meta/traits.h"
#include "colt/macro/assert.h"
#include "colt/macro/on_scope_exit.h"
#include "colt/bit/endian.h"
#include "colt/num/math.h"
#include "colt/dsa/option.h"

namespace clt
{
  /// @brief Represents the possible character encoding
  enum class StringEncoding : u8
  {
    /// @brief ASCII
    ASCII = 0,
    /// @brief UTF8
    UTF8 = 1,
    /// @brief UTF16 Big Endian
    UTF16BE = 2,
    /// @brief UTF16 Little Endian
    UTF16LE = 3,
    /// @brief UTF32 Big Endian
    UTF32BE = 4,
    /// @brief UTF32 Little Endian
    UTF32LE = 5,

#ifdef COLT_LITTLE_ENDIAN
    /// @brief UTF16 in host format (little endian)
    UTF16 = UTF16LE,
    /// @brief UTF32 in host format (little endian)
    UTF32 = UTF32LE
#elif defined(COLT_BIG_ENDIAN)
    /// @brief UTF16 in host format (big endian)
    UTF16 = UTF16BE,
    /// @brief UTF32 in host format (big endian)
    UTF32 = UTF32BE
#else
  #error
#endif // COLT_LITTLE_ENDIAN
  };

  /// @brief Check if 'encoding' is UTF16 or UTF8 (where a single code point
  /// might occupy more than a single object).
  /// @param encoding The encoding to check for
  /// @return True if UTF16BE, UTF16LE or UTF8
  constexpr bool is_variadic_encoding(StringEncoding encoding) noexcept
  {
    using enum clt::StringEncoding;
    return encoding == UTF16BE || encoding == UTF16LE || encoding == UTF8;
  }

  namespace uni
  {
    /// @brief The minimum value of the lead surrogate
    static constexpr char16_t LEAD_SURROGATE_MIN = 0xd800u;
    /// @brief The maximum value of the lead surrogate
    static constexpr char16_t LEAD_SURROGATE_MAX = 0xdbffu;
    /// @brief The minimum value of the trailing surrogate
    static constexpr char16_t TRAIL_SURROGATE_MIN = 0xdc00u;
    /// @brief The maximum value of the trailing surrogate
    static constexpr char16_t TRAIL_SURROGATE_MAX = 0xdfffu;
    /// @brief LEAD_SURROGATE_MIN - (0x10000 >> 10)
    static constexpr char16_t LEAD_OFFSET = 0xd7c0u;
    /// @brief 0x10000u - (LEAD_SURROGATE_MIN << 10) - TRAIL_SURROGATE_MIN
    static constexpr char32_t SURROGATE_OFFSET = 0xfca02400u;
    /// @brief The maximum code point that can be stored (inclusive)
    static constexpr char32_t CODE_POINT_MAX = 0x0010ffffu;

    /// @brief Check if value represents a lead surrogate
    /// @param value The value
    /// @return True if lead surrogate
    constexpr bool is_lead_surrogate(char16_t value) noexcept
    {
      return value >= LEAD_SURROGATE_MIN && value <= LEAD_SURROGATE_MAX;
    }

    /// @brief Check if value represents a trail surrogate
    /// @param value The value
    /// @return True if trail surrogate
    constexpr bool is_trail_surrogate(char16_t value) noexcept
    {
      return value >= TRAIL_SURROGATE_MIN && value <= TRAIL_SURROGATE_MAX;
    }
    
    /// @brief Check if a value represents a trail byte in UTF8
    /// @param value The value
    /// @return True if trail byte
    constexpr bool is_trail(char8_t value) noexcept
    {
      return (value >> 6) == 0b10;
    }

    /// @brief Returns the sequence length of a UTF16 sequence starting with value.
    /// @param value The start of the sequence
    /// @return 1 or 2
    constexpr u8 sequence_length(char16_t value) noexcept
    {
      return 1 + (u8)is_lead_surrogate(value);
    }

    /// @brief Returns the sequence length of a UTF8 sequence starting with value.
    /// This function will return 1 on invalid UTF8.
    /// @param value The start of the sequence
    /// @return 1 or 2
    constexpr u8 sequence_length(char8_t _value) noexcept
    {
      if (_value < 0x80) [[likely]]
        return 1;
      else if ((_value >> 5) == 0b110)
        return 2;
      else if ((_value >> 4) == 0b1110)
        return 3;
      if ((_value >> 3) == 0b1110)
        return 4;
      return 1;
    }

    /// @brief Check if a code point is in the Basic Multilingual Plane
    /// @param cp The code point
    /// @return True if 'cp' is in the BMP
    constexpr bool is_in_bmp(char32_t cp)
    {
      return cp < char32_t(0x10000);
    }
  } // namespace uni

  /// @brief 8-bit char (used for UTF8)
  class Char8
  {
    /// @brief The value
    char8_t _value = 0;

  public:
    /// @brief The encoding of the current type
    static constexpr StringEncoding encoding = StringEncoding::UTF8;
    /// @brief The maximum count of Char32 needed to form a code point
    static constexpr size_t max_sequence = 4;

    /// @brief Constructor
    /// @param value Initial value
    constexpr Char8(char8_t value) noexcept
        : _value(value)
    {
    }

    /// @brief Constructor
    /// @param value Initial value
    constexpr Char8(char value) noexcept
        : _value((char8_t)value)
    {
    }
    constexpr Char8() noexcept = default;
    MAKE_DEFAULT_COPY_AND_MOVE_FOR(Char8);

    /// @brief Returns the value
    constexpr operator char8_t() const noexcept { return _value; }

    /// @brief Returns the value
    /// @return The value of the Char8
    constexpr char8_t value() const noexcept { return _value; }

    /// @brief Check if the current char represents a trailing UTF8.
    /// A trailing UTF8 is any char that is NOT the beginning of a UTF8
    /// sequence. An invalid UTF8 char cannot be trail.
    /// @return True if not the beginning of a UTF8 sequence
    constexpr bool is_trail() const noexcept { return (_value >> 6) == 0b10; }

    /// @brief Check if the current char represents a lead UTF8.
    /// A lead UTF8 is either an ASCII character or the start
    /// of a multibyte UTF8 sequence.
    /// @return True if start of a UTF8 sequence or ASCII
    constexpr bool is_valid_lead() const noexcept
    {
      // 0b10xx'xxxx is not a valid lead
      return !is_trail() && _value <= 0b11110111;
    }

    /// @brief The UTF8 sequence length if this is the start of the sequence.
    /// @tparam SAFE True if the method should return an Option None for invalid
    ///              lead. If this is false, the method returns 1.
    /// @return [1, 4] or None (if SAFE and invalid lead)
    template<bool SAFE = true>
    constexpr std::conditional_t<SAFE, Option<u8>, u8> sequence_length()
        const noexcept
    {
      if (_value < 0x80) [[likely]]
        return 1;
      else if ((_value >> 5) == 0b110)
        return 2;
      else if ((_value >> 4) == 0b1110)
        return 3;
      if ((_value >> 3) == 0b1110)
        return 4;
      if constexpr (SAFE)
        return None;
      else
        return 1;
    }
  };

  class Char32BE;
  class Char16BE;
  class Char32LE;
  class Char16LE;

  // TODO: replace with std::conditional_t and static_assert
#ifdef COLT_LITTLE_ENDIAN
  /// @brief Represents a 32-bit char with the host endianness
  using Char32 = Char32LE;
  /// @brief Represents a 32-bit char with reversed endianness from host
  using Char32Other = Char32BE;
  /// @brief Represents a 16-bit char with the host endianness
  using Char16 = Char16LE;
  /// @brief Represents a 16-bit char with reversed endianness from host
  using Char16Other = Char16BE;
#elif defined(COLT_BIG_ENDIAN)
  /// @brief Represents a 32-bit char with the host endianness
  using Char32 = Char32BE;
  /// @brief Represents a 32-bit char with reversed endianness from host
  using Char32Other = Char32LE;
  /// @brief Represents a 16-bit char with the host endianness
  using Char16 = Char16BE;
  /// @brief Represents a 16-bit char with reversed endianness from host
  using Char16Other = Char16LE;
#else
  #error "Unknown endianness!"
#endif

  /// @brief Big Endian 32-bit Char
  class Char32BE
  {
    /// @brief The value (always stored as Big Endian)
    char32_t _value = 0;

  public:
    /// @brief The encoding of the current type
    static constexpr StringEncoding encoding = StringEncoding::UTF32BE;
    /// @brief The maximum count of Char32 needed to form a code point
    static constexpr size_t max_sequence = 1;

    /// @brief Constructs a big endian value from a little endian value
    /// @param value The value (in little endian)
    constexpr Char32BE(Char32LE value) noexcept;
    /// @brief Constructs a big endian value from host endianness
    /// @param value The value (in host endianness)
    constexpr Char32BE(char32_t value) noexcept
        : _value((char32_t)bit::htob((u32)value))
    {
    }

    constexpr Char32BE() noexcept = default;
    MAKE_DEFAULT_COPY_AND_MOVE_FOR(Char32BE);

    /// @brief Return the current value as a host CodePoint.
    /// If the host is big endian, this is a no-op.
    /// @return The current value as a host CodePoint
    constexpr char32_t as_host() const noexcept
    {
      return (char32_t)bit::btoh((u32)_value);
    }
    /// @brief Return the current value as a little endian CodePoint.
    /// @return The current value as a little endian CodePoint
    constexpr Char32LE as_little() const noexcept;
    /// @brief Return the current value as a big endian CodePoint.
    /// This is a no-op.
    /// @return The current value as a big endian CodePoint
    constexpr Char32BE as_big() const noexcept { return *this; }

    /// @brief Returns the value in host endian
    constexpr operator char32_t() const noexcept { return as_host(); }
    /// @brief Returns the value in little endian
    constexpr operator Char32LE() const noexcept;

    /// @brief Returns the value in big endian
    /// @return Value in big endian
    constexpr char32_t in_endian() const noexcept { return _value; }

    /// @brief Check if the current UTF32 is a valid code point
    /// @return True if valid code point
    constexpr bool is_valid() const noexcept
    {
      return as_host() <= uni::CODE_POINT_MAX;
    }
  };

  /// @brief Little Endian 32-bit Char
  class Char32LE
  {
    char32_t _value = 0;

  public:
    /// @brief The encoding of the current type
    static constexpr StringEncoding encoding = StringEncoding::UTF32LE;
    /// @brief The maximum count of Char32 needed to form a code point
    static constexpr size_t max_sequence = 1;

    /// @brief Constructs a little endian value from a big endian value
    /// @param value The value (in little endian)
    constexpr Char32LE(Char32BE value) noexcept;
    /// @brief Constructs a little endian value from host endianness
    /// @param value The value (in host endianness)
    constexpr Char32LE(char32_t value) noexcept
        : _value((char32_t)bit::htol((u32)value))
    {
    }

    constexpr Char32LE() noexcept = default;
    MAKE_DEFAULT_COPY_AND_MOVE_FOR(Char32LE);

    /// @brief Return the current value as a host CodePoint.
    /// If the host is little endian, this is a no-op.
    /// @return The current value as a host CodePoint
    constexpr char32_t as_host() const noexcept
    {
      return (char32_t)bit::ltoh((u32)_value);
    }
    /// @brief Return the current value as a little endian CodePoint.
    /// This is a no-op.
    /// @return The current value as a little endian CodePoint
    constexpr Char32LE as_little() const noexcept { return *this; }
    /// @brief Return the current value as a big endian CodePoint.
    /// @return The current value as a big endian CodePoint
    constexpr Char32BE as_big() const noexcept;

    /// @brief Returns the value in host endian
    constexpr operator char32_t() const noexcept { return as_host(); }
    /// @brief Returns the value in big endian
    constexpr operator Char32BE() const noexcept;

    /// @brief Returns the value in little endian
    /// @return Value in little endian
    constexpr char32_t in_endian() const noexcept { return _value; }

    /// @brief Check if the current UTF32 is a valid code point
    /// @return True if valid code point
    constexpr bool is_valid() const noexcept
    {
      return as_host() <= uni::CODE_POINT_MAX;
    }
  };

  /// @brief Big Endian 16-bit Char
  class Char16BE
  {
    /// @brief The value (always stored as Big Endian)
    char16_t _value = 0;

  public:
    /// @brief The encoding of the current type
    static constexpr StringEncoding encoding = StringEncoding::UTF16BE;
    /// @brief The maximum count of Char16 needed to form a code point
    static constexpr size_t max_sequence = 2;

    /// @brief Constructs a big endian value from a little endian value
    /// @param value The value (in little endian)
    constexpr Char16BE(Char16LE value) noexcept;
    /// @brief Constructs a big endian value from host endianness
    /// @param value The value (in host endianness)
    constexpr Char16BE(char16_t value) noexcept
        : _value((char16_t)bit::htob((u16)value))
    {
    }

    constexpr Char16BE() noexcept = default;
    MAKE_DEFAULT_COPY_AND_MOVE_FOR(Char16BE);

    /// @brief Return the current value as a host CodePoint.
    /// If the host is big endian, this is a no-op.
    /// @return The current value as a host CodePoint
    constexpr char16_t as_host() const noexcept
    {
      return (char16_t)bit::btoh((u16)_value);
    }
    /// @brief Return the current value as a little endian CodePoint.
    /// @return The current value as a little endian CodePoint
    constexpr Char16LE as_little() const noexcept;
    /// @brief Return the current value as a big endian CodePoint.
    /// This is a no-op.
    /// @return The current value as a big endian CodePoint
    constexpr Char16BE as_big() const noexcept { return *this; }

    /// @brief Returns the value in host endian
    constexpr operator char16_t() const noexcept { return as_host(); }
    /// @brief Returns the value in little endian
    constexpr operator Char16LE() const noexcept;

    /// @brief Returns the value in big endian
    /// @return Value in big endian
    constexpr char16_t in_endian() const noexcept { return _value; }

    /// @brief Check if the current Char16 represents a lead surrogate.
    /// A lead surrogate must be followed by a trail surrogate to form
    /// a valid UTF16 sequence.
    /// @return True if lead surrogate
    constexpr bool is_lead_surrogate() const noexcept
    {
      return uni::is_lead_surrogate(*this);
    }

    /// @brief Check if the current Char16 represents a trail surrogate.
    /// A trail surrogate must be preceded by a lead surrogate to form
    /// a valid UTF16 sequence.
    /// @return True if trail surrogate
    constexpr bool is_trail_surrogate() const noexcept
    {
      return uni::is_trail_surrogate(*this);
    }

    /// @brief The UTF16 sequence length if this is the start of the sequence.
    /// @return 1 or 2
    constexpr u8 sequence_length() const noexcept
    {
      return 1 + (u8)is_lead_surrogate();
    }
  };

  /// @brief Little Endian 16-bit Char
  class Char16LE
  {
    char16_t _value = 0;

  public:
    /// @brief The encoding of the current type
    static constexpr StringEncoding encoding = StringEncoding::UTF16LE;
    /// @brief The maximum count of Char16 needed to form a code point
    static constexpr size_t max_sequence = 2;

    /// @brief Constructs a little endian value from a big endian value
    /// @param value The value (in little endian)
    constexpr Char16LE(Char16BE value) noexcept;
    /// @brief Constructs a little endian value from host endianness
    /// @param value The value (in host endianness)
    constexpr Char16LE(char16_t value) noexcept
        : _value((char16_t)bit::htol((u16)value))
    {
    }

    constexpr Char16LE() noexcept = default;
    MAKE_DEFAULT_COPY_AND_MOVE_FOR(Char16LE);

    /// @brief Return the current value as a host CodePoint.
    /// If the host is little endian, this is a no-op.
    /// @return The current value as a host CodePoint
    constexpr char16_t as_host() const noexcept
    {
      return (char16_t)bit::ltoh((u16)_value);
    }
    /// @brief Return the current value as a little endian CodePoint.
    /// This is a no-op.
    /// @return The current value as a little endian CodePoint
    constexpr Char16LE as_little() const noexcept { return *this; }
    /// @brief Return the current value as a big endian CodePoint.
    /// @return The current value as a big endian CodePoint
    constexpr Char16BE as_big() const noexcept;

    /// @brief Returns the value in host endian
    constexpr operator char16_t() const noexcept { return as_host(); }
    /// @brief Returns the value in little endian
    constexpr operator Char16BE() const noexcept;

    /// @brief Returns the value in little endian
    /// @return Value in little endian
    constexpr char16_t in_endian() const noexcept { return _value; }

    /// @brief Check if the current Char16 represents a lead surrogate.
    /// A lead surrogate must be followed by a trail surrogate to form
    /// a valid UTF16 sequence.
    /// @return True if lead surrogate
    constexpr bool is_lead_surrogate() const noexcept
    {
      return uni::is_lead_surrogate(*this);
    }

    /// @brief Check if the current Char16 represents a trail surrogate.
    /// A trail surrogate must be preceded by a lead surrogate to form
    /// a valid UTF16 sequence.
    /// @return True if trail surrogate
    constexpr bool is_trail_surrogate() const noexcept
    {
      return uni::is_trail_surrogate(*this);
    }

    /// @brief The UTF16 sequence length if this is the start of the sequence.
    /// @return 1 or 2
    constexpr u8 sequence_length() const noexcept
    {
      return 1 + (u8)is_lead_surrogate();
    }
  };

  namespace meta
  {
    /// @brief Represents any of the Colt char types
    template<typename T>
    concept CharType =
        meta::is_any_of<T, char, Char8, Char16LE, Char16BE, Char32LE, Char32BE>;

    /// @brief Represents any of the C++ char types
    template<typename T>
    concept CppCharType = meta::is_any_of<T, char, char8_t, char16_t, char32_t>;

    /// @brief Returns "" with the right char type
    /// @tparam T The char type
    /// @return "" or u8"" or u"" or U""
    template<CharType T>
    consteval auto empty_string_literal() noexcept
    {
      // We can cheat a bit as 0 is represented the same on different endianness.
      if constexpr (std::same_as<T, char>)
        return "";
      if constexpr (std::same_as<T, Char8>)
        return u8"";
      if constexpr (meta::is_any_of<T, Char16LE, Char16BE>)
        return u"";
      if constexpr (meta::is_any_of<T, Char32BE, Char32LE>)
        return U"";
    }

    template<CppCharType Ty>
    struct cppchar_to_char
    {
    };
    template<>
    struct cppchar_to_char<char>
    {
      using type = char;
    };
    template<>
    struct cppchar_to_char<char8_t>
    {
      using type = Char8;
    };
    template<>
    struct cppchar_to_char<char16_t>
    {
      using type = Char16;
    };
    template<>
    struct cppchar_to_char<char32_t>
    {
      using type = Char32;
    };
    template<CppCharType Ty>
    using cppchar_to_char_t = typename cppchar_to_char<Ty>::type;

    /// @brief Converts an encoding to the char that must represent it
    /// @tparam ENCODING The encoding to convert
    template<StringEncoding ENCODING>
    struct encoding_to_char
    {
    };
    template<>
    struct encoding_to_char<StringEncoding::ASCII>
    {
      using type = char;
    };
    template<>
    struct encoding_to_char<StringEncoding::UTF8>
    {
      using type = Char8;
    };
    template<>
    struct encoding_to_char<StringEncoding::UTF16BE>
    {
      using type = Char16BE;
    };
    template<>
    struct encoding_to_char<StringEncoding::UTF16LE>
    {
      using type = Char16LE;
    };
    template<>
    struct encoding_to_char<StringEncoding::UTF32BE>
    {
      using type = Char32BE;
    };
    template<>
    struct encoding_to_char<StringEncoding::UTF32LE>
    {
      using type = Char32LE;
    };

    template<StringEncoding ENCODING>
    using encoding_to_char_t = typename encoding_to_char<ENCODING>::type;

    /// @brief Converts a char type to its encoding
    /// @tparam type The character type
    template<CharType type>
    static constexpr StringEncoding char_to_encoding_v = []()
    {
      using enum clt::StringEncoding;
      if constexpr (std::same_as<type, char>)
        return ASCII;
      if constexpr (std::same_as<type, Char8>)
        return UTF8;
      if constexpr (std::same_as<type, Char16LE>)
        return UTF16LE;
      if constexpr (std::same_as<type, Char16BE>)
        return UTF16BE;
      if constexpr (std::same_as<type, Char32LE>)
        return UTF32LE;
      if constexpr (std::same_as<type, Char32BE>)
        return UTF32BE;
    }();
  } // namespace meta

  constexpr Char32BE::Char32BE(Char32LE value) noexcept
      : _value((char32_t)bit::byteswap((u32)value.in_endian()))
  {
  }

  constexpr Char32LE Char32BE::as_little() const noexcept
  {
    return (char32_t)bit::byteswap((u32)_value);
  }

  constexpr Char32BE::operator Char32LE() const noexcept
  {
    return as_little();
  }

  // ^^^ BIG ENDIAN
  // vvv LITTLE ENDIAN

  constexpr Char32LE::Char32LE(Char32BE value) noexcept
      : _value((char32_t)bit::byteswap((u32)value.in_endian()))
  {
  }

  constexpr Char32BE Char32LE::as_big() const noexcept
  {
    return (char32_t)bit::byteswap((u32)_value);
  }

  constexpr Char32LE::operator Char32BE() const noexcept
  {
    return as_big();
  }

  // ^^^ 32
  // vvv 16

  constexpr Char16BE::Char16BE(Char16LE value) noexcept
      : _value((char16_t)bit::byteswap((u16)value.in_endian()))
  {
  }

  constexpr Char16LE Char16BE::as_little() const noexcept
  {
    return (char16_t)bit::byteswap((u16)_value);
  }

  constexpr Char16BE::operator Char16LE() const noexcept
  {
    return as_little();
  }

  // ^^^ BIG ENDIAN
  // vvv LITTLE ENDIAN

  constexpr Char16LE::Char16LE(Char16BE value) noexcept
      : _value((char16_t)bit::byteswap((u16)value.in_endian()))
  {
  }

  constexpr Char16BE Char16LE::as_big() const noexcept
  {
    return (char16_t)bit::byteswap((u16)_value);
  }

  constexpr Char16LE::operator Char16BE() const noexcept
  {
    return as_big();
  }
} // namespace clt

template<clt::meta::is_any_of<clt::Char32BE, clt::Char32LE> Ty>
struct fmt::formatter<Ty>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const Ty op, FormatContext& ctx) const
  {
    using namespace clt::uni;

    // 4 char max for utf-8 + NUL terminator
    char8_t array8[clt::Char8::max_sequence + 1] = {0};
    auto from                                    = &op;
    auto result                                  = &array8[0];
    if (to_utf8(from, 1, result, 5) == ConvError::NO_ERROR)
      return fmt::format_to(ctx.out(), "{}", reinterpret_cast<const char*>(array8));
    FMT_THROW(std::runtime_error("invalid unicode"));
    clt::unreachable("Cannot be reached!");
  }
};

#endif // !HG_UNI_UNICODE_TYPES
