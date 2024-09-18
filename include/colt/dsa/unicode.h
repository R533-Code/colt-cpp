/*****************************************************************/ /**
 * @file   unicode.h
 * @brief  Contains unicode utilities used throughout the library.
 * 
 * `unitlen` returns the number of char[8|16|32]_t that makes up a
 * NUL-terminated string.
 * `strlen` returns the number of CODE POINTS that makes up a
 * NUL-terminated string.
 * `countlen` returns the number of CODE POINTS that makes up
 * a string of 'n' units.
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
#ifndef HG_DSA_UNICODE
#define HG_DSA_UNICODE

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

/// @brief The major version of unicode used by Colt
#define COLT_UNICODE_VERSION_MAJOR (UNI_ALGO_UNICODE_VERSION / 1000000)
/// @brief The minor version of unicode used by Colt
#define COLT_UNICODE_VERSION_MINOR (UNI_ALGO_UNICODE_VERSION / 1000 % 1000)
/// @brief The update version of unicode used by Colt
#define COLT_UNICODE_VERSION_UPDATE (UNI_ALGO_UNICODE_VERSION % 1000)

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

  namespace uni
  {
    /// @brief Represents a conversion error
    enum class ConvError
    {
      /// @brief No errors
      NO_ERROR,
      /// @brief Not enough space in the resulting buffer
      NOT_ENOUGH_SPACE,
      /// @brief Invalid input sequence
      INVALID_INPUT,
    };

    /// @brief Converts any unicode encoding to UTF-8.
    /// The pointers are taken by reference:
    /// On INVALID_INPUT, 'from' will point to the start of the character
    /// sequence that is invalid.
    /// On NOT_ENOUGH_SPACE, 'from' will point to the
    /// @tparam Ty The source type
    /// @param from The source start
    /// @param from_size The source count (not byte size!)
    /// @param result The result where to write
    /// @param result_size The result buffer byte count
    /// @return NO_ERROR or the encountered error.
    template<meta::CharType Ty>
    constexpr ConvError to_utf8(
        const Ty*& from, size_t from_size, char8_t*& result,
        size_t result_size) noexcept;

    /// @brief Converts a code point in host endianness to a UTF16BE sequence
    /// @param from The code point to convert
    /// @param result Pointer to write to
    /// @warning 'result' must have at least 2 16-bit integers of capacity
    /// @return Pointer to after the last written character
    constexpr char16_t* unsafe_utf32to16(char32_t from, char16_t* result) noexcept;

    /// @brief Converts a UTF16 surrogate pair to code point
    /// @param high The high surrogate
    /// @param low The low surrogate
    /// @return The code point
    constexpr char32_t surrogate_to_cp(char16_t high, char16_t low) noexcept
    {
      return (high << 10) + low - 0x35fdc00;
    }

    /// @brief Converts a UTF16 sequence to a code point
    /// @param from The start of the sequence
    /// @param result The result in which to write the code point
    /// @return On error, returns 'from' and sets result to REPLACEMENT CHARACTER,
    ///         else the next start of sequence.
    template<typename T>
      requires(meta::is_any_of<T, Char16BE, Char16LE>)
    constexpr const T* unsafe_utf16to32(const T* from, char32_t& result) noexcept;

    /// @brief Converts a UTF8 sequence to a code point
    /// @param from The start of the sequence
    /// @param result The result in which to write the code point
    /// @return On error, returns 'from' and sets result to REPLACEMENT CHARACTER,
    ///         else the next start of sequence.
    constexpr const Char8* unsafe_utf8to32(
        const Char8* from, char32_t& result) noexcept;

    /// @brief Indexes into a sequence of characters, returning the n-th code point.
    /// @tparam underlying_type The character encoding type
    /// @param _ptr The start of the sequence
    /// @param index The code point index
    /// @return The n-th code point
    /// @warning The sequence must be valid unicode
    template<meta::CharType underlying_type>
    constexpr char32_t index_front(
        const underlying_type* _ptr, size_t index) noexcept;

    /// @brief Indexes the back of a unicode sequence.
    /// This function will fire an assert on invalid sequences
    /// @tparam underlying_type The encoding char type
    /// @param _ptr The pointer (must be passed the end of the last character)
    /// @param _index The index (0 for last, 1 for penultimate)
    /// @return The decoded code point
    template<meta::CharType underlying_type>
    constexpr char32_t index_back(
        const underlying_type* _ptr, size_t _index) noexcept;

    template<typename underlying_type>
      requires(meta::CharType<underlying_type> || meta::CppCharType<underlying_type>)
    constexpr const underlying_type* iterator_index_front(
        const underlying_type* _ptr, size_t _index) noexcept;

    template<typename underlying_type>
      requires(meta::CharType<underlying_type> || meta::CppCharType<underlying_type>)
    constexpr const underlying_type* iterator_index_back(
        const underlying_type* _ptr, size_t _index) noexcept;

    /// @brief Returns the size in bytes of a NUL-terminated string
    /// @tparam T The char type
    /// @param start The string whose size in bytes to determine
    /// @return The size in bytes (not including NUL-terminator)
    template<typename T>
      requires(meta::CppCharType<T> || meta::CharType<T>)
    constexpr size_t unitlen(const T* start) noexcept;

    /// @brief Count the number of code points of a string of size 'count'.
    /// @tparam T The encoding char type
    /// @param start The start of the
    /// @param units The unit count
    /// @return The number of code points
    template<typename T>
      requires(meta::CppCharType<T> || meta::CharType<T>)
    constexpr size_t countlen(const T* start, size_t units) noexcept;

    /// @brief Returns the number of code points of a NUL-terminated string
    /// @tparam T The char type
    /// @param start The string whose length to determine
    /// @return The size in code points (not including NUL-terminator)
    template<typename T>
      requires(meta::CppCharType<T> || meta::CharType<T>)
    constexpr size_t strlen(const T* start) noexcept;

    struct LenInfo
    {
      size_t strlen;
      size_t unitlen;
    };

    /// @brief Returns the strlen and unitlen of a NUL terminated string.
    /// @tparam T The char type
    /// @param start The start of the NUL terminated string
    /// @return LenInfo representing the result
    template<typename T>
      requires(meta::CppCharType<T> || meta::CharType<T>)
    constexpr LenInfo len(const T* start) noexcept;

    namespace details
    {
      /// @brief Optimized strlen for UTF8
      /// @param ptr The NUL-terminated string whose code point count to return
      /// @return Return the number of code point (not including NUL-terminator)
      LenInfo len8(const char8_t* ptr) noexcept;
      /// @brief Optimized strlen for UTF16LE
      /// @param ptr The NUL-terminated string whose code point count to return
      /// @return Return the number of code point (not including NUL-terminator)
      LenInfo len16LE(const char16_t* ptr) noexcept;
      /// @brief Optimized strlen for UTF16BE
      /// @param ptr The NUL-terminated string whose code point count to return
      /// @return Return the number of code point (not including NUL-terminator)
      LenInfo len16BE(const char16_t* ptr) noexcept;
      /// @brief Optimized strlen for native UTF16
      /// @param ptr The NUL-terminated string whose code point count to return
      /// @return Return the number of code point (not including NUL-terminator)
      LenInfo len16(const char16_t* ptr) noexcept;
      /// @brief Optimized unitlen for UTF16.
      /// This works for both endianness as zero are represented the same
      /// way on both endianness.
      /// The implementation uses SIMD instructions.
      /// @param ptr The NUL-terminated string whose unit count to return
      /// @return Return the count of char16_t forming the string
      size_t unitlen16(const char16_t* ptr) noexcept;
      /// @brief Optimized unitlen for UTF32.
      /// This works for both endianness as zero are represented the same
      /// way on both endianness.
      /// The implementation uses SIMD instructions.
      /// @param ptr The NUL-terminated string whose unit count to return
      /// @return Return the count of char32_t forming the string
      size_t unitlen32(const char32_t* ptr) noexcept;
    } // namespace details

    /// @brief Iterator over Unicode encoded strings
    /// @tparam ENCODING The encoding
    template<StringEncoding ENCODING>
    class CodePointIterator
    {
      using ptr_t = meta::encoding_to_char_t<ENCODING>;

      const ptr_t* ptr;

    public:
      constexpr CodePointIterator(const ptr_t* ptr) noexcept
          : ptr(ptr)
      {
      }

      MAKE_DEFAULT_COPY_AND_MOVE_FOR(CodePointIterator);

      constexpr CodePointIterator& operator++() noexcept
      {
        if constexpr (meta::is_any_of<ptr_t, Char32BE, Char32LE, char>)
          ++ptr;
        if constexpr (meta::is_any_of<ptr_t, Char16BE, Char16LE>)
          ptr += ptr->sequence_length();
        if constexpr (meta::is_any_of<ptr_t, Char8>)
        {
          auto value = ptr->sequence_length();
          assert_true("Invalid UTF8!", value.is_value());
          ptr += *value;
        }
        return *this;
      }

      constexpr CodePointIterator operator++(int)
      {
        auto copy = *this;
        ++(*this);
        return copy;
      }

      constexpr CodePointIterator& operator--()
      {
        if constexpr (meta::is_any_of<ptr_t, Char32BE, Char32LE, char>)
          --ptr;
        if constexpr (meta::is_any_of<ptr_t, Char16BE, Char16LE>)
          ptr -= 1 + ptr->is_trail_surrogate();
        if constexpr (meta::is_any_of<ptr_t, Char8>)
        {
          --ptr;
          while (ptr->is_trail())
            --ptr;
        }
        return *this;
      }

      constexpr CodePointIterator operator--(int)
      {
        auto copy = *this;
        --(*this);
        return copy;
      }

      constexpr char32_t operator*() noexcept
      {
        if constexpr (meta::is_any_of<ptr_t, Char32BE, Char32LE, char>)
          return static_cast<char32_t>(*ptr);
        if constexpr (meta::is_any_of<ptr_t, Char16BE, Char16LE>)
        {
          char32_t result;
          auto check = unsafe_utf16to32(ptr, result);
          assert_true("Invalid UTF16!", check != ptr);
          return result;
        }
        if constexpr (meta::is_any_of<ptr_t, Char8>)
        {
          char32_t result;
          auto check = unsafe_utf8to32(ptr, result);
          assert_true("Invalid UTF8!", check != ptr);
          return result;
        }
      }

      friend constexpr auto operator<=>(
          const CodePointIterator&, const CodePointIterator&) noexcept = default;
    };

    /************************
    | IMPLEMENTATIONS vvv   |
    ************************/

    constexpr char16_t* unsafe_utf32to16(char32_t from, char16_t* result) noexcept
    {
      if (const u32 cp = from; cp < (u32)0x10000)
        *(result++) = static_cast<char16_t>(cp);
      else
      {
        *(result++) = (char16_t)(u16)(LEAD_OFFSET + (cp >> 10));
        *(result++) = (char16_t)(u16)(TRAIL_SURROGATE_MIN + (cp & 0x3FF));
      }
      return result;
    }

    constexpr const Char8* unsafe_utf8to32(
        const Char8* from, char32_t& result) noexcept
    {
      auto first = *from;
      auto value = first.sequence_length();
      if (value.is_none())
      {
        result = U'\ufffd';
        return from;
      }
      auto ret = static_cast<char32_t>(first);
      switch_no_default(*value)
      {
      case 1:
        break;
      case 2:
        ++from;
        ret = ((ret << 6) & 0x7ff) + ((*from) & 0x3f);
        break;
      case 3:
        ++from;
        ret = ((ret << 12) & 0xffff) + ((*from << 6) & 0xfff);
        ++from;
        ret += (*from) & 0x3f;
        break;
      case 4:
        ++from;
        ret = ((ret << 18) & 0x1fffff) + ((*from << 12) & 0x3ffff);
        ++from;
        ret += (*from << 6) & 0xfff;
        ++from;
        ret += (*from) & 0x3f;
        break;
      }
      result = ret;
      return from + 1;
    }

    template<meta::CharType Ty>
    constexpr ConvError to_utf8(
        const Ty*& from, size_t from_size, char8_t*& result,
        size_t result_size) noexcept
    {
      if constexpr (std::same_as<Ty, char>)
      {
        const auto min = math::min(from_size, result_size);
        if (std::is_constant_evaluated())
        {
          const auto max_from = from + min;
          while (from != max_from)
            *result++ = *from++;
        }
        else
        {
          std::memcpy(result, from, min);
          result += min;
          from += min;
        }
        return min < from_size * sizeof(Ty) ? ConvError::NOT_ENOUGH_SPACE
                                            : ConvError::NO_ERROR;
      }
      if constexpr (meta::is_any_of<Ty, Char32BE, Char32LE>)
      {
        const auto max_from   = from + from_size;
        const auto max_result = result + result_size;

        while (from != max_from)
        {
          const u32 as_host = from->as_host();
          if (as_host > CODE_POINT_MAX) [[unlikely]]
            return ConvError::INVALID_INPUT;

          if (const u32 cp = as_host; cp < 0x80) [[likely]]
          {
            if (result >= max_result)
              return ConvError::NOT_ENOUGH_SPACE;
            *(result++) = static_cast<char8_t>(cp);
          }
          else if (cp < 0x800) // 2 bytes
          {
            if (result + 1 >= max_result)
              return ConvError::NOT_ENOUGH_SPACE;
            *result++ = static_cast<char8_t>((cp >> 6) | 0xc0);
            *result++ = static_cast<char8_t>((cp & 0x3f) | 0x80);
          }
          else if (cp < 0x10000) // 3 bytes
          {
            if (result + 2 >= max_result)
              return ConvError::NOT_ENOUGH_SPACE;
            *result++ = static_cast<char8_t>((cp >> 12) | 0xe0);
            *result++ = static_cast<char8_t>(((cp >> 6) & 0x3f) | 0x80);
            *result++ = static_cast<char8_t>((cp & 0x3f) | 0x80);
          }
          else // 4 bytes
          {
            if (result + 3 >= max_result)
              return ConvError::NOT_ENOUGH_SPACE;
            *result++ = static_cast<char8_t>((cp >> 18) | 0xf0);
            *result++ = static_cast<char8_t>(((cp >> 12) & 0x3f) | 0x80);
            *result++ = static_cast<char8_t>(((cp >> 6) & 0x3f) | 0x80);
            *result++ = static_cast<char8_t>((cp & 0x3f) | 0x80);
          }
          from++;
        }
        return ConvError::NO_ERROR;
      }
      // TODO: add UTF16 support
    }

    template<typename T>
      requires(meta::is_any_of<T, Char16BE, Char16LE>)
    constexpr const T* unsafe_utf16to32(const T* from, char32_t& result) noexcept
    {
      auto first = *from;
      auto value = first.sequence_length();
      if (first.is_lead_surrogate()) [[unlikely]]
      {
        if (auto second = from[1]; second.is_trail_surrogate()) [[likely]]
        {
          result = surrogate_to_cp(first, second);
          return from + 2;
        }
        // lead surrogate must be followed by trail
        result = U'\ufffd';
        return from;
      }
      ++from;
      result = static_cast<char32_t>(first);
      return from;
    }

    template<meta::CharType underlying_type>
    constexpr char32_t index_front(
        const underlying_type* _ptr, size_t index) noexcept
    {
      auto ptr = iterator_index_front(_ptr, index);
      if constexpr (meta::is_any_of<underlying_type, Char32BE, Char32LE, char>)
        return static_cast<char32_t>(*ptr);
      if constexpr (meta::is_any_of<underlying_type, Char8>)
      {
        char32_t result;
        auto check = unsafe_utf8to32(ptr, result);
        assert_true("Invalid UTF8!", ptr != check);
        return result;
      }
      if constexpr (meta::is_any_of<underlying_type, Char16BE, Char16LE>)
      {
        char32_t result;
        auto check = unsafe_utf16to32(ptr, result);
        assert_true("Invalid UTF16!", ptr != check);
        return result;
      }
    }

    template<meta::CharType underlying_type>
    constexpr char32_t index_back(
        const underlying_type* _ptr, size_t _index) noexcept
    {
      auto ptr = iterator_index_back(_ptr, _index);
      if constexpr (meta::is_any_of<underlying_type, Char32BE, Char32LE, char>)
        return static_cast<char32_t>(*ptr);
      if constexpr (meta::is_any_of<underlying_type, Char8>)
      {
        char32_t result;
        auto check = unsafe_utf8to32(ptr, result);
        assert_true("Invalid UTF8!", ptr != check);
        return result;
      }
      if constexpr (meta::is_any_of<underlying_type, Char16BE, Char16LE>)
      {
        char32_t result;
        auto check = unsafe_utf16to32(ptr, result);
        assert_true("Invalid UTF16!", ptr != check);
        return result;
      }
    }

    template<typename underlying_type>
      requires(meta::CharType<underlying_type> || meta::CppCharType<underlying_type>)
    constexpr const underlying_type* iterator_index_front(
        const underlying_type* _ptr, size_t index) noexcept
    {
      if constexpr (meta::is_any_of<underlying_type, Char32BE, Char32LE>)
        return _ptr + index;
      else if constexpr (meta::is_any_of<underlying_type, char>)
        return _ptr + index;
      else
      {
        auto ptr = _ptr;
        while (index != 0)
        {
          auto len = uni::sequence_length(*ptr);
          --index;
          ptr += len;
        }
        return ptr;
      }
    }

    template<typename underlying_type>
      requires(meta::CharType<underlying_type> || meta::CppCharType<underlying_type>)
    constexpr const underlying_type* iterator_index_back(
        const underlying_type* _ptr, size_t _index) noexcept
    {
      // TODO: handle possible overflow when negating index
      if constexpr (meta::is_any_of<underlying_type, char, Char32BE, Char32LE>)
        return _ptr + -static_cast<i64>(_index);
      if constexpr (meta::is_any_of<underlying_type, Char8, char8_t>)
      {
        auto ptr = _ptr;
        while (_index != 0)
        {
          while (uni::is_trail(*ptr))
            --ptr;
          --ptr;
          --_index;
        }
        while (uni::is_trail(*ptr))
          --ptr;
        return ptr;
      }
      if constexpr (meta::is_any_of<underlying_type, Char16BE, Char16LE, char16_t>)
      {
        auto ptr = _ptr;
        while (_index != 0)
        {
          ptr -= 1 + uni::is_trail_surrogate(*ptr);
          --_index;
        }
        return ptr;
      }
    }

    template<typename T>
      requires(meta::CppCharType<T> || meta::CharType<T>)
    constexpr size_t unitlen(const T* start) noexcept
    {
      assert_true("Expected non-null pointer!", start != nullptr);
      if (std::is_constant_evaluated())
      {
        const T* end = start;
        while (*end != T{})
          ++end;
        return end - start;
      }
      else
      {
        if constexpr (std::same_as<char, T>)
          return std::strlen(start);
        if constexpr (meta::is_any_of<T, char8_t, Char8>)
          return std::strlen(reinterpret_cast<const char*>(start));
        if constexpr (meta::is_any_of<T, char16_t, Char16LE, Char16BE>)
          return details::unitlen16(reinterpret_cast<const char16_t*>(start));
        if constexpr (meta::is_any_of<T, char32_t, Char32LE, Char32BE>)
          return details::unitlen32(reinterpret_cast<const char32_t*>(start));
      }
    }

    template<typename T>
      requires(meta::CppCharType<T> || meta::CharType<T>)
    constexpr std::pair<size_t, size_t> count_and_middle(
        const T* start, size_t unit_len) noexcept
    {
      if constexpr (meta::is_any_of<T, char, char32_t, Char32BE, Char32LE>)
        return {unit_len, unit_len / 2};

      auto second = start + unit_len / 2;
      // Correct pointer
      if constexpr (meta::is_any_of<T, char8_t, Char8>)
      {
        while (is_trail(*second))
          ++second;
      }
      else
      {
        if (is_trail_surrogate(*second))
          ++second;
      }
      auto lhs                = uni::countlen(start, second - start);
      auto rhs                = uni::countlen(second, (start + unit_len) - second);
      const auto count_result = lhs + rhs;
      const auto half_count   = count_result / 2;
      if (lhs < half_count)
        second = iterator_index_front(second, half_count - lhs);
      else if (rhs < half_count)
        second = iterator_index_back(second, half_count - rhs);
      if (count_result % 2 == 0 && count_result != 0)
        second = iterator_index_back(second, 1);
      
      return {count_result, second - start};
    }

    template<typename T>
      requires(meta::CppCharType<T> || meta::CharType<T>)
    constexpr size_t countlen(const T* start, size_t unit_len) noexcept
    {
      assert_true("Expected non-null pointer!", start != nullptr);
      if constexpr (meta::is_any_of<T, char, char32_t, Char32BE, Char32LE>)
        return unit_len;
      if (std::is_constant_evaluated())
      {
        size_t result = 0;
        size_t index  = 0;
        // ^ this variable exist to avoid overflow with unit_len if decrementing.
        // Such overflow could happen if the last char16 was a lead surrogate
        while (index < unit_len)
        {
          size_t len = sequence_length(*start);
          index += len;
          start += len;
          ++result;
        }
        return result;
      }
      else if constexpr (std::same_as<T, Char16BE>)
        return simdutf::count_utf16be(ptr_to<const char16_t*>(start), unit_len);
      else if constexpr (std::same_as<T, Char16LE>)
        return simdutf::count_utf16le(ptr_to<const char16_t*>(start), unit_len);
      else if constexpr (std::same_as<T, char16_t>)
        return simdutf::count_utf16(ptr_to<const char16_t*>(start), unit_len);
      else if constexpr (std::same_as<T, char8_t>)
        return simdutf::count_utf8(reinterpret_cast<const char*>(start), unit_len);
      else if constexpr (std::same_as<T, Char8>)
        return simdutf::count_utf8(ptr_to<const char*>(start), unit_len);
    }

    template<typename T>
      requires(meta::CppCharType<T> || meta::CharType<T>)
    constexpr size_t strlen(const T* start) noexcept
    {
      return len(start).strlen;
    }
    
    template<typename T>
      requires(meta::CppCharType<T> || meta::CharType<T>)
    constexpr LenInfo len(const T* start) noexcept
    {
      assert_true("Expected non-null pointer!", start != nullptr);
      if constexpr (meta::is_any_of<T, char, char32_t, Char32BE, Char32LE>)
      {
        auto cache = uni::unitlen(start);
        return {cache, cache};
      }

      if constexpr (meta::is_any_of<T, char8_t, Char8>)
      {
        if (std::is_constant_evaluated())
        {
          const auto copy = start;
          size_t len   = 0;
          const T* end = start;
          char8_t current;
          while ((current = static_cast<char8_t>(*end)) != u8'\0')
          {
            end += sequence_length(current);
            ++len;
          }
          return {len, static_cast<size_t>(start - copy)};
        }
        else
          return details::len8(reinterpret_cast<const char8_t*>(start));
      }
      if constexpr (meta::is_any_of<T, char16_t, Char16BE, Char16LE>)
      {
        if (std::is_constant_evaluated())
        {
          const auto copy = start;
          size_t len   = 0;
          const T* end = start;
          char16_t current;
          while ((current = static_cast<char16_t>(*end)) != T{})
          {
            end += sequence_length(current);
            ++len;
          }
          return {len, static_cast<size_t>(start - copy)};
        }
        else
        {
          if constexpr (std::same_as<T, char16_t>)
            return details::len16(start);
          if constexpr (std::same_as<T, Char16BE>)
            return details::len16BE(reinterpret_cast<const char16_t*>(start));
          if constexpr (std::same_as<T, Char16LE>)
            return details::len16LE(reinterpret_cast<const char16_t*>(start));
        }
      }
    }
  } // namespace uni

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

#endif // !HG_DSA_UNICODE
