#ifndef HG_COLT_TYPE_LIST
#define HG_COLT_TYPE_LIST

#include <cstdint>

namespace clt::meta
{
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

  template<typename... Ts>
  /// @brief List of types
  /// @tparam ...Ts The types to hold
  struct type_list
  {
    template<uint64_t index> requires (index < sizeof...(Ts))
    /// @brief Returns the type at index 'index'
    using get = typename details::get_n<index, Ts...>::type;

    static constexpr size_t size = sizeof...(Ts);
  };
}

#endif //!HG_COLT_TYPE_LIST