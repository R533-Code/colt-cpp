#ifndef HG_COLT_STRING_VIEW
#define HG_COLT_STRING_VIEW

#include <type_traits>

#include "./helper.h"
#include "./span.h"
#include "../str/distance.h"

namespace clt
{
  template<typename CharT = char>
  class StringViewOf
    : public View<CharT>
  {
    static_assert(std::is_same_v<CharT, char>, "StringViewOf only supports char for now!");
    /// @brief Helper type
    using ViewT = View<CharT>;

  public:

    /// @brief Constructs an empty StringViewOf
    constexpr StringViewOf() noexcept
      : ViewT(nullptr, nullptr) {}
    /// @brief Range constructor
    /// @param begin The beginning of the view
    /// @param end The end of the view
    constexpr StringViewOf(const CharT* begin, const CharT* end) noexcept
      : ViewT(begin, end) {}
    /// @brief Constructs a StringView over a NUL terminated string
    /// @param cstr The NUL terminated string to span over
    constexpr StringViewOf(const CharT* cstr) noexcept
      : ViewT(cstr, clt::strlen(cstr)) {}
    /// @brief Constructs a StringView over a NUL terminated string, including its NUL terminator
    /// @param cstr The NUL terminated string to span over
    /// @param  Tag object (WithNUL)
    constexpr StringViewOf(const CharT* cstr, meta::WithNULT) noexcept
      : ViewT(cstr, clt::strlen(cstr) + 1) {}
    /// @brief Copy constructor
    /// @param  The StringView to copy
    constexpr StringViewOf(const StringViewOf&) noexcept = default;
    /// @brief Move constructor
    /// @param  The StringView to move
    constexpr StringViewOf(StringViewOf&&) noexcept = default;

    constexpr StringViewOf& operator=(const StringViewOf&) noexcept = default;
    constexpr StringViewOf& operator=(StringViewOf&&) noexcept = default;

    /// @brief Pops all spaces from the beginning and the end of the StringView.
    /// The characters that are considered spaces are '\n', ' ', '\v', '\t'.
    constexpr StringViewOf strip_spaces() noexcept
    {
      while (ViewT::is_not_empty())
        if (std::isspace(*ViewT::begin()))
          ViewT::pop_front();
        else
          break;

      while (ViewT::is_not_empty())
        if (std::isspace(*(ViewT::begin() + ViewT::size() - 1)))
          ViewT::pop_front();
        else
          break;
      return *this;
    }

    /// @brief Value that signifies that the value was not found
    static constexpr size_t npos = std::numeric_limits<size_t>::max();

    /// @brief Finds first occurrence of 'chr' and returns its index, or npos if not found
    /// @param chr The character to search for
    /// @param offset The offset from which to begin to search
    /// @return The index of 'chr' or npos if not found
    constexpr size_t find(CharT chr, size_t offset = 0) const noexcept
    {
      for (size_t i = offset; i < this->get_size(); i++)
      {
        if (this->get_data()[i] == chr)
          return i;
      }
      return npos;
    }

    /// @brief Check if the StringView begins with 'chr'
    /// @param chr The character to check for
    /// @return True if the current StringView begins with 'chr'
    constexpr bool begins_with(CharT chr) const noexcept
    {
      if (this->is_empty())
        return false;
      return this->get_front() == chr;
    }

    /// @brief Check if the StringView begins with another StringView
    /// @param str The StringView to search for
    /// @return True if the current StringView begins with 'str'
    constexpr bool begins_with(StringViewOf str) const noexcept
    {
      if (str.get_size() > this->get_size())
        return false;
      for (size_t i = 0; i < str.get_size(); i++)
      {
        if (str[i] != (*this)[i])
          return false;
      }
      return true;
    }

    /// @brief Conversion operator
    /// @return View
    constexpr operator View<CharT>() const noexcept
    {
      return { ViewT::begin(), ViewT::end() };
    }

    /// @brief Uses comparison operators of View
    /// @return Result of comparison
    friend constexpr auto operator<=>(const StringViewOf&, const StringViewOf&) noexcept = default;
  };

  /// @brief StringViewOf char
  using StringView = StringViewOf<char>;
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