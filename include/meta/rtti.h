/** @file rtti.h
* Contains custom run-time type informations to help with down-casts.
*/

#ifndef HG_COLT_RTTI
#define HG_COLT_RTTI

#include "./traits.h"
#include "../util/contracts.h"

namespace clt
{
  namespace meta
  {
    template<typename T>
    /// @brief Check if 'T' implements the necessary helpers to be used by 'dyn_cast'
    concept DynCastable =
      std::is_same_v<
      std::decay_t<decltype(std::declval<std::add_const_t<T>*>()->classof())>,
      std::decay_t<decltype(std::add_const_t<T>::classof_v)>>;

    template<typename From, typename To>
    /// @brief Check if 'From' is dynamically cast-able to 'To'
    concept DynCastableTo =
      DynCastable<To> &&
      std::is_base_of_v<From, To> &&
      std::is_same_v<
      decltype(std::declval<From*>()->classof()),
      decltype(std::declval<To*>()->classof())>;
  }

  template<typename To, typename From> requires meta::DynCastableTo<From, std::remove_pointer_t<To>>
  /// @brief Tries to dynamically cast from 'From' to 'To' returning nullptr if 'from' is not a 'To'.
  /// This allows to verify down-casts.
  /// @tparam To The type to cast to.
  /// This type does not need to be a pointer with correctly qualified const.
  /// This function will itself return a pointer to a 'To' with const qualifications
  /// of 'from'.
  /// @tparam From The type to cast from.
  /// @param from The pointer to try cast
  /// @return A valid pointer or nullptr if 'from' is not a 'To'
  [[nodiscard]]
  constexpr std::add_pointer_t<meta::match_cv_t<From, std::remove_pointer_t<To>>> dyn_cast(From* from) noexcept
  COLT_PRE(from != nullptr)
  {
    // Pointer Type to cast to (with const matching const of from)
    using ToP = std::add_pointer_t<meta::match_cv_t<From, std::remove_pointer_t<To>>>;
    // Type to cast to (without pointer)
    using ToN = std::remove_pointer_t<ToP>;
    if (from->classof() != ToN::classof_v)
      return static_cast<ToP>(nullptr);
    return static_cast<ToP>(from);
  }
  COLT_POST();

  template<typename To, typename From> requires meta::DynCastableTo<From, std::remove_pointer_t<To>>
  /// @brief Check if 'from' is of type 'To'
  /// @tparam To The type to check equality with
  /// @tparam From The type of the object whose type to check
  /// @param from The pointer whose true type to check
  /// @return True if 'from' is a 'To' else false
  [[nodiscard]]
  constexpr bool is_a(From* from) noexcept
  COLT_PRE(from != nullptr)
  {
    // Type to cast to (without pointer)
    using ToN = meta::match_cv_t<From, std::remove_pointer_t<To>>;
    return from->classof() == ToN::classof_v;
  }
  COLT_POST();
}

#endif //!HG_COLT_RTTI