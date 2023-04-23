/** @file string.h
* Contains String class.
*/

#ifndef HG_COLT_STRING
#define HG_COLT_STRING

#include "./vector.h"
#include "./string_view.h"
#include "./expect.h"
#include "../refl/enum.h"
#include "../str/parse.h"

DECLARE_ENUM_WITH_TYPE(u8, clt::io, IOError, FILE_EOF, FILE_ERROR, INVALID_FMT, OUT_OF_RANGE);

namespace clt
{
  template<auto ALLOCATOR, StringEncoding ENCODING>
    requires meta::AllocatorScope<ALLOCATOR>
  /// @brief Unspecialized BasicString
  class BasicString{};

  template<auto ALLOCATOR>
    requires meta::AllocatorScope<ALLOCATOR>
  /// @brief ASCII BasicString
  class BasicString<ALLOCATOR, StringEncoding::ASCII>
    : public Vector<char, ALLOCATOR>
  {
    /// @brief The underlying vector providing storage
    using UnderlyingVector = Vector<char, ALLOCATOR>;

    /// @brief True if the allocator is global
    static constexpr bool is_global = meta::GlobalAllocator<ALLOCATOR>;
    /// @brief True if the allocator is local (we need a reference to it)
    static constexpr bool is_local = meta::LocalAllocator<ALLOCATOR>;

  public:
    template<typename AllocT> requires is_local
    /// @brief Constructor for BasicString using a local allocator
    /// @param alloc The local allocator
    constexpr BasicString(AllocT& alloc) noexcept
      : UnderlyingVector(alloc) {}

    /// @brief Default constructor for BasicString using a global allocator
    constexpr BasicString() noexcept requires is_global = default;

    template<typename AllocT>
    /// @brief Constructor for BasicString using a local allocator
    /// @param alloc The local allocator
    /// @param strv The StringView to use whose content to copy
    constexpr BasicString(AllocT& alloc, StringView strv) noexcept requires is_local
      : UnderlyingVector(alloc, strv) {}

    /// @brief Constructor for BasicString using a global allocator
    /// @param strv The StringView to use whose content to copy
    constexpr BasicString(StringView strv) noexcept requires is_global
      : UnderlyingVector(strv) {}

    template<typename AllocT, size_t N> requires is_local
    constexpr BasicString(AllocT& alloc, const char(&x)[N]) noexcept
      : UnderlyingVector(alloc, StringView{ x, x + N }) {}

    template<size_t N> requires is_global
    constexpr BasicString(const char(&x)[N]) noexcept
      : UnderlyingVector(StringView{ x, x + N }) {}

    /// @brief Converts a String to a StringView
    /// @return Span over the whole Vector
    constexpr operator StringView() noexcept
    {
      return StringView{ this->begin(), this->end() };
    }

    /// @brief Gets a line from a file.
    /// The resulting BasicString is not NUL terminated, and does not contain the new line.
    /// FILE_EOF and FILE_ERROR is only returned if no characters were read.
    /// @param from The (opened) file from which to read characters
    /// @param reserve The count of characters to reserve before reading characters
    /// @param strip_front If true, skips all blank (' ', '\t') characters in the front of the string
    /// @return BasicString containing the line or either FILE_EOF or FILE_ERROR.
    static Expect<BasicString, io::IOError> getLine(FILE* from, u64 reserve = 64, bool strip_front = true) noexcept
    {
      BasicString str;
      auto gchar = std::fgetc(from);
      if (gchar == EOF)
      {
        if (feof(from))
          return { Error, io::IOError::FILE_EOF };
        else
          return { Error, io::IOError::FILE_ERROR };
      }
      str.reserve(reserve);
      if (strip_front && std::isspace(gchar))
      {
        //Consume spaces
        while ((gchar = std::fgetc(from)) != EOF)
          if (!clt::isblank(gchar))
            break;
      }
      for (;;)
      {
        if (gchar != '\n' && gchar != EOF)
        {
          str.push_back(static_cast<char>(gchar));
          gchar = std::fgetc(from);
        }
        else
          break;
      }
      return str;
    }

    /// @brief Gets a line from a 'stdin'.
    /// The resulting BasicString is not NUL terminated, and does not contain the new line.
    /// FILE_EOF and FILE_ERROR is only returned if no characters were read.
    /// @param reserve The count of characters to reserve before reading characters
    /// @param strip_front If true, skips all blank (' ', '\t') characters in the front of the string
    /// @return BasicString containing the line or either FILE_EOF or FILE_ERROR.
    static Expect<BasicString, io::IOError> getLine(u64 reserve = 64, bool strip_front = true) noexcept
    {
      return getLine(stdin, reserve, strip_front);
    }
  };

  /// @brief ASCII String
  using String = BasicString<mem::GlobalAllocatorDescription, StringEncoding::ASCII>;

  template<>
  /// @brief Overload for String
  struct str::parser<String>
    : str::Recommended<128>
  {
    constexpr ParseResult operator()(maybe_out<String> str, StringView to_parse) const noexcept
    {
      str.construct(to_parse);
      return ParseResult{ to_parse.end(), ParseErrorCode::SUCCESS };
    }
  };
}

template<>
struct fmt::formatter<clt::String>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    auto end = ctx.end();
    assert_true("Possible format for StringView is: {}!", it == end);
    return it;
  }

  template<typename FormatContext>
  auto format(const clt::String& str, FormatContext& ctx)
  {
    return fmt::format_to(ctx.out(), "{:.{}}", str.data(), str.size());
  }
};

#endif //!HG_COLT_STRING