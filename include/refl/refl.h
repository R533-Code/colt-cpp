#ifndef HG_COLT_REFL
#define HG_COLT_REFL

#include "../meta/string_literal.h"
#include "../meta/traits.h"
#include "../meta/type_list.h"
#include "../structs/string_view.h"

namespace clt
{
  template<typename T>
  /// @brief Unspecialized reflection informations
  /// @tparam T The type on which to reflect
  struct refl
  {
    /// @brief Member to detect if 'refl' is not specialized
    static constexpr bool unspecialized = true;
  };

  namespace meta
  {
    template<typename T>
    /// @brief Check if a type provides reflection capabilities
    concept Reflectable = std::same_as<decltype(clt::refl<T>::str()), clt::StringView>;
  }
  
  template<typename T> requires std::is_pointer_v<T>
    && meta::Reflectable<std::remove_pointer_t<T>>
    && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
  /// @brief Pointer overload for reflection information
  /// @tparam T The pointer type
  struct refl<T>
  {
  private:
    /// @brief Helper string for generating type name
    static constexpr StringView ptr_str = "PTR<";
    /// @brief Type name
    static constexpr StringView name_str = refl<std::remove_pointer_t<T>>::str();
    /// @brief Helper string for generating type name
    static constexpr StringView end_str = ">";

  public:
    /// @brief Returns the qualified type name
    /// @return StringView over the qualified type name
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<ptr_str, name_str, end_str>;
    }

  public:
    /// @brief The unqualified reflected type
    using unqualified_refl = refl<std::remove_cv_t<T>>;
    /// @brief The only member is the pointer itself
    using members_type = typename meta::type_list<T>;

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    /// @brief Applies a lambda on each member of an object
    /// @tparam On The object type
    /// @tparam F The lambda type
    /// @param obj The object
    /// @param fn The lambda
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      fn(obj);
    }

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    /// @brief Applies a lambda on each method of an object
    /// @tparam On The object type
    /// @tparam F The lambda type
    /// @param obj The object
    /// @param fn The lambda
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      //Pointers do not have methods... Do nothing.
    }
  };

  template<typename T> requires meta::Reflectable<std::remove_cv_t<T>>
    && std::is_const_v<T> && (!std::is_volatile_v<T>)
  struct refl<T>
  {
  private:
    /// @brief Helper string for qualifications
    static constexpr StringView cv_str = "const ";
    /// @brief Type name
    static constexpr StringView name_str = refl<std::remove_cv_t<T>>::str();

  public:
    /// @brief Returns the qualified type name
    /// @return StringView over the qualified type name
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<cv_str, name_str>;
    }

  private:
    /// @brief Member type list of unqualified reflection informations
    using members_from_decay_t = typename refl<std::remove_cv_t<T>>::members_type;

  public:
    /// @brief The unqualified reflected type
    using unqualified_refl = refl<std::remove_cv_t<T>>;

    /// @brief The qualified members type list
    using members_type = typename members_from_decay_t::template apply<std::add_const>;

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    /// @brief Applies a lambda on each member of an object
    /// @tparam On The object type
    /// @tparam F The lambda type
    /// @param obj The object
    /// @param fn The lambda
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      unqualified_refl::apply_on_members(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    /// @brief Applies a lambda on each method of an object
    /// @tparam On The object type
    /// @tparam F The lambda type
    /// @param obj The object
    /// @param fn The lambda
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      unqualified_refl::apply_on_methods(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }
  };

  template<typename T> requires meta::Reflectable<std::remove_cv_t<T>>
    && std::is_volatile_v<T> && (!std::is_const_v<T>)
  struct refl<T>
  {
  private:
    /// @brief Helper string for qualifications
    static constexpr StringView cv_str = "volatile ";
    /// @brief Type name
    static constexpr StringView name_str = refl<std::remove_cv_t<T>>::str();

  public:
    /// @brief Returns the qualified type name
    /// @return StringView over the qualified type name
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<cv_str, name_str>;
    }

  private:
    /// @brief Member type list of unqualified reflection informations
    using members_from_decay_t = typename refl<std::remove_cv_t<T>>::members_type;

  public:
    /// @brief The unqualified reflected type
    using unqualified_refl = refl<std::remove_cv_t<T>>;

    /// @brief The qualified members type list
    using members_type = typename members_from_decay_t::template apply<std::add_volatile>;

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    /// @brief Applies a lambda on each member of an object
    /// @tparam On The object type
    /// @tparam F The lambda type
    /// @param obj The object
    /// @param fn The lambda
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      unqualified_refl::apply_on_members(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    /// @brief Applies a lambda on each method of an object
    /// @tparam On The object type
    /// @tparam F The lambda type
    /// @param obj The object
    /// @param fn The lambda
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      unqualified_refl::apply_on_methods(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }
  };

  template<typename T> requires meta::Reflectable<std::remove_cv_t<T>>
    && std::is_const_v<T> && std::is_volatile_v<T>
  struct refl<T>
  {
  private:
    /// @brief Helper string for qualifications
    static constexpr StringView cv_str = "const volatile ";
    /// @brief Type name
    static constexpr StringView name_str = refl<std::remove_cv_t<T>>::str();

  public:
    /// @brief Returns the qualified type name
    /// @return StringView over the qualified type name
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<cv_str, name_str>;
    }

  private:
    /// @brief Member type list of unqualified reflection informations
    using members_from_decay_t = typename refl<std::remove_cv_t<T>>::members_type;

  public:
    /// @brief The unqualified reflected type
    using unqualified_refl = refl<std::remove_cv_t<T>>;
    
    /// @brief The qualified members type list
    using members_type = typename members_from_decay_t::template apply<std::add_cv>;

    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    /// @brief Applies a lambda on each member of an object
    /// @tparam On The object type
    /// @tparam F The lambda type
    /// @param obj The object
    /// @param fn The lambda
    static constexpr void apply_on_members(On&& obj, F&& fn)
    {
      unqualified_refl::apply_on_members(
        std::forward<On>(obj), std::forward<F>(fn)
      );
    }
    
    template<typename On, typename F> requires std::same_as<On, std::decay_t<T>>
    /// @brief Applies a lambda on each method of an object
    /// @tparam On The object type
    /// @tparam F The lambda type
    /// @param obj The object
    /// @param fn The lambda
    static constexpr void apply_on_methods(On&& obj, F&& fn)
    {
      unqualified_refl::apply_on_methods(
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

  template<typename T, typename F> requires meta::Reflectable<T>
  /// @brief Calls 'f' with each registered member of 'of'
  /// @tparam F The lambda type
  /// @tparam T The object type
  /// @param  Helper type (usually 'Members')
  /// @param of The object whose members to access
  /// @param f The lambda function
  constexpr void for_each(meta::MembersT, T&& of, F&& f) noexcept
  {
    refl<std::decay_t<T>>::apply_for_members(std::forward<T>(of), std::forward<F>(f));
  }

  template<typename T, typename F> requires meta::Reflectable<T>
  /// @brief Calls 'f' with each registered methods of 'of'
  /// @tparam F The lambda type
  /// @tparam T The object type
  /// @param  Helper type (usually 'Methods')
  /// @param of The object whose members to access
  /// @param f The lambda function
  constexpr void for_each(meta::MethodsT, T&& of, F&& f) noexcept
  {
    refl<std::decay_t<T>>::apply_for_methods(std::forward<T>(of), std::forward<F>(f));
  }
}

#define DECLARE_BUILTIN_TYPE(type) \
  template<> \
  struct clt::refl<type> { \
    static constexpr clt::StringView str() noexcept { return #type; } \
    using unqualified_refl = refl<std::remove_cv_t<type>>; \
    using members_type = clt::meta::type_list<type>; \
    template<typename On, typename F> requires std::same_as<On, std::decay_t<type>>\
    static constexpr void apply_on_members(On&& obj, F&& fn) { \
        fn(obj); \
    } \
    template<typename On, typename F> requires std::same_as<On, std::decay_t<type>> \
    static constexpr void apply_on_methods(On&& obj, F&& fn) { \
    } \
  }

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

#undef DECLARE_BUILTIN_TYPE

#define COLT_DECLARE_TYPE(type, ...) \
  template<> \
  struct clt::refl<type> { \
  }

template<typename T> requires (!fmt::is_formattable<T>::value) && clt::meta::Reflectable<T>
struct fmt::formatter<T>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    auto end = ctx.end();
    assert_true("Possible format for Reflect-able is: {}!", it == end);
    return it;
  }

  template<typename FormatContext>
  auto format(const T& obj, FormatContext& ctx)
  {
    auto out = ctx.out();
    out = fmt::format_to(out, "{{\n");
    /*clt::for_each(clt::Members, obj,
      [&out, i = 0ULL]<typename T>(T&& a) mutable
      {
        out = fmt::format_to(out, "  {}: {}\n",
          clt::refl<std::decay_t<T>>::str(), std::forward<T>(a));
      }
    );*/
    out = fmt::format_to(out, "}}\n");
    return ;
  }
};

#endif //!HG_COLT_REFL