#ifndef HG_COLT_REFL
#define HG_COLT_REFL

#include "../meta/traits.h"

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
    /// @brief Check if a type does not provide reflection capabilities
    concept NonReflectable = std::same_as<decltype(refl<T>::unspecialized), bool>;

    template<typename T>
    /// @brief Check if a type provides reflection capabilities
    concept Reflectable = (!NonReflectable<T>);
  }
}

#endif //!HG_COLT_REFL