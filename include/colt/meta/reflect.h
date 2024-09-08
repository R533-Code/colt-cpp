/*****************************************************************/ /**
 * @file   reflect.h
 * @brief  Contains utilities to reflect over a type at compile-time.
 * To add reflection to a type, add `COLT_ENABLE_REFLECTION` inside
 * of the class/struct, then use `COLT_DECLARE_TYPE` to add informations
 * about the members and methods of the type.
 * See `COLT_DECLARE_TYPE` for example usage.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_META_REFLECT
#define HG_META_REFLECT

#include <zpp_bits.h>
#include <fmt/format.h>
#include <colt/macro/assert.h>
#include <colt/typedefs.h>
#include <colt/hash.h>
#include <colt/dsa/iterator.h>
#include <colt/dsa/option.h>
#include <colt/dsa/string_view.h>
#include <colt/meta/map.h>

namespace clt::meta
{
  /// @brief Check if T is a reflectable through reflect<>
  /// @tparam T The type to check for
  template<typename T>
  struct is_reflectable
  {
    static constexpr bool value = false;
  };

  /// @brief Check if T is a reflectable through reflect<>
  /// @tparam T The type to check for
  template<typename T>
    requires std::is_pointer_v<T>
             && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
  struct is_reflectable<T>
  {
    static constexpr bool value = is_reflectable<std::remove_pointer_t<T>>::value;
  };

  /// @brief Check if T is a reflectable through reflect<>
  /// @tparam T The type to check for
  template<typename T>
    requires std::is_reference_v<T>
             && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
  struct is_reflectable<T>
  {
    static constexpr bool value = is_reflectable<std::remove_reference_t<T>>::value;
  };

  /// @brief Check if T is a reflectable through reflect<>
  /// @tparam T The type to check for
  template<typename T>
    requires(std::is_const_v<T> || std::is_volatile_v<T>)
  struct is_reflectable<T>
  {
    static constexpr bool value = is_reflectable<std::remove_cv_t<T>>::value;
  };

  /// @brief Short-hand for 'is_reflectable<T>::value'
  template<typename T>
  inline constexpr bool is_reflectable_v = is_reflectable<T>::value;

  template<typename T>
  concept reflectable = is_reflectable_v<T>;

  /// @brief The entity kind on which reflection is applied
  enum class EntityKind : u8
  {
    /// @brief Enumerations
    IS_ENUM,
    /// @brief Pointers, references, and all other built-in types
    IS_BUILTIN,
    /// @brief User defined class
    IS_CLASS,
    /// @brief Unknown type
    IS_UNKNOWN
  };

  template<typename T>
  /// @brief DO NOT USE! USE reflect<>, The kind on which reflection is applied
  /// @tparam T The type to whose EntityKind to check
  struct entity_kind
  {
    static constexpr EntityKind value = EntityKind::IS_UNKNOWN;
  };

  template<typename T>
  /// @brief DO NOT USE! USE reflect<>, short-hand for 'entity_kind<T>::value'
  inline constexpr EntityKind entity_kind_v = entity_kind<T>::value;

  template<typename T>
  /// @brief DO NOT USE! USE reflect<>, The name of the type on which reflection is applied
  /// @tparam T The type whose name to check
  struct unqualified_name
  {
    static constexpr StringView value = "!UNKNOWN_TYPE!";
  };

  template<typename T>
  /// @brief DO NOT USE! USE reflect<>, short-hand for 'unqualified_name<T>::value'
  inline constexpr StringView unqualified_name_v = unqualified_name<T>::value;

  template<typename T>
  /// @brief DO NOT USE! USE reflect<>, The members type list of the type on which reflection is applied
  /// @tparam T The type whose type list to check
  struct members_type
  {
    using type = meta::type_list<>;
  };

  template<typename T>
  /// @brief DO NOT USE! USE reflect<>, short-hand for 'members_type<T>::type'
  using members_type_t = typename members_type<T>::type;

  template<typename On>
  /// @brief DO NOT USE! USE reflect<>, Overload for each reflected type to apply a lambda on each of its members
  struct apply_on_members
  {
    template<typename F>
      requires std::is_invocable_v<F, On>
    constexpr void operator()(On&&, F&&) const
        noexcept(std::is_nothrow_invocable_v<F, On>)
    {
      //Does nothing...
    }
  };

  template<typename On>
  /// @brief DO NOT USE! USE reflect<>, Overload for each reflected type to apply a lambda on each of its methods
  struct apply_on_methods
  {
    template<typename F>
      requires std::is_invocable_v<F, On>
    constexpr void operator()(On&&, F&&) const
        noexcept(std::is_nothrow_invocable_v<F, On>)
    {
      //Does nothing...
    }
  };

  /// @brief Helper string for generating type name
  static constexpr StringView PTR_STR_END = "*";
  /// @brief Helper string for generating type name
  static constexpr StringView DOUBLE_REF = "&&";
  /// @brief Helper string for generating type name
  static constexpr StringView SINGLE_REF = "&";
  /// @brief Helper string for generating type name
  static constexpr StringView CONST_STR = "const ";
  /// @brief Helper string for generating type name
  static constexpr StringView VOLATILE_STR = "volatile ";

  template<typename T>
  /// @brief Deactivated reflection. Use COLT_DECLARE_TYPE to add reflection for a type.
  struct reflect
  {
  };

  /// @brief Unspecialized reflection informations
  /// @tparam T The type on which to reflect
  template<typename T>
    requires is_reflectable_v<T>
             && (!std::is_pointer_v<T>) && (!std::is_reference_v<T>)
             && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
  struct reflect<T>
  {
    static constexpr EntityKind kind() noexcept { return entity_kind_v<T>; }

    static constexpr StringView str() noexcept { return unqualified_name_v<T>; }

    using members_type = members_type_t<T>;

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      meta::apply_on_members<T>{}(std::forward<On>(obj), std::forward<F>(fn));
    }

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      meta::apply_on_methods<T>{}(std::forward<On>(obj), std::forward<F>(fn));
    }
  };

  /// @brief Specialized reflection for pointers
  /// @tparam T The pointer on which to reflect
  template<typename T>
    requires std::is_pointer_v<T> && is_reflectable_v<std::remove_pointer_t<T>>
             && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
  struct reflect<T>
  {
  private:
    static constexpr StringView _NAME = reflect<std::remove_pointer_t<T>>::str();

  public:
    static constexpr EntityKind kind() noexcept { return EntityKind::IS_BUILTIN; }

    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<_NAME, PTR_STR_END>;
    }

    using members_type = typename members_type_t<
        std::remove_pointer_t<T>>::template apply<std::add_pointer>;

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_members(On&&, F&&) noexcept
    {
      //Pointers do not have members... Do nothing.
    }

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_methods(On&&, F&&) noexcept
    {
      //Pointers do not have methods... Do nothing.
    }
  };

  /// @brief Specialized reflection for lvalue references
  /// @tparam T The reference on which to reflect
  template<typename T>
    requires std::is_lvalue_reference_v<T>
             && is_reflectable_v<std::remove_reference_t<T>>
             && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
  struct reflect<T>
  {
  private:
    static constexpr StringView _NAME = reflect<std::remove_reference_t<T>>::str();

  public:
    static constexpr EntityKind kind() noexcept { return EntityKind::IS_BUILTIN; }

    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<_NAME, SINGLE_REF>;
    }

    using members_type = typename members_type_t<
        std::remove_reference_t<T>>::template apply<std::add_lvalue_reference>;

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_members(On&&, F&&) noexcept
    {
      //References do not have members... Do nothing.
    }

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_methods(On&&, F&&) noexcept
    {
      //References do not have members... Do nothing.
    }
  };

  /// @brief Specialized reflection for rvalue references
  /// @tparam T The reference on which to reflect
  template<typename T>
    requires std::is_rvalue_reference_v<T>
             && is_reflectable_v<std::remove_reference_t<T>>
             && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
  struct reflect<T>
  {
  private:
    static constexpr StringView _NAME = reflect<std::remove_reference_t<T>>::str();

  public:
    static constexpr EntityKind kind() noexcept { return EntityKind::IS_BUILTIN; }

    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<_NAME, DOUBLE_REF>;
    }

    using members_type = typename meta::members_type_t<
        std::remove_reference_t<T>>::template apply<std::add_rvalue_reference>;

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_members(On&&, F&&) noexcept
    {
      //References do not have members... Do nothing.
    }

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_methods(On&&, F&&) noexcept
    {
      //References do not have members... Do nothing.
    }
  };

  /// @brief Specialized reflection for 'const' types
  /// @tparam T The type on which to reflect
  template<typename T>
    requires is_reflectable_v<std::remove_cv_t<T>> && std::is_const_v<T>
             && (!std::is_volatile_v<T>)
  struct reflect<T> : public reflect<std::remove_cv_t<T>>
  {
  private:
    static constexpr StringView _NAME = reflect<std::remove_cv_t<T>>::str();

  public:
    static constexpr meta::EntityKind kind() noexcept
    {
      return meta::entity_kind_v<std::remove_cv_t<T>>;
    }

    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<CONST_STR, _NAME>;
    }

    using members_type = typename meta::members_type_t<
        std::remove_cv_t<T>>::template apply<std::add_const>;

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_members(
          std::forward<On>(obj), std::forward<F>(fn));
    }

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_methods(
          std::forward<On>(obj), std::forward<F>(fn));
    }
  };

  template<typename T>
    requires is_reflectable_v<std::remove_cv_t<T>> && std::is_volatile_v<T>
             && (!std::is_const_v<T>)
  /// @brief Specialized reflection for 'volatile' types
  /// @tparam T The type on which to reflect
  struct reflect<T> : public reflect<std::remove_cv_t<T>>
  {
  private:
    static constexpr StringView _NAME = reflect<std::remove_cv_t<T>>::str();

  public:
    static constexpr meta::EntityKind kind() noexcept
    {
      return meta::entity_kind_v<std::remove_cv_t<T>>;
    }

    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<VOLATILE_STR, _NAME>;
    }

    using members_type = typename meta::members_type_t<
        std::remove_cv_t<T>>::template apply<std::add_volatile>;

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_members(
          std::forward<On>(obj), std::forward<F>(fn));
    }

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_methods(
          std::forward<On>(obj), std::forward<F>(fn));
    }
  };

  template<typename T>
    requires is_reflectable_v<std::remove_cv_t<T>> && std::is_const_v<T>
             && std::is_volatile_v<T>
  /// @brief Specialized reflection for 'const volatile' types
  /// @tparam T The type on which to reflect
  struct reflect<T> : public reflect<std::remove_cv_t<T>>
  {
  private:
    static constexpr StringView _NAME = reflect<std::remove_cv_t<T>>::str();

  public:
    static constexpr meta::EntityKind kind() noexcept
    {
      return meta::entity_kind_v<std::remove_cv_t<T>>;
    }

    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<CONST_STR, VOLATILE_STR, _NAME>;
    }

    using members_type = typename meta::members_type_t<
        std::remove_cv_t<T>>::template apply<std::add_cv>;

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_members(
          std::forward<On>(obj), std::forward<F>(fn));
    }

    template<typename On, typename F>
      requires std::same_as<On, std::remove_cvref_t<T>>
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_methods(
          std::forward<On>(obj), std::forward<F>(fn));
    }
  };

  template<typename F, typename... Ts>
    requires std::conjunction_v<std::is_invocable<F, Ts>...>
  /// @brief Calls 'f' with each arguments
  /// @tparam F The lambda type
  /// @tparam ...Ts The parameter pack
  /// @tparam  SFINAE helper
  /// @param f The lambda to call
  /// @param ...args The arguments to pass to the lambda
  constexpr void for_each(F&& f, Ts&&... args) noexcept
  {
    (f(std::forward<Ts>(args)), ...);
  }

  /// @brief Tag struct for iterating over members
  struct members_t
  {
  };
  /// @brief Tag struct for iterating over methods
  struct methods_t
  {
  };

  /// @brief Tag object for iterating over members
  inline constexpr members_t Members;

  /// @brief Tag object for iterating over methods
  inline constexpr methods_t Methods;

  template<typename T, typename F>
    requires is_reflectable_v<T>
  /// @brief Calls 'f' with each registered member of 'of'
  /// @tparam F The lambda type
  /// @tparam T The object type
  /// @param  Helper type (usually 'Members')
  /// @param of The object whose members to access
  /// @param f The lambda function
  constexpr void for_each(members_t, T&& of, F&& f) noexcept
  {
    reflect<std::remove_cvref_t<T>>::apply_on_members(
        std::forward<T>(of), std::forward<F>(f));
  }

  template<typename T, typename F>
    requires is_reflectable_v<T>
  /// @brief Calls 'f' with each registered methods of 'of'
  /// @tparam F The lambda type
  /// @tparam T The object type
  /// @param  Helper type (usually 'Methods')
  /// @param of The object whose members to access
  /// @param f The lambda function
  constexpr void for_each(methods_t, T&& of, F&& f) noexcept
  {
    reflect<std::remove_cvref_t<T>>::apply_on_methods(
        std::forward<T>(of), std::forward<F>(f));
  }

  template<typename T>
  concept reflectable_enum = std::is_enum_v<T> && clt::meta::is_reflectable_v<T>;
} // namespace clt::meta

namespace clt
{
  template<meta::hash_algorithm Algo, meta::reflectable T>
    requires(!meta::hashable<T> && !meta::is_contiguously_hashable_v<T>)
  static constexpr void hash_append(Algo& algo, const T& self)
  {
    using namespace clt::meta;
    for_each(
        Members, self, [&]<typename Ty>(Ty& value) { hash_append(algo, value); });
  }
} // namespace clt

template<clt::meta::reflectable T>
constexpr static auto serialize(auto& archive, T& self)
{
  using namespace clt::meta;

  decltype(archive(10)) error;
  for_each(
      Members, self,
      [&]<typename Ty>(Ty& value)
      {
        if (zpp::bits::failure(static_cast<std::errc>(error)))
          return;
        error = archive(value);
      });
  return error;
}

/// @brief Declares reflection informations for built-in 'TYPE'
#define DECLARE_BUILTIN_TYPE(TYPE)                 \
  template<>                                       \
  struct clt::meta::entity_kind<TYPE>              \
  {                                                \
    static constexpr clt::meta::EntityKind value = \
        clt::meta::EntityKind::IS_BUILTIN;         \
  };                                               \
  template<>                                       \
  struct clt::meta::is_reflectable<TYPE>           \
  {                                                \
    static constexpr bool value = true;            \
  };                                               \
                                                   \
  template<>                                       \
  struct clt::meta::members_type<TYPE>             \
  {                                                \
    using type = meta::type_list<>;                \
  };                                               \
                                                   \
  template<>                                       \
  struct clt::meta::unqualified_name<TYPE>         \
  {                                                \
    static constexpr StringView value = #TYPE;     \
  }

/***************************
* Declare all built-in types
****************************/

DECLARE_BUILTIN_TYPE(char);
DECLARE_BUILTIN_TYPE(char8_t);
DECLARE_BUILTIN_TYPE(char16_t);
DECLARE_BUILTIN_TYPE(char32_t);
DECLARE_BUILTIN_TYPE(i8);
DECLARE_BUILTIN_TYPE(u8);
DECLARE_BUILTIN_TYPE(i16);
DECLARE_BUILTIN_TYPE(u16);
DECLARE_BUILTIN_TYPE(i32);
DECLARE_BUILTIN_TYPE(u32);
DECLARE_BUILTIN_TYPE(i64);
DECLARE_BUILTIN_TYPE(u64);
DECLARE_BUILTIN_TYPE(float);
DECLARE_BUILTIN_TYPE(double);

//We no longer need this macro
#undef DECLARE_BUILTIN_TYPE

#define COLT_DETAILS_MEMBER_TO_MEMBER_PTR(type, name) , decltype(type::name)
#define COLT_DETAILS_IF_CONSTEXPR_IS_MEMBER_CALL_LAMBDA(TYPE, member)     \
  if constexpr (std::is_member_object_pointer_v<decltype(&TYPE::member)>) \
    fn(obj.member);
#define COLT_DETAILS_IF_CONSTEXPR_IS_METHOD_CALL_LAMBDA(TYPE, member)       \
  if constexpr (std::is_member_function_pointer_v<decltype(&TYPE::member)>) \
    fn(&obj.member);

/// @brief Adds the necessary friends declarations for reflection
#define COLT_ENABLE_REFLECTION()   \
  template<typename _TMeTaReflect> \
  friend struct clt::meta::reflect

/// @brief Enables reflection on a type.
/// ~~~~~~~~~~~~~~{.cpp}
/// class Test
/// {
///   u64 a;
///   u64 b;
///
///   //Required for private members
///   COLT_ENABLE_REFLECTION();
/// };
/// COLT_DECLARE_TYPE(Test, a, b);
/// ~~~~~~~~~~~~~~
#define COLT_DECLARE_TYPE(TYPE, member, ...)                                        \
  template<>                                                                        \
  struct clt::meta::entity_kind<TYPE>                                               \
  {                                                                                 \
    static constexpr clt::meta::EntityKind value = clt::meta::EntityKind::IS_CLASS; \
  };                                                                                \
  template<>                                                                        \
  struct clt::meta::is_reflectable<TYPE>                                            \
  {                                                                                 \
    static constexpr bool value = true;                                             \
  };                                                                                \
  template<>                                                                        \
  struct clt::meta::reflect<TYPE>                                                   \
  {                                                                                 \
    static constexpr clt::meta::EntityKind kind() noexcept                          \
    {                                                                               \
      return clt::meta::EntityKind::IS_CLASS;                                       \
    }                                                                               \
    static constexpr clt::StringView str() noexcept                                 \
    {                                                                               \
      return #TYPE;                                                                 \
    }                                                                               \
    using members_type =                                                            \
        typename clt::meta::type_list<decltype(TYPE::member) COLT_FOR_EACH_1ARG(    \
            COLT_DETAILS_MEMBER_TO_MEMBER_PTR, TYPE,                                \
            __VA_ARGS__)>::template remove_if<std::is_member_function_pointer>;     \
    using methods_type =                                                            \
        typename clt::meta::type_list<decltype(&TYPE::member) COLT_FOR_EACH_1ARG(   \
            COLT_DETAILS_MEMBER_TO_MEMBER_PTR, TYPE,                                \
            __VA_ARGS__)>::template remove_if<std::is_member_object_pointer>;       \
    static constexpr size_t members_count() noexcept                                \
    {                                                                               \
      return members_type::size;                                                    \
    }                                                                               \
    static constexpr size_t methods_count() noexcept                                \
    {                                                                               \
      return members_type::size;                                                    \
    }                                                                               \
    template<typename On, typename F>                                               \
      requires std::same_as<std::remove_cvref_t<On>, std::remove_cvref_t<TYPE>>     \
    static constexpr void apply_on_members(On&& obj, F&& fn)                        \
    {                                                                               \
      if constexpr (std::is_member_object_pointer_v<decltype(&TYPE::member)>)       \
        fn(obj.member);                                                             \
      COLT_FOR_EACH_1ARG(                                                           \
          COLT_DETAILS_IF_CONSTEXPR_IS_MEMBER_CALL_LAMBDA, TYPE, __VA_ARGS__)       \
    }                                                                               \
    template<typename On, typename F>                                               \
      requires std::same_as<std::remove_cvref_t<On>, std::remove_cvref_t<TYPE>>     \
    static constexpr void apply_on_methods(On&& obj, F&& fn)                        \
    {                                                                               \
      if constexpr (std::is_member_function_pointer_v<decltype(&TYPE::member)>)     \
        fn(&obj.member);                                                            \
      COLT_FOR_EACH_1ARG(                                                           \
          COLT_DETAILS_IF_CONSTEXPR_IS_METHOD_CALL_LAMBDA, TYPE, __VA_ARGS__)       \
    }                                                                               \
  };                                                                                \
  template<>                                                                        \
  struct clt::meta::is_contiguously_hashable<TYPE>                                  \
  {                                                                                 \
    static constexpr bool value = reflect<TYPE>::members_type::template remove_if<  \
                                      is_contiguously_hashable>::size               \
                                      == 0                                          \
                                  && !has_padding_v<TYPE>;                          \
  }

#define COLT_DETAILS_EXPAND_ENUM(en) , en
#define COLT_DETAILS_STRINGIZE_ENUM(en) \
  , std::string_view                    \
  {                                     \
    #en                                 \
  }
#define COLT_DETAILS_MAP_PAIR_ENUM(en) \
  , std::pair                          \
  {                                    \
    std::string_view{#en}, en          \
  }

#define ADD_REFLECTION_FOR_CONSECUTIVE_ENUM(namespace_name, name, first, ...)                           \
  template<>                                                                                            \
  struct clt::meta::entity_kind<namespace_name::name>                                                   \
  {                                                                                                     \
    static constexpr clt::meta::EntityKind value = clt::meta::EntityKind::IS_ENUM;                      \
  };                                                                                                    \
  template<>                                                                                            \
  struct clt::meta::reflect<namespace_name::name>                                                       \
  {                                                                                                     \
    using enum_type = std::underlying_type_t<namespace_name::name>;                                     \
    static constexpr std::string_view str() noexcept                                                    \
    {                                                                                                   \
      return #name;                                                                                     \
    }                                                                                                   \
    static constexpr bool is_consecutive() noexcept                                                     \
    {                                                                                                   \
      return true;                                                                                      \
    }                                                                                                   \
    static constexpr std::array name##_str = {std::string_view{                                         \
        #first} COLT_FOR_EACH(COLT_DETAILS_STRINGIZE_ENUM, __VA_ARGS__)};                               \
    static constexpr size_t min() noexcept                                                              \
    {                                                                                                   \
      return 0;                                                                                         \
    }                                                                                                   \
    static constexpr size_t max() noexcept                                                              \
    {                                                                                                   \
      return name##_str.size() - 1;                                                                     \
    }                                                                                                   \
    static constexpr size_t count() noexcept                                                            \
    {                                                                                                   \
      return name##_str.size();                                                                         \
    }                                                                                                   \
    static constexpr std::string_view to_str(namespace_name::name value)                                \
    {                                                                                                   \
      assert_true("Enum out of range!", static_cast<enum_type>(value) <= max());                        \
      return name##_str[static_cast<enum_type>(value)];                                                 \
    }                                                                                                   \
    static constexpr clt::Option<namespace_name::name> from(enum_type value)                            \
    {                                                                                                   \
      if (value > max())                                                                                \
        return clt::None;                                                                               \
      return static_cast<namespace_name::name>(value);                                                  \
    }                                                                                                   \
                                                                                                        \
  private:                                                                                              \
    using ArrayTable_t = std::array<                                                                    \
        std::pair<std::string_view, namespace_name::name>, name##_str.size()>;                          \
    using Map_t =                                                                                       \
        clt::meta::Map<std::string_view, namespace_name::name, name##_str.size()>;                      \
    static constexpr ArrayTable_t get_array()                                                           \
    {                                                                                                   \
      using enum namespace_name::name;                                                                  \
      ArrayTable_t ret = {std::pair{std::string_view{#first}, first} COLT_FOR_EACH(                     \
          COLT_DETAILS_MAP_PAIR_ENUM, __VA_ARGS__)};                                                    \
      return ret;                                                                                       \
    }                                                                                                   \
    static const ArrayTable_t internal_map;                                                             \
    static const Map_t map;                                                                             \
                                                                                                        \
  public:                                                                                               \
    static constexpr clt::Option<namespace_name::name> from(std::string_view str)                       \
    {                                                                                                   \
      return map.find(str);                                                                             \
    }                                                                                                   \
    static constexpr clt::enum_iterator<namespace_name::name, 0, name##_str.size() - 1> iter() noexcept \
    {                                                                                                   \
      return {};                                                                                        \
    }                                                                                                   \
  };                                                                                                    \
  inline constexpr clt::meta::reflect<namespace_name::name>::ArrayTable_t                               \
      clt::meta::reflect<namespace_name::name>::internal_map =                                          \
          clt::meta::reflect<namespace_name::name>::get_array();                                        \
  inline constexpr clt::meta::reflect<namespace_name::name>::Map_t                                      \
      clt::meta::reflect<namespace_name::name>::map = {                                                 \
          {clt::meta::reflect<namespace_name::name>::internal_map}};                                    \
  template<>                                                                                            \
  struct clt::meta::is_reflectable<namespace_name::name>                                                \
  {                                                                                                     \
    static constexpr bool value = true;                                                                 \
  }

/// @brief Declares an enumeration with reflection support
#define DECLARE_ENUM_WITH_TYPE(type, namespace_name, name, first, ...) \
  namespace namespace_name                                             \
  {                                                                    \
    enum class name : type                                             \
    {                                                                  \
      first COLT_FOR_EACH(COLT_DETAILS_EXPAND_ENUM, __VA_ARGS__)       \
    };                                                                 \
  }                                                                    \
  ADD_REFLECTION_FOR_CONSECUTIVE_ENUM(namespace_name, name, first, __VA_ARGS__)

#define DECLARE_ENUM(namespace_name, name, first, ...)           \
  namespace namespace_name                                       \
  {                                                              \
    enum class name                                              \
    {                                                            \
      first COLT_FOR_EACH(COLT_DETAILS_EXPAND_ENUM, __VA_ARGS__) \
    };                                                           \
  }                                                              \
  ADD_REFLECTION_FOR_CONSECUTIVE_ENUM(namespace_name, name, first, __VA_ARGS__)

//Add reflection for already existing enum
ADD_REFLECTION_FOR_CONSECUTIVE_ENUM(
    clt::meta, EntityKind, IS_ENUM, IS_BUILTIN, IS_CLASS, IS_UNKNOWN);

template<clt::meta::reflectable_enum T>
struct fmt::formatter<T>
{
  bool human_readable = false;

  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    auto it  = ctx.begin();
    auto end = ctx.end();
    if (it == end)
      return it;
    if (*it == 'h')
    {
      ++it;
      human_readable = true;
    }
    assert_true("Possible format for Enum are: {} or {:h}!", *it == '}');
    return it;
  }

  template<typename FormatContext>
  auto format(const T& exp, FormatContext& ctx) const
  {
    using namespace clt;
    using namespace clt::meta;

    if (human_readable)
      return fmt::format_to(ctx.out(), "{}", reflect<T>::to_str(exp));
    return fmt::format_to(
        ctx.out(), "{}::{}", reflect<T>::str(), reflect<T>::to_str(exp));
  }
};

template<clt::meta::reflectable T>
  requires(!clt::meta::formattable<T>)
          && (clt::meta::reflect<T>::kind() == clt::meta::EntityKind::IS_CLASS)
struct fmt::formatter<T>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const T& obj, FormatContext& ctx) const
  {
    using namespace clt;
    using namespace clt::meta;
    auto it = fmt::format_to(ctx.out(), "{}: {{ ", reflect<T>::str());

    u64 i = 0;
    for_each(
        Members, obj,
        [&]<typename Ti>(const Ti& o)
        {
          it = fmt::format_to(
              it, "{}{}", o, ++i == reflect<T>::members_count() ? " }" : ", ");
        });
    return it;
  }
};

#endif // !HG_META_REFLECT
