/** @file refl.h
* Adds reflection for built-in types.
*/

#ifndef HG_COLT_REFL
#define HG_COLT_REFL

#ifndef FMT_HEADER_ONLY
  #define FMT_HEADER_ONLY
#endif //!FMT_HEADER_ONLY
#include "../../libraries/fmt/include/fmt/format.h"

#include "../meta/string_literal.h"
#include "../meta/traits.h"
#include "../meta/type_list.h"
#include "../util/macro.h"
#include "../structs/string_view.h"

namespace clt::meta
{
  template<typename T>
  struct is_reflectable
  {
    static constexpr bool value = false;
  };

  template<typename T> requires std::is_pointer_v<T>
    && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
  struct is_reflectable<T>
  {
    static constexpr bool value = is_reflectable<std::remove_pointer_t<T>>::value;
  };

  template<typename T> requires std::is_reference_v<T>
    && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
  struct is_reflectable<T>
  {
    static constexpr bool value = is_reflectable<std::remove_reference_t<T>>::value;
  };

  template<typename T> requires (std::is_const_v<T> || std::is_volatile_v<T>)
  struct is_reflectable<T>
  {
    static constexpr bool value = is_reflectable<std::remove_cv_t<T>>::value;
  };

  template<typename T>
  inline constexpr bool is_reflectable_v = is_reflectable<T>::value;
}

namespace clt::refl
{
  template<typename T>
  struct unqualified_name
  {
    static constexpr StringView value = "!UNKNOWN_TYPE!";
  };

  template<typename T>
  inline constexpr StringView unqualified_name_v = unqualified_name<T>::value;

  template<typename T>
  struct members_type
  {
    using type = meta::type_list<>;
  };

  template<typename T>
  using members_type_t = typename members_type<T>::type;

  template<typename On>
  struct apply_on_members
  {
    template<typename F> requires std::is_invocable_v<F, On>
    constexpr void operator()(On&&, F&&) const
      noexcept(std::is_nothrow_invocable_v<F, On>)
    {
      //Does nothing...
    }
  };
  
  template<typename On>
  struct apply_on_methods
  {
    template<typename F> requires std::is_invocable_v<F, On>
    constexpr void operator()(On&&, F&&) const
      noexcept(std::is_nothrow_invocable_v<F, On>)
    {
      //Does nothing...
    }
  };
}

namespace clt
{
  namespace details
  {
    /// @brief Helper string for generating type name
    static constexpr StringView PTR_STR_START   = "PTR<";
    /// @brief Helper string for generating type name
    static constexpr StringView PTR_STR_END     = ">";

    /// @brief Helper string for generating type name
    static constexpr StringView DOUBLE_REF      = "&&";
    /// @brief Helper string for generating type name
    static constexpr StringView SINGLE_REF      = "&";
    /// @brief Helper string for generating type name
    static constexpr StringView CONST_STR       = "const ";
    /// @brief Helper string for generating type name
    static constexpr StringView VOLATILE_STR    = "volatile ";

  }

  template<typename T>
  struct reflect {};

  template<typename T> requires meta::is_reflectable_v<T>
    && (!std::is_pointer_v<T>) && (!std::is_reference_v<T>)
    && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
  /// @brief Unspecialized reflection informations
  /// @tparam T The type on which to reflect
  struct reflect<T>
  {
    static constexpr StringView str() noexcept
    {
      return refl::unqualified_name_v<T>;
    }

    using members_type = typename refl::members_type_t<std::remove_pointer_t<T>>::template apply<std::add_pointer>;

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      refl::apply_on_members<T>{}(std::forward<On>(obj), std::forward<F>(fn));
    }

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      refl::apply_on_methods<T>{}(std::forward<On>(obj), std::forward<F>(fn));
    }
  };
  
  template<typename T> requires std::is_pointer_v<T>
    && meta::is_reflectable_v<std::remove_pointer_t<T>>
    && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
  struct reflect<T>
  {
  private:
    static constexpr StringView _NAME = reflect<std::remove_pointer_t<T>>::str();

  public:
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<details::PTR_STR_START,
        _NAME,
        details::PTR_STR_END
      >;
    }

    using members_type = typename refl::members_type_t<std::remove_pointer_t<T>>::template apply<std::add_pointer>;

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      fn(obj);
    }

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_methods(On&&, F&&)
    {
      //Pointers do not have methods... Do nothing.
    }
  };

  template<typename T> requires std::is_lvalue_reference_v<T>
    && meta::is_reflectable_v<std::remove_reference_t<T>>
    && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
  struct reflect<T>
  {
  private:
    static constexpr StringView _NAME = reflect<std::remove_reference_t<T>>::str();

  public:
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<
        _NAME,
        details::SINGLE_REF
      >;
    }
    
    using members_type = typename refl::members_type_t<std::remove_reference_t<T>>::template apply<std::add_lvalue_reference>;

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_members(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_methods(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }
  };

  template<typename T> requires std::is_rvalue_reference_v<T>
    && meta::is_reflectable_v<std::remove_reference_t<T>>
    && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
    struct reflect<T>
  {
  private:
    static constexpr StringView _NAME = reflect<std::remove_reference_t<T>>::str();

  public:
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<
        _NAME,
        details::DOUBLE_REF
      >;
    }

    using members_type = typename refl::members_type_t<std::remove_reference_t<T>>::template apply<std::add_rvalue_reference>;

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_members(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_methods(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }
  };

  template<typename T> requires meta::is_reflectable_v<std::remove_cv_t<T>>
    && std::is_const_v<T> && (!std::is_volatile_v<T>)
    struct reflect<T>
  {
  private:
    static constexpr StringView _NAME = reflect<std::remove_cv_t<T>>::str();
  
  public:
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<
        details::CONST_STR,
        _NAME
      >;
    }

    using members_type = typename refl::members_type_t<std::remove_cv_t<T>>::template apply<std::add_const>;

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_members(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_methods(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }
  };

  template<typename T> requires meta::is_reflectable_v<std::remove_cv_t<T>>
    && std::is_volatile_v<T> && (!std::is_const_v<T>)
  struct reflect<T>
  {
  private:
    static constexpr StringView _NAME = reflect<std::remove_cv_t<T>>::str();

  public:
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<
        details::VOLATILE_STR,
        _NAME
      >;
    }

    using members_type = typename refl::members_type_t<std::remove_cv_t<T>>::template apply<std::add_volatile>;

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_members(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_methods(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }
  };

  template<typename T> requires meta::is_reflectable_v<std::remove_cv_t<T>>
    && std::is_const_v<T> && std::is_volatile_v<T>
  struct reflect<T>
  {
  private:
    static constexpr StringView _NAME = reflect<std::remove_cv_t<T>>::str();

  public:
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<
        details::CONST_STR,
        details::VOLATILE_STR,
        _NAME
      >;
    }

    using members_type = typename refl::members_type_t<std::remove_cv_t<T>>::template apply<std::add_cv>;

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>    
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_members(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      reflect<std::remove_reference_t<T>>::apply_on_methods(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }
  };
  
  template <typename F, typename... Ts>
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

  namespace meta
  {
    /// @brief Tag struct for iterating over members
    struct MembersT{};
    /// @brief Tag struct for iterating over methods
    struct MethodsT{};
  }

  /// @brief Tag object for iterating over members
  inline constexpr meta::MembersT Members;

  /// @brief Tag object for iterating over methods
  inline constexpr meta::MethodsT Methods;

  template<typename T, typename F> requires meta::is_reflectable_v<T>
  /// @brief Calls 'f' with each registered member of 'of'
  /// @tparam F The lambda type
  /// @tparam T The object type
  /// @param  Helper type (usually 'Members')
  /// @param of The object whose members to access
  /// @param f The lambda function
  constexpr void for_each(meta::MembersT, T&& of, F&& f) noexcept
  {
    reflect<std::decay_t<T>>::apply_on_members(std::forward<T>(of), std::forward<F>(f));
  }

  template<typename T, typename F> requires meta::is_reflectable_v<T>
  /// @brief Calls 'f' with each registered methods of 'of'
  /// @tparam F The lambda type
  /// @tparam T The object type
  /// @param  Helper type (usually 'Methods')
  /// @param of The object whose members to access
  /// @param f The lambda function
  constexpr void for_each(meta::MethodsT, T&& of, F&& f) noexcept
  {
    reflect<std::decay_t<T>>::apply_on_methods(std::forward<T>(of), std::forward<F>(f));
  }
}

/// @brief Declares reflection informations for built-in 'TYPE'
#define DECLARE_BUILTIN_TYPE(TYPE) \
template<> \
struct clt::meta::is_reflectable<TYPE> \
{ \
  static constexpr bool value = true; \
}; \
\
template<> \
struct clt::refl::members_type<TYPE> \
{ \
  using type = meta::type_list<TYPE>; \
}; \
\
template<> \
struct clt::refl::unqualified_name<TYPE> \
{ \
  static constexpr StringView value = #TYPE; \
}; \
\
template<> \
struct clt::refl::apply_on_members<TYPE> \
{ \
  template<typename F> requires std::is_invocable_v<F, TYPE> \
  constexpr void operator()(TYPE&& obj, F&& fn) const \
    noexcept(std::is_nothrow_invocable_v<F, TYPE>) \
  { \
    fn(obj); \
  } \
}

/***************************
* Declare all built-in types
****************************/

DECLARE_BUILTIN_TYPE(char);
DECLARE_BUILTIN_TYPE(char8);
DECLARE_BUILTIN_TYPE(char16);
DECLARE_BUILTIN_TYPE(char32);
DECLARE_BUILTIN_TYPE(i8);
DECLARE_BUILTIN_TYPE(u8);
DECLARE_BUILTIN_TYPE(i16);
DECLARE_BUILTIN_TYPE(u16);
DECLARE_BUILTIN_TYPE(i32);
DECLARE_BUILTIN_TYPE(u32);
DECLARE_BUILTIN_TYPE(i64);
DECLARE_BUILTIN_TYPE(u64);
DECLARE_BUILTIN_TYPE(f32);
DECLARE_BUILTIN_TYPE(f64);

//We no longer need this macro
#undef DECLARE_BUILTIN_TYPE

#define COLT_DETAILS_MEMBER_TO_MEMBER_PTR(type, name) , decltype(&type::name)
#define COLT_DETAILS_IF_CONSTEXPR_IS_MEMBER_CALL_LAMBDA(TYPE, member) if constexpr (std::is_member_object_pointer_v<decltype(&TYPE::member)>) fn(obj.member);

/// @brief Adds the necessary friends declarations for reflection
#define COLT_ENABLE_REFLECTION() template<typename T> friend struct clt::refl

/// @brief Enables reflection on a type.
/// ~~~~~~~~~~~~~~{.cpp}
/// class Test
/// {
///   u64 a;
///   u64 b;
/// 
///   COLT_ENABLE_REFLECTION();
/// };
/// COLT_DECLARE_TYPE(Test, a, b);
/// ~~~~~~~~~~~~~~
#define COLT_DECLARE_TYPE(TYPE, member, ...) \
  template<> \
  struct clt::refl<TYPE> { \
    static constexpr clt::StringView str() noexcept { return #TYPE; } \
    using unqualified_refl = refl<std::remove_cv_t<TYPE>>; \
    using members_type = typename clt::meta::type_list<decltype(&TYPE::member) COLT_FOR_EACH_1ARG(COLT_DETAILS_MEMBER_TO_MEMBER_PTR, TYPE, __VA_ARGS__)> \
      ::template remove_if<std::is_member_function_pointer>; \
    using methods_type = typename clt::meta::type_list<decltype(&TYPE::member) COLT_FOR_EACH_1ARG(COLT_DETAILS_MEMBER_TO_MEMBER_PTR, TYPE, __VA_ARGS__)> \
      ::template remove_if<std::is_member_object_pointer>; \
    template<typename On, typename F> requires std::same_as<std::decay_t<On>, std::decay_t<TYPE>>\
    static constexpr void apply_on_members(On&& obj, F&& fn) { \
        if constexpr (std::is_member_object_pointer_v<decltype(&TYPE::member)>) \
          fn(obj.member); \
        COLT_FOR_EACH_1ARG(COLT_DETAILS_IF_CONSTEXPR_IS_MEMBER_CALL_LAMBDA, TYPE, __VA_ARGS__) \
    } \
  }

#endif //!HG_COLT_REFL