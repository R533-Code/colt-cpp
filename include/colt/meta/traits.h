/*****************************************************************/ /**
 * @file   meta_traits.h
 * @brief  Contains some meta-programming helpers.
 *
 * @author RPC
 * @date   January 2024
 *********************************************************************/
#ifndef HG_META_TRAITS
#define HG_META_TRAITS

#include <cstdint>
#include <tuple>
#include <zpp_bits.h>
#include <fmt/format.h>
#include <type_traits>
#include "colt/config.h"

namespace clt::meta
{
  /// @brief Check if a type is serializable using zpp-bits
  template<typename T>
  concept serializable = requires(zpp::bits::basic_out<std::span<std::byte>> out, T obj) {
        {
          static_cast<std::errc>(out(obj))
        } -> std::same_as<std::errc>;
  };

  /// @brief Check if a type is formattable using fmt
  template<typename T>
  concept formattable = requires(T obj) {
    {
      fmt::format_to("{}", obj)
    }->std::same_as<std::string>;
  };

  /// @brief Check if a type has padding bits or not
  /// @tparam T The type
  template<typename T>
  struct has_padding
      : std::conditional_t<
            std::has_unique_object_representations_v<T> || std::floating_point<T>,
            std::false_type, std::true_type>
  {
  };

  /// @brief Shorthand for has_padding<T>::value
  /// @tparam T The type
  template<typename T>
  static constexpr bool has_padding_v = has_padding<T>::value;

  /// @brief Helpers to inherit from built-in types
  /// @tparam T The built-in type to inherit from
  template<typename T, u64 LINE>
  struct inherit_t
  {
    T value{};
  };

  /// @brief Empty type
  /// @tparam T Any type to generate different empty_t
  template<typename T>
  struct empty_t
  {
  };

  template<typename Of, typename For>
  /// @brief Makes a type match the const-ness of another
  /// @tparam Of The type whose const-ness to match against
  /// @tparam For The type to which to add const if necessary
  struct match_const
  {
    using type = std::conditional_t<
        std::is_const_v<Of>, std::add_const_t<For>, std::remove_const_t<For>>;
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
    using type = std::conditional_t<
        std::is_volatile_v<Of>, std::add_volatile_t<For>,
        std::remove_volatile_t<For>>;
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

  namespace details
  {
    template<typename T>
      requires(std::is_void_v<T>)
    /// @brief Helper for sizeof_or_zero
    /// @tparam T The type to check for
    /// @return 0
    consteval size_t sizeof_or_zero() noexcept
    {
      return 0;
    }

    template<typename T>
      requires(!std::is_void_v<T>)
    /// @brief Helper for sizeof_or_zero
    /// @tparam T The type to check for
    /// @return sizeof(T)
    consteval size_t sizeof_or_zero() noexcept
    {
      return sizeof(T);
    }
  } // namespace details

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

  template<typename Debug, typename Release>
  /// @brief Chooses a type if on Debug configuration, or another for Release
  /// @tparam Debug The type on Debug configuration
  /// @tparam Release The type on Release configuration
  struct for_debug_for_release
  {
    using type = std::conditional_t<clt::is_debug_build(), Debug, Release>;
  };

  template<typename Debug, typename Release>
  /// @brief Shorthand for 'for_debug_for_release<Debug, Release>::type'
  /// @tparam Debug The type on Debug configuration
  /// @tparam Release The type on Release configuration
  using for_debug_for_release_t =
      typename for_debug_for_release<Debug, Release>::type;

  template<typename T>
  /// @brief Helper condition to give to static_assert to avoid compilation error
  /// before template is not instantiated.
  /// @tparam T Any type
  inline constexpr bool ALWAYS_FALSE = false;

  template<typename T, typename... U>
  /// @brief Check if a type is any of a list
  concept is_any_of = (std::same_as<T, U> || ...);

  template<typename T, typename... U>
  /// @brief Check if all the type are the same
  concept are_all_same = (std::same_as<T, U> || ...);
} // namespace clt::meta

#endif // !HG_META_TRAITS