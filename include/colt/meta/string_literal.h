/*****************************************************************//**
 * @file   string_literal.h
 * @brief  Contains StringLiteral, used to pass string literals
 *         as template parameters.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_META_STRING_LITERAL
#define HG_META_STRING_LITERAL

#include <array>
#include <algorithm>

namespace clt::meta
{
  template<size_t N>
  /// @brief Type to pass string literals as template parameters
  struct StringLiteral
  {
    /// @brief Constructs a StringLiteral from a literal string
    /// @param str The literal string
    constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }

    /// @brief Size of the literal string (without NUL terminator)
    /// @return Size of the literal string
    static constexpr size_t size() noexcept { return N - 1; }

    /// @brief The value of the literal string
    char value[N];
  };

  template<StringLiteral... Strs>
  /// @brief Concatenates StringLiterals at compile time
  struct join
  {
    /// @brief Concatenate all the StringLiterals and returns an array storing the result
    static constexpr auto impl() noexcept
    {
      constexpr std::size_t len = (Strs.size() + ... + 0);
      std::array<char, len + 1> arr{};
      auto append = [i = 0, &arr](auto const& s) mutable
      {
        for (size_t j = 0; j < s.size(); j++)
          arr[i++] = s.value[j];
      };
      (append(Strs), ...);
      arr[len] = '\0';
      return arr;
    }

    /// @brief Array of characters representing concatenated string
    static constexpr auto arr = impl();

    /// @brief Concatenation result
    static constexpr const char* value{arr.data()};
  };

  template<StringLiteral... Strs>
  /// @brief Short-hand for join<...>::value
  static constexpr auto join_v = join<Strs...>::value;
} // namespace clt::meta

#endif //!HG_META_STRING_LITERAL