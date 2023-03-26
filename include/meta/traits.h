#ifndef HG_COLT_TRAITS
#define HG_COLT_TRAITS

#include <type_traits>

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

  template<typename Of, typename For>
  /// @brief Makes a type match the const-ness of another
  /// @tparam Of The type whose const-ness to match against
  /// @tparam For The type to which to add const if necessary
  struct match_const
  {
    using type = std::conditional_t<std::is_const_v<Of>, std::add_const_t<For>, std::remove_const_t<For>>;
  };

  template<typename Of, typename For>
  /// @brief Short-hand for match_const<Of, For>::type
  /// @tparam Of The type whose const-ness to match against
  /// @tparam For The type to which to add const if necessary
  using match_const_t = typename match_const<Of, For>::type;

  template<typename Of, typename For>
  /// @brief Makes a type match the volatile-ness of another
  /// @tparam Of The type whose volatile-ness to match against
  /// @tparam For The type to which to add volatile if necessary
  struct match_volatile
  {
    using type = std::conditional_t<std::is_volatile_v<Of>, std::add_volatile_t<For>, std::remove_volatile_t<For>>;
  };

  template<typename Of, typename For>
  /// @brief Short-hand for match_volatile<Of, For>::type
  /// @tparam Of The type whose volatile-ness to match against
  /// @tparam For The type to which to add volatile if necessary
  using match_volatile_t = typename match_volatile<Of, For>::type;

  template<typename Of, typename For>
  /// @brief Makes a type match the qualifiers of another
  /// @tparam Of The type whose qualifiers to match against
  /// @tparam For The type to which to add const if necessary
  struct match_cv
  {
    using type = match_volatile_t<Of, match_const_t<Of, For>>;
  };

  template<typename Of, typename For>
  /// @brief Short-hand for match_cv<Of, For>::type
  /// @tparam Of The type whose qualifiers to match against
  /// @tparam For The type to transform if necessary
  using match_cv_t = typename match_cv<Of, For>::type;

  template<typename T>
  concept StdRatio = (std::is_integral_v<decltype(T::num)>) && (std::is_integral_v<decltype(T::den)>);

  namespace details
  {
    template<typename T> requires (std::is_void_v<T>)
    /// @brief Helper for sizeof_or_zero
    /// @tparam T The type to check for
    /// @return 0
    consteval size_t sizeof_or_zero() noexcept
    {
      return 0;
    }

    template<typename T> requires (!std::is_void_v<T>)
    /// @brief Helper for sizeof_or_zero
    /// @tparam T The type to check for
    /// @return sizeof(T)
    consteval size_t sizeof_or_zero() noexcept
    {
      return sizeof(T);
    }
  }

  template<typename T>
  /// @brief Returns the sizeof a type or 0 if void
  /// @tparam T The type
  struct sizeof_or_zero
  {
    static constexpr size_t value = details::sizeof_or_zero<T>();
  };

  template<typename T>
  /// @brief Shorthand for sizeof_or_zero<T>::value
  /// @tparam T The type to check for
  inline constexpr size_t sizeof_or_zero_v = sizeof_or_zero<T>::value;  
}

#endif //!HG_COLT_TRAITS