#ifndef HG_COLT_TYPE_LIST
#define HG_COLT_TYPE_LIST

#include <cstdint>
#include <tuple>
#include <type_traits>

namespace clt::meta
{
  template<typename T>
  /// @brief Helper condition to give to static_assert to avoid compilation error
  /// before template is not instantiated.
  /// @tparam T Any type
  inline constexpr bool always_false = false;

  template<typename... Ts>
  // forward declaration
  struct type_list;

  namespace details
  {
    template<uint64_t index, typename T, typename... Ts>
    /// @brief Helper for type_list::get<...>
    /// @tparam T Current type
    /// @tparam ...Ts Types
    struct get_ni;

    template<typename T, typename... Ts>
    /// @brief Helper for type_list::get<...>
    /// @tparam T Current type
    /// @tparam ...Ts Types
    struct get_ni<0, T, Ts...>
    {
      /// @brief The type of get<...>
      using type = T;
    };

    template<uint64_t index, typename T, typename... Ts>
    /// @brief Helper for type_list::get<...>
    /// @tparam T Current type
    /// @tparam ...Ts Types
    struct get_ni
    {
      /// @brief The type of get<...>
      using type = typename get_ni<index - 1, Ts...>::type;
    };

    template<uint64_t index, typename... Ts>
    /// @brief Helper for type_list::get<...>
    /// @tparam ...Ts Types
    struct get_n
    {
      static_assert(index < sizeof...(Ts), "Invalid index for get<...>!");
      /// @brief The type returned by 'get<...>'
      using type = typename get_ni<index, Ts...>::type;
    };

    template<uint64_t index>
    /// @brief Helper for type_list::get<...>, generates an error out of bound
    /// @tparam ...Ts Types
    struct get_n<index>
    {};
  }

  namespace details
  {
    template<typename...Ts>
    using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

    template<typename T, typename...Ts>
    using remove_t = tuple_cat_t<
      typename std::conditional_t<
      std::is_same_v<T, Ts>,
      std::tuple<>,
      std::tuple<Ts>
      >...
    >;

    template<typename... Ts>
    type_list<Ts...> from_tuple_to_type_list(std::tuple<Ts...>) noexcept
    {
      static_assert(always_false<void>,
        "from_tuple_type_list is not allowed in an evaluated context!");
    }
  }

  template<typename T>
  /// @brief Converts a tuple to a type list
  /// @tparam T The tuple type
  using tuple_to_type_list_t = decltype(details::from_tuple_to_type_list(std::declval<T>()));

  template<typename... Ts>
  /// @brief List of types
  /// @tparam ...Ts The types to hold
  struct type_list
  {
    /// @brief The type of the current list (not very useful)
    using this_list = type_list<Ts...>;

    template<uint64_t index> requires (index < sizeof...(Ts))
    /// @brief Returns the type at index 'index'
    using get = typename details::get_n<index, Ts...>::type;

    template<typename T>
    /// @brief Pushes a new type at the back of the list
    /// @tparam T The new type
    using push_back = type_list<Ts..., T>;

    template<typename T>
    /// @brief Pushes a new type at the front of the list
    /// @tparam T The new type
    using push_front = type_list<T, Ts...>;

    template<template<typename> typename apply_f>
    /// @brief Applies a transformation to the list.
    /// To avoid compiler error, write "template" before accessing this member
    using apply = type_list<typename apply_f<Ts>::type...>;

    /// @brief Returns the head of the list
    using head = get<0>;
    /// @brief Returns the tail of the list
    using tail = get<sizeof...(Ts) - 1>;

    /// @brief Size of the list
    static constexpr size_t size = sizeof...(Ts);

    /// @brief Returns the head of the list or void if the list is empty
    using head_or_void = std::conditional_t<size == 0, void, get<0>>;
    /// @brief Returns the tail of the list or void if the list is empty
    using tail_or_void = std::conditional_t<size == 0, void, get<size - 1>>;

    template<typename T, template<typename> condition>
    /// @brief Pushes a type to the front of the list if 'condition<T>::value' is true
    /// @tparam T The type to push
    using push_front_if = std::conditional_t<condition<T>::value, push_front<T>, this_list>;

    template<typename T, template<typename> condition>
    /// @brief Pushes a type to the back of the list if 'condition<T>::value' is true
    /// @tparam T The type to push
    using push_back_if = std::conditional_t<condition<T>::value, push_back<T>, this_list>;

    template<typename What>
    /// @brief Remove all types that are the same as 'What'
    /// @tparam What The type to remove
    using remove_all = tuple_to_type_list_t<details::remove_t<What, Ts...>>;
    
    /// @brief Removes all void types from the type list
    using pop_void = remove_all<void>;
  };
}

#endif //!HG_COLT_TYPE_LIST