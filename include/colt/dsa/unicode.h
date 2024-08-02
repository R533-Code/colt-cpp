#ifndef HG_DSA_UNICODE
#define HG_DSA_UNICODE

#include <utf8/checked.h>
#include <utf8/unchecked.h>
#include <cstring>
#include <utility>

#include "colt/num/typedefs.h"
#include "colt/macro/assert.h"
#include "colt/macro/on_scope_exit.h"
#include "colt/bit/endian.h"

namespace clt
{
  /// @brief Returns the length of a NUL-terminated string
  /// @param start The string whose length to determine
  /// @return The length (not including NUL-terminator)
  constexpr size_t strlen(const char* start) noexcept
  {
    assert_true("Expected non-null pointer!", start != nullptr);
    if (std::is_constant_evaluated())
    {
      const char* end = start;
      while (*end != '\0')
        ++end;
      return end - start;
    }
    return std::strlen(start);
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

  constexpr char32 clt::char32be::as_host() const noexcept
  {
    return (char32_t)bit::btoh((u32)value());
  }

  constexpr char32le clt::char32be::as_little() const noexcept
  {
    return (char32_t)bit::byteswap((u32)value());
  }

  constexpr char32be clt::char32be::as_big() const noexcept
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

  constexpr char32 clt::char32le::as_host() const noexcept
  {
    return (char32_t)bit::ltoh((u32)value());
  }

  constexpr char32le clt::char32le::as_little() const noexcept
  {
    return value();
  }

  constexpr char32be clt::char32le::as_big() const noexcept
  {
    return (char32_t)bit::byteswap((u32)value());     
  }
}

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
    char32_t array32[1] = {op.as_host().value()};
    // 4 char max for utf-8 + NUL terminator
    char array8[5]      = {0, 0, 0, 0, 0};
    utf8::utf32to8(&array32[0], &array32[1], &array8[0]);
    return fmt::format_to(ctx.out(), "{}", array8);
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
    char32_t array32[1] = {op.as_host().value()};
    // 4 char max for utf-8 + NUL terminator
    char array8[5]      = {0, 0, 0, 0, 0};
    utf8::utf32to8(&array32[0], &array32[1], &array8[0]);
    return fmt::format_to(ctx.out(), "{}", array8);
  }
};

#endif // !HG_DSA_UNICODE
