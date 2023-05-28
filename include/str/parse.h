/** @file parse.h
* Contains parser<> template for most built-in type.
*/

#ifndef HG_COLT_PARSE
#define HG_COLT_PARSE

#include <charconv>
#include <concepts>
#include <limits>

#include "scn/scn.h"

#include "../structs/string_view.h"
#include "../util/params.h"
#include "../util/typedefs.h"
#include "../refl/enum.h"

namespace clt::meta
{
  template<Integral T>
  inline constexpr u64 max_digits10_v = static_cast<u64>(clt::ceil(clt::log10(std::numeric_limits<T>::max()))) + std::is_signed_v<T>;
}

#endif //!HG_COLT_PARSE