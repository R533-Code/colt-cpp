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
    concept Reflectable = std::same_as<decltype(refl<T>::str()), StringView>;
  }
  
  template<typename T> requires std::is_pointer_v<T>
    && meta::Reflectable<std::remove_pointer_t<T>>
    && (!std::is_const_v<T>) && (!std::is_volatile_v<T>)
    struct refl<T>
  {
  private:
    static constexpr StringView ptr_str = "PTR<";
    static constexpr StringView name_str = refl<std::remove_pointer_t<T>>::str();
    static constexpr StringView end_str = ">";

  public:
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<ptr_str, name_str, end_str>;
    }

  private:
    using members_from_decay_t = typename refl<std::remove_cv_t<std::remove_pointer_t<T>>>::members_type;

  public:
    using members_type = typename members_from_decay_t::template apply<std::add_pointer>;
  };

  template<typename T> requires meta::Reflectable<std::remove_cv_t<T>>
    && std::is_const_v<T> && (!std::is_volatile_v<T>)
  struct refl<T>
  {
  private:
    static constexpr StringView const_str = "const ";
    static constexpr StringView name_str = refl<std::remove_cv_t<T>>::str();

  public:
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<const_str, name_str>;
    }

  private:
    using members_from_decay_t = typename refl<std::remove_cv_t<T>>::members_type;

  public:
    using members_type = typename members_from_decay_t::template apply<std::add_const>;
  };

  template<typename T> requires meta::Reflectable<std::remove_cv_t<T>>
    && std::is_volatile_v<T> && (!std::is_const_v<T>)
  struct refl<T>
  {
  private:
    static constexpr StringView volatile_str = "volatile ";
    static constexpr StringView name_str = refl<std::remove_cv_t<T>>::str();

  public:
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<volatile_str, name_str>;
    }

  private:
    using members_from_decay_t = typename refl<std::remove_cv_t<T>>::members_type;

  public:
    using members_type = typename members_from_decay_t::template apply<std::add_volatile>;
  };

  template<typename T> requires meta::Reflectable<std::remove_cv_t<T>>
    && std::is_const_v<T> && std::is_volatile_v<T>
  struct refl<T>
  {
  private:
    static constexpr StringView cv_str = "const volatile ";
    static constexpr StringView name_str = refl<std::remove_cv_t<T>>::str();

  public:
    static constexpr clt::StringView str() noexcept
    {
      return meta::join_strv_v<cv_str, name_str>;
    }

  private:
    using members_from_decay_t = typename refl<std::remove_cv_t<T>>::members_type;

  public:
    using members_type = typename members_from_decay_t::template apply<std::add_cv>;
  };  
}

#define DECLARE_BUILTIN_TYPE(type) \
  template<> \
  struct clt::refl<type> { \
    static constexpr clt::StringView str() noexcept { return #type; } \
    using members_type = clt::meta::type_list<type>; \
    template<typename On, typename F> requires std::same_as<On, std::decay_t<type>>\
    static constexpr void apply_on_members(On&& obj, F&& fn) { \
        fn(obj); \
    } \
    template<typename On, typename F> requires std::same_as<On, std::decay_t<type>> \
    static constexpr void apply_on_methods(On&& obj, F&& fn) { \
    } \
  }

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

#endif //!HG_COLT_REFL