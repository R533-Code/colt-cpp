#ifndef HG_COLT_STRING_VIEW
#define HG_COLT_STRING_VIEW

#include <type_traits>

#include "./span.h"
#include "../str/distance.h"
#include "../str/ascii.h"

namespace clt
{
  /// @brief Possible String encoding provided by the library
  enum class StringEncoding
  {
    ASCII, UTF8, UTF32
  };

  template<StringEncoding>
  /// @brief Unspecialized BasicStringView
  class BasicStringView {};

  template<>
  /// @brief ASCII StringView
  class BasicStringView<StringEncoding::ASCII>
    : public View<char>
  {
    /// @brief char
    using CharT = char;

    /// @brief Helper type
    using ViewT = View<CharT>;

  public:
    /// @brief Constructs an empty BasicStringView
    constexpr BasicStringView() noexcept
      : ViewT(nullptr, nullptr) {}
    
    template<size_t N>
    constexpr BasicStringView(const CharT(&x)[N]) noexcept
      : ViewT(x, x + N) {}
    /// @brief Range constructor
    /// @param begin The beginning of the view
    /// @param end The end of the view
    constexpr BasicStringView(const CharT* begin, const CharT* end) noexcept
      : ViewT(begin, end) {}
    /// @brief Constructs a StringView over a NUL terminated string
    /// @param cstr The NUL terminated string to span over
    constexpr BasicStringView(const CharT* cstr) noexcept
      : ViewT(cstr, clt::strlen(cstr)) {}
    /// @brief Constructs a StringView over a NUL terminated string, including its NUL terminator
    /// @param cstr The NUL terminated string to span over
    /// @param  Tag object (WithNUL)
    constexpr BasicStringView(const CharT* cstr, meta::WithNULT) noexcept
      : ViewT(cstr, clt::strlen(cstr) + 1) {}
    /// @brief Copy constructor
    /// @param  The StringView to copy
    constexpr BasicStringView(const BasicStringView&) noexcept = default;
    /// @brief Move constructor
    /// @param  The StringView to move
    constexpr BasicStringView(BasicStringView&&) noexcept = default;

    constexpr BasicStringView& operator=(const BasicStringView&) noexcept = default;
    constexpr BasicStringView& operator=(BasicStringView&&) noexcept = default;

    /// @brief Strips whitespace from the beginning of the string 
    /// @return Self
    constexpr BasicStringView& strip_prefix() noexcept
    {
      while (!ViewT::is_empty())
        if (clt::isspace(*ViewT::begin()))
          ViewT::pop_front();
        else
          break;

      return *this;
    }
    
    /// @brief Strips whitespace from the end of the string 
    /// @return Self
    constexpr BasicStringView& strip_suffix() noexcept
    {
      while (!ViewT::is_empty())
        if (clt::isspace(*(ViewT::begin() + ViewT::size() - 1)))
          ViewT::pop_back();
        else
          break;
      return *this;
    }

    /// @brief Pops all spaces from the beginning and the end of the StringView.
    /// The characters that are considered spaces are '\n', ' ', '\v', '\t'.
    /// @return Self
    constexpr BasicStringView& strip() noexcept
    {
      return strip_prefix(), strip_suffix();
    }

    /// @brief Value that signifies that the value was not found
    static constexpr size_t npos = std::numeric_limits<size_t>::max();

    /// @brief Finds first occurrence of 'chr' and returns its index, or npos if not found
    /// @param chr The character to search for
    /// @param offset The offset from which to begin to search
    /// @return The index of 'chr' or npos if not found
    constexpr size_t find(CharT chr, size_t offset = 0) const noexcept
    {
      for (size_t i = offset; i < this->size(); i++)
      {
        if (this->data()[i] == chr)
          return i;
      }
      return npos;
    }

    constexpr BasicStringView substr(size_t pos = 0, size_t count = npos) const noexcept
      COLT_PRE(pos <= size())
    {
      return BasicStringView{ ViewT::data() + pos,
        ViewT::data() + pos + clt::min(ViewT::size() - pos, count) };
    }
    COLT_POST()

    /// @brief Check if the StringView begins with 'chr'
    /// @param chr The character to check for
    /// @return True if the current StringView begins with 'chr'
    constexpr bool begins_with(CharT chr) const noexcept
    {
      if (this->is_empty())
        return false;
      return this->front() == chr;
    }

    /// @brief Check if the StringView begins with another StringView
    /// @param str The StringView to search for
    /// @return True if the current StringView begins with 'str'
    constexpr bool begins_with(BasicStringView str) const noexcept
    {
      if (str.size() > this->size())
        return false;
      for (size_t i = 0; i < str.size(); i++)
      {
        if (str[i] != (*this)[i])
          return false;
      }
      return true;
    }
    
    /// @brief Uses comparison operators of View
    /// @return Result of comparison
    friend constexpr auto operator<=>(const BasicStringView&, const BasicStringView&) noexcept = default;
  };

  /// @brief BasicStringView of ASCII characters.
  using StringView = BasicStringView<StringEncoding::ASCII>;
}

template<>
struct fmt::formatter<clt::StringView>
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
  auto format(const clt::StringView& str, FormatContext& ctx)
  {
    return fmt::format_to(ctx.out(), "{:.{}}", str.data(), str.size());
  }
};

#endif //!HG_COLT_STRING_VIEW