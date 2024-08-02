#ifndef HG_DSA_UNICODE
#define HG_DSA_UNICODE

#include <uni_algo/all.h>
#include <cstring>
#include <utility>

#include "colt/num/typedefs.h"
#include "colt/meta/traits.h"
#include "colt/macro/assert.h"
#include "colt/macro/on_scope_exit.h"
#include "colt/bit/endian.h"

namespace clt
{
  namespace meta
  {
    /// @brief Represents any of the built-in char types
    template<typename T>
    concept CharType = meta::is_any_of<T, char, char8_t, char16_t, char32_t>;
  }

  /// @brief Represents the possible character encoding
  enum class StringEncoding : u8
  {
    /// @brief ASCII
    ASCII,
    /// @brief UTF8
    UTF8,
    /// @brief UTF16 Big Endian
    UTF16BE,
    /// @brief UTF16 Little Endian
    UTF16LE,
    /// @brief UTF32 Big Endian
    UTF32BE,
    /// @brief UTF32 Little Endian
    UTF32LE
  };

  class char32be;
  class char32le;

#ifdef COLT_LITTLE_ENDIAN
  /// @brief Represents a 32-bit char with the host endianness
  using char32 = char32le;
#elif defined(COLT_BIG_ENDIAN)
  /// @brief Represents a 32-bit char with the host endianness
  using char32 = char32be;
#else
  #error "Unknown endianness!"
#endif

  /// @brief Big Endian 32-bit Char
  class char32be
  {
    /// @brief The value (always stored as Big Endian)
    char32_t _value;

  public:
    /// @brief Constructs a big endian value from a little endian value
    /// @param value The value (in little endian)
    constexpr char32be(char32le value) noexcept;
    /// @brief Constructs a big endian value from host endianness
    /// @param value The value (in host endianness)
    constexpr char32be(char32_t value) noexcept;

    MAKE_DEFAULT_COPY_AND_MOVE_FOR(char32be);

    /// @brief Return the current value as a host char32.
    /// If the host is big endian, this is a no-op.
    /// @return The current value as a host char32
    constexpr char32 as_host() const noexcept;
    /// @brief Return the current value as a little endian char32.
    /// @return The current value as a little endian char32
    constexpr char32le as_little() const noexcept;
    /// @brief Return the current value as a big endian char32.
    /// This is a no-op.
    /// @return The current value as a big endian char32
    constexpr char32be as_big() const noexcept;

    /// @brief Returns the value in big endian
    /// @return Value in big endian
    constexpr char32_t value() const noexcept { return _value; }
  };

  /// @brief Little Endian 32-bit Char
  class char32le
  {
    char32_t _value;

  public:
    /// @brief Constructs a little endian value from a big endian value
    /// @param value The value (in little endian)
    constexpr char32le(char32be value) noexcept;
    /// @brief Constructs a little endian value from host endianness
    /// @param value The value (in host endianness)
    constexpr char32le(char32_t value) noexcept;

    MAKE_DEFAULT_COPY_AND_MOVE_FOR(char32le);

    /// @brief Return the current value as a host char32.
    /// If the host is little endian, this is a no-op.
    /// @return The current value as a host char32
    constexpr char32 as_host() const noexcept;
    /// @brief Return the current value as a little endian char32.
    /// This is a no-op.
    /// @return The current value as a little endian char32
    constexpr char32le as_little() const noexcept;
    /// @brief Return the current value as a big endian char32.
    /// @return The current value as a big endian char32
    constexpr char32be as_big() const noexcept;

    /// @brief Returns the value in little endian
    /// @return Value in little endian
    constexpr char32_t value() const noexcept { return _value; }
  };

  constexpr char32be::char32be(char32_t value) noexcept
      : _value((char32_t)bit::htob((u32)value))
  {
  }

  constexpr char32be::char32be(char32le value) noexcept
      : _value((char32_t)bit::byteswap((u32)value.value()))
  {
  }

  constexpr char32 char32be::as_host() const noexcept
  {
    return (char32_t)bit::btoh((u32)value());
  }

  constexpr char32le char32be::as_little() const noexcept
  {
    return (char32_t)bit::byteswap((u32)value());
  }

  constexpr char32be char32be::as_big() const noexcept
  {
    return value();
  }

  // ^^^ BIG ENDIAN
  // vvv LITTLE ENDIAN

  constexpr char32le::char32le(char32_t value) noexcept
      : _value((char32_t)bit::htol((u32)value))
  {
  }

  constexpr char32le::char32le(char32be value) noexcept
      : _value((char32_t)bit::byteswap((u32)value.value()))
  {
  }

  constexpr char32 char32le::as_host() const noexcept
  {
    return (char32_t)bit::ltoh((u32)value());
  }

  constexpr char32le char32le::as_little() const noexcept
  {
    return value();
  }

  constexpr char32be char32le::as_big() const noexcept
  {
    return (char32_t)bit::byteswap((u32)value());
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
    static constexpr const char32_t SURROGATE_OFFSET = 0xfca02400u;
    /// @brief The maximum code point that can be stored (inclusive)
    static constexpr char32_t CODE_POINT_MAX = 0x0010ffffu;

    /// @brief Check if a UTF8 byte is a trailing byte
    /// @param oc The byte
    /// @return True if trailing byte
    constexpr bool is_trail(char8_t oc)
    {
      return ((0xff & oc) >> 6) == 0x2;
    }

    /// @brief Check if a code point is a lead surrogate
    /// @param cp The code point
    /// @return True if 'cp' is a lead surrogate
    constexpr bool is_lead_surrogate(char16_t cp)
    {
      return (cp >= LEAD_SURROGATE_MIN && cp <= LEAD_SURROGATE_MAX);
    }

    /// @brief Check if a code point is a trail surrogate
    /// @param cp The code point
    /// @return True if 'cp' is a trail surrogate
    constexpr bool is_trail_surrogate(char32_t cp)
    {
      return (cp >= TRAIL_SURROGATE_MIN && cp <= TRAIL_SURROGATE_MAX);
    }

    /// @brief Check if a code point is in the Basic Multilingual Plane
    /// @param cp The code point
    /// @return True if 'cp' is in the BMP
    constexpr bool is_in_bmp(char32_t cp)
    {
      return cp < char32_t(0x10000);
    }
    
    /// @brief Returns the size that a code point would occupy as UTF16
    /// @param cp The code point
    /// @return The size in byte
    constexpr size_t cp_size_utf16(char32_t cp) noexcept
    {
      return ((size_t)!is_in_bmp(cp) + 1) * 2;
    }

    /// @brief Returns the size that a code point would occupy as UTF8
    /// @param cp The code point
    /// @return The size in byte
    constexpr size_t cp_size_utf8(char32_t cp) noexcept
    {
      if (cp < 0x80)
        return 1;
      else if (cp < 0x800)
        return 2;
      else if (cp < 0x10000)
        return 3;
      else
        return 4;
    }    

    /// @brief Converts a code point in host endianness to a UTF16BE sequence
    /// @param from The code point to convert
    /// @param result Pointer to write to
    /// @warning 'result' must have at least 2 16-bit integers of capacity
    /// @return Pointer to after the last written character
    constexpr char16_t* unsafe_utf32to16le(char32_t from, char16_t* result) noexcept
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
    
    /// @brief Converts a code point in host endianness to a UTF8
    /// @param from The code point to convert
    /// @param result Pointer to write to
    /// @warning 'result' must have at least 4 bytes of capacity
    /// @return Pointer to after the last written character
    constexpr char8_t* unsafe_utf32to8(char32_t from, char8_t* result) noexcept
    {
      // single byte
      if (const u32 cp = from; cp < 0x80)
        *(result++) = static_cast<char8_t>(cp);
      // 2 bytes
      else if (cp < 0x800)
      {
        *(result++) = static_cast<char8_t>((cp >> 6) | 0xc0);
        *(result++) = static_cast<char8_t>((cp & 0x3f) | 0x80);
      }
      // 3 bytes
      else if (cp < 0x10000)
      {
        *(result++) = static_cast<char8_t>((cp >> 12) | 0xe0);
        *(result++) = static_cast<char8_t>(((cp >> 6) & 0x3f) | 0x80);
        *(result++) = static_cast<char8_t>((cp & 0x3f) | 0x80);
      }
      // 4 bytes
      else
      {
        *(result++) = static_cast<char8_t>((cp >> 18) | 0xf0);
        *(result++) = static_cast<char8_t>(((cp >> 12) & 0x3f) | 0x80);
        *(result++) = static_cast<char8_t>(((cp >> 6) & 0x3f) | 0x80);
        *(result++) = static_cast<char8_t>((cp & 0x3f) | 0x80);
      }
      return result;
    }
  }

  /// @brief Returns the size in bytes of a NUL-terminated string
  /// @tparam T The char type
  /// @param start The string whose size in bytes to determine
  /// @return The size in bytes (not including NUL-terminator)
  template<meta::CharType T>
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
  template<meta::CharType T>
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
} // namespace clt

template<>
struct fmt::formatter<clt::char32le>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(clt::char32le op, FormatContext& ctx) const
  {
    // 4 char max for utf-8 + NUL terminator
    char8_t array8[5] = {0, 0, 0, 0, 0};
    clt::uni::unsafe_utf32to8(op.as_host().value(), array8);
    return fmt::format_to(ctx.out(), "{}", reinterpret_cast<const char*>(array8));
  }
};

template<>
struct fmt::formatter<clt::char32be>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(clt::char32be op, FormatContext& ctx) const
  {
    // 4 char max for utf-8 + NUL terminator
    char8_t array8[5] = {0, 0, 0, 0, 0};
    clt::uni::unsafe_utf32to8(op.as_host().value(), array8);
    return fmt::format_to(ctx.out(), "{}", reinterpret_cast<const char*>(array8));
  }
};

#endif // !HG_DSA_UNICODE
