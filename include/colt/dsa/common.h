#ifndef HG_DSA_COMMON
#define HG_DSA_COMMON

#include <concepts>
#include <type_traits>

#include "colt/num/typedefs.h"
#include "colt/macro/assert.h"

namespace clt
{
  template<typename To, typename From>
  /// @brief Helper to converts a pointer to a type to a pointer to another type
  /// @tparam To The type to convert
  /// @tparam From The type to convert from
  /// @param frm The value to convert
  /// @return Converted value
  constexpr To ptr_to(From frm) noexcept
    requires std::is_pointer_v<To> && std::is_pointer_v<From>
  {
    return static_cast<To>(
        static_cast<meta::match_cv_t<std::remove_pointer_t<From>, void>*>(frm));
  }

  /// @brief Tag type for constructing an empty Option
  struct none_t
  {
  };

  /// @brief Tag object for constructing an empty Option
  static constexpr none_t None;

  /// @brief Tag struct for constructing an object in place
  struct in_place_t
  {
  };

  /// @brief Tag object for construting an object in place
  inline constexpr in_place_t InPlace;
} // namespace clt

#endif // !HG_DSA_COMMON
