#ifndef HG_DSA_UNICODE
#define HG_DSA_UNICODE

#include <uni_algo/all.h>
#include <cstring>
#include <utility>
#include <ranges>

#include "colt/num/typedefs.h"
#include "colt/meta/traits.h"
#include "colt/macro/assert.h"
#include "colt/macro/on_scope_exit.h"
#include "colt/bit/endian.h"
#include "colt/math/math.h"
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

    constexpr bool is_lead_surrogate(char16_t value) noexcept
    {
      return value >= LEAD_SURROGATE_MIN && value <= LEAD_SURROGATE_MAX;
    }
    
    constexpr bool is_trail_surrogate(char16_t value) noexcept
    {
      return value >= TRAIL_SURROGATE_MIN && value <= TRAIL_SURROGATE_MAX;
    }

    /// @brief Check if a code point is in the Basic Multilingual Plane
    /// @param cp The code point
    /// @return True if 'cp' is in the BMP
    constexpr bool is_in_bmp(char32_t cp)
    {
      return cp < char32_t(0x10000);
    }

    /*
    /// @brief Check if a code point is in the Basic Multilingual Plane
    /// @param cp The code point
    /// @return True if 'cp' is in the BMP
    constexpr bool is_in_bmp(CodePoint cp)
    {
      return cp.value() < char32_t(0x10000);
    }

    template<bool SAFE = true>
    constexpr std::conditional_t<SAFE, Option<u8>, u8> size_as_utf16(
        CodePoint cp) noexcept
    {
      if constexpr (SAFE)
        if (cp.value() > CODE_POINT_MAX)
          return None;
      return ((u8)!is_in_bmp(cp.value()) + 1) * 2;
    }

    template<bool SAFE = true>
    constexpr std::conditional_t<SAFE, Option<u8>, u8> size_as_utf8(
        CodePoint cp) noexcept
    {
      if (cp.value() < 0x80)
        return 1;
      else if (cp.value() < 0x800)
        return 2;
      else if (cp.value() < 0x10000)
        return 3;
      if constexpr (!SAFE)
        return 4;
      else
      {
        if (cp <= CODE_POINT_MAX)
          return 4;
        return None;
      }
    }
*/    
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
    constexpr operator char8_t() const noexcept
    {
      return _value;
    }

    /// @brief Returns the value
    /// @return The value of the Char8
    constexpr char8_t value() const noexcept
    {
      return _value;
    }

    /// @brief Check if the current char represents a trailing UTF8.
    /// A trailing UTF8 is any char that is NOT the beginning of a UTF8
    /// sequence. An invalid UTF8 char cannot be trail.
    /// @return True if not the beginning of a UTF8 sequence
    constexpr bool is_trail() const noexcept
    {
      return (_value >> 6) == 0b10;
    }

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
    constexpr std::conditional_t<SAFE, Option<u8>, u8> sequence_length() const noexcept
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
    static constexpr size_t max_sequence     = 1;

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
    constexpr Char32BE as_big() const noexcept
    {
      return *this;
    }

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
    constexpr Char32LE as_little() const noexcept
    {
      return *this;
    }
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
    constexpr u8 sequence_length() const noexcept { return 1 + (u8)is_lead_surrogate(); }
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
    constexpr u8 sequence_length() const noexcept { return 1 + (u8)is_lead_surrogate(); }
  };

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

  namespace meta
  {
    /// @brief Represents any of the Colt char types
    template<typename T>
    concept CharType = meta::is_any_of<T, char, Char8, Char16LE, Char16BE, Char32LE, Char32BE>;

    /// @brief Represents any of the C++ char types
    template<typename T>
    concept CppCharType = meta::is_any_of<T, char, char8_t, char16_t, char32_t>;

    /// @brief Returns "" with the right char type
    /// @tparam T The char type
    /// @return "" or u8"" or u"" or U""
    template<CharType T>
    consteval auto empty_string_literal() noexcept
    {
      if constexpr (std::same_as<T, char>)
        return "";
      if constexpr (std::same_as<T, Char8>)
        return u8"";
      if constexpr (meta::is_any_of<T, Char16LE, Char16BE>)
        return u"";
      if constexpr (meta::is_any_of<T, Char32BE, Char32LE>)
        return U"";
    }

    /// @brief Converts an encoding to the char that must represent it
    /// @tparam ENCODING The encoding to convert
    template<StringEncoding ENCODING>
    using encoding_to_char_t = decltype([]()
      {
        using enum clt::StringEncoding;
        if constexpr (ENCODING == ASCII)
          return char{};
        if constexpr (ENCODING == UTF8)
          return Char8{};
        if constexpr (ENCODING == UTF16BE)
          return Char16BE{};
        if constexpr (ENCODING == UTF16LE)
          return Char16LE{};
        if constexpr (ENCODING == UTF32BE)
          return Char32BE{};
        if constexpr (ENCODING == UTF32LE)
          return Char32LE{};
      }());

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

  /// @brief Returns the size in bytes of a NUL-terminated string
  /// @tparam T The char type
  /// @param start The string whose size in bytes to determine
  /// @return The size in bytes (not including NUL-terminator)
  template<meta::CppCharType T>
  constexpr size_t bytelen(const T* start) noexcept
  {
    assert_true("Expected non-null pointer!", start != nullptr);
    if constexpr (meta::is_any_of<T, char, char8_t>)
    {
      if (std::is_constant_evaluated())
      {
        const T* end = start;
        while (*end != T{0})
          ++end;
        return end - start;
      }
      else
      {
        if constexpr (std::same_as<char, T>)
          return std::strlen(start);
        return std::strlen(reinterpret_cast<const char*>(start));
      }
    }
    const T* end = start;
    while (*end != T{0})
      ++end;
    return (end - start) * sizeof(T);
  }

  /// @brief Returns the number of code points of a NUL-terminated string
  /// @tparam T The char type
  /// @param start The string whose length to determine
  /// @return The size in code points (not including NUL-terminator)
  template<meta::CppCharType T>
  constexpr size_t strlen(const T* start) noexcept
  {
    assert_true("Expected non-null pointer!", start != nullptr);
    if constexpr (meta::is_any_of<T, char, char32_t>)
      return clt::bytelen(start) / sizeof(T);

    if constexpr (std::same_as<T, char8_t>)
    {
      size_t len   = 0;
      const T* end = start;
      char8_t current;
      while ((current = *end) != T{0})
      {
        if (current < 128)
          end += 1;
        else if ((current & 0xE0) == 0xC0)
          end += 2;
        else if ((current & 0xF0) == 0xE0)
          end += 3;
        else if ((current & 0xF8) == 0xF0)
          end += 4;
        ++len;
      }
      return len;
    }
    if constexpr (std::same_as<T, char16_t>)
    {
      size_t len   = 0;
      const T* end = start;
      char16_t current;
      while ((current = *end) != T{0})
      {
        if (uni::is_lead_surrogate(current))
          end += 2;
        else
          end += 1;
        ++len;
      }
      return len;
    }
  }

  namespace uni
  {
    enum class ConvError
    {
      NO_ERROR,
      NOT_ENOUGH_SPACE,
      INVALID_INPUT,
    };

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
        const auto min = math::min(from_size * sizeof(Ty), result_size);
        const auto max_from   = from + (min / sizeof(Ty));
        const auto max_result = result + min;

        // TODO: second pass because there might still be some storage in the buffer.
        while (from != max_from)
        {
          const u32 as_host = from->as_host();
          if (as_host > CODE_POINT_MAX) [[unlikely]]
            return ConvError::INVALID_INPUT;
          from++;

          if (const u32 cp = as_host; cp < 0x80) [[likely]]
            *(result++) = static_cast<char8_t>(cp);          
          else if (cp < 0x800) // 2 bytes
          {
            *result++ = static_cast<char8_t>((cp >> 6) | 0xc0);
            *result++ = static_cast<char8_t>((cp & 0x3f) | 0x80);
          }          
          else if (cp < 0x10000) // 3 bytes
          {
            *result++ = static_cast<char8_t>((cp >> 12) | 0xe0);
            *result++ = static_cast<char8_t>(((cp >> 6) & 0x3f) | 0x80);
            *result++ = static_cast<char8_t>((cp & 0x3f) | 0x80);
          }          
          else // 4 bytes
          {
            *result++ = static_cast<char8_t>((cp >> 18) | 0xf0);
            *result++ = static_cast<char8_t>(((cp >> 12) & 0x3f) | 0x80);
            *result++ = static_cast<char8_t>(((cp >> 6) & 0x3f) | 0x80);
            *result++ = static_cast<char8_t>((cp & 0x3f) | 0x80);
          }
        }
        return min < from_size * sizeof(Ty) ? ConvError::NOT_ENOUGH_SPACE
                                          : ConvError::NO_ERROR;
      }
      // TODO: add UTF16 support
    }

    /// @brief Converts a code point in host endianness to a UTF16BE sequence
    /// @param from The code point to convert
    /// @param result Pointer to write to
    /// @warning 'result' must have at least 2 16-bit integers of capacity
    /// @return Pointer to after the last written character
    template<bool BIG_OR_LITTLE>
    constexpr char16_t* utf32to16(char32_t from, char16_t* result) noexcept
    {
      if (const u32 cp = from; is_in_bmp(from))
        *(result++) = static_cast<char16_t>(cp);
      else
      {
        *(result++) = (char16_t)bit::htol((u16)(LEAD_OFFSET + (cp >> 10)));
        *(result++) = (char16_t)bit::htol((u16)(TRAIL_SURROGATE_MIN + (cp & 0x3FF)));
      }
      return result;
    }

    /// @brief Converts a code point in host endianness to a UTF16BE sequence
    /// @param from The code point to convert
    /// @param result Pointer to write to
    /// @warning 'result' must have at least 2 16-bit integers of capacity
    /// @return Pointer to after the last written character
    constexpr char16_t* unsafe_utf32to16be(char32_t from, char16_t* result) noexcept
    {
      if (const u32 cp = from; cp < (u32)0x10000)
        *(result++) = static_cast<char16_t>(cp);
      else
      {
        *(result++) = (char16_t)bit::htob((u16)(LEAD_OFFSET + (cp >> 10)));
        *(result++) = (char16_t)bit::htob((u16)(TRAIL_SURROGATE_MIN + (cp & 0x3FF)));
      }
      return result;
    }

    //constexpr char8_t* unsafe_utf8to32be(char8_t* from, char32_t* result) noexcept
    //{
    //  auto first = *from;
    //  auto ret   = static_cast<char32_t>(first);
    //  switch_no_default(unsafe_sequence_length(first))
    //  {
    //  case 1:
    //    break;
    //  case 2:
    //    ++from;
    //    ret = ((ret << 6) & 0x7ff) + ((*from) & 0x3f);
    //    break;
    //  case 3:
    //    ++from;
    //    ret = ((ret << 12) & 0xffff) + ((*from << 6) & 0xfff);
    //    ++from;
    //    ret += (*from) & 0x3f;
    //    break;
    //  case 4:
    //    ++from;
    //    ret = ((ret << 18) & 0x1fffff) + ((*from << 12) & 0x3ffff);
    //    ++from;
    //    ret += (*from << 6) & 0xfff;
    //    ++from;
    //    ret += (*from) & 0x3f;
    //    break;
    //  }
    //  ++from;
    //  *result = bit::htol(ret);
    //  return from;
    //}

    ///// @brief Converts a code point in host endianness to a UTF8
    ///// @param from The code point to convert
    ///// @param result Pointer to write to
    ///// @warning 'result' must have at least 4 bytes of capacity
    ///// @return Pointer to after the last written character
    //constexpr char8_t* unsafe_utf32to8(char32_t from, char8_t* result) noexcept
    //{
    //  // single byte
    //  if (const u32 cp = from; cp < 0x80)
    //    *(result++) = static_cast<char8_t>(cp);
    //  // 2 bytes
    //  else if (cp < 0x800)
    //  {
    //    *(result++) = static_cast<char8_t>((cp >> 6) | 0xc0);
    //    *(result++) = static_cast<char8_t>((cp & 0x3f) | 0x80);
    //  }
    //  // 3 bytes
    //  else if (cp < 0x10000)
    //  {
    //    *(result++) = static_cast<char8_t>((cp >> 12) | 0xe0);
    //    *(result++) = static_cast<char8_t>(((cp >> 6) & 0x3f) | 0x80);
    //    *(result++) = static_cast<char8_t>((cp & 0x3f) | 0x80);
    //  }
    //  // 4 bytes
    //  else
    //  {
    //    *(result++) = static_cast<char8_t>((cp >> 18) | 0xf0);
    //    *(result++) = static_cast<char8_t>(((cp >> 12) & 0x3f) | 0x80);
    //    *(result++) = static_cast<char8_t>(((cp >> 6) & 0x3f) | 0x80);
    //    *(result++) = static_cast<char8_t>((cp & 0x3f) | 0x80);
    //  }
    //  return result;
    //}
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
  auto format(Ty op, FormatContext& ctx) const
  {
    using namespace clt::uni;

    // 4 char max for utf-8 + NUL terminator
    char8_t array8[5] = { 0 };
    auto from         = &op;
    auto result       = &array8[0];
    if (to_utf8(from, 1, result, 5) == ConvError::NO_ERROR)
      return fmt::format_to(ctx.out(), "{}", reinterpret_cast<const char*>(array8));
    FMT_THROW(std::runtime_error("invalid unicode"));
  }
};

#endif // !HG_DSA_UNICODE
