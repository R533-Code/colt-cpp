#ifndef HG_COLT_STRING_LITERAL
#define HG_COLT_STRING_LITERAL

#include <type_traits>
#include <array>

namespace clt::meta
{
  template<typename T>
  /// @brief Check if T is a string literal "..."
  /// @tparam T The type to check for
  struct is_literal_char_str
  {
    /// @brief True if string literal
    static constexpr bool value = std::is_array_v<std::remove_reference_t<T>> && std::same_as<std::decay_t<std::remove_all_extents_t<std::remove_reference_t<T>>>, char>;
  };

  template<typename T>
  /// @brief Short-hand for is_literal_char_str::value
  /// @tparam T The type to check for
  inline constexpr bool is_literal_char_str_v = is_literal_char_str<T>::value;

  template <size_t N>
  /// @brief Type to pass string literals as parameters
  struct StringLiteral
  {
    /// @brief Constructs a StringLiteral from a literal string
    /// @param str The literal string
    constexpr StringLiteral(const char (&str)[N])
    {
      std::copy_n(str, N, value);
    }

    /// @brief Size of the literal string (without NUL terminator)
    /// @return Size of the literal string
    static constexpr size_t size() noexcept { return N - 1; }
    
    /// @brief The value of the literal string
    char value[N];
  };

  template <StringLiteral... Strs>
  /// @brief Concatenates StringView at compile time
  struct join
  {
    /// @brief Concatenate all the StringView and returns an array storing the result
    static constexpr auto impl() noexcept
    {
      constexpr std::size_t len = (Strs.size() + ... + 0);
      std::array<char, len + 1> arr{};
      auto append = [i = 0, &arr](auto const& s) mutable {
        for (size_t j = 0; j < s.size(); j++) arr[i++] = s.value[j];
      };
      (append(Strs), ...);
      arr[len] = '\0';
      return arr;
    }
    
    /// @brief Array of characters representing concatenated string
    static constexpr auto arr = impl();
    /// @brief Concatenation result
    static constexpr const char* value{ arr.data() };
  };

  template <StringLiteral... Strs>
  /// @brief Short-hand for join<...>::value
  static constexpr auto join_v = join<Strs...>::value;
}

#endif //!HG_COLT_STRING_LITERAL