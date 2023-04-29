#ifndef HG_COLT_COLTCPP
#define HG_COLT_COLTCPP

#include "io/print.h"
#include "io/input.h"

#include "util/assert_true.h"
#include "util/contracts.h"
#include "util/debug_level.h"
#include "util/on_exit.h"
#include "util/benchmark.h"
#include "util/params.h"
#include "util/volatile.h"
#include "util/macro.h"

#include "meta/type_list.h"
#include "meta/rtti.h"
#include "meta/string_literal.h"
#include "meta/traits.h"
#include "meta/type_list.h"

#include "structs/string.h"
#include "structs/vector.h"
#include "structs/string_view.h"
#include "structs/expect.h"
#include "structs/option.h"

#include "str/distance.h"
#include "str/distance.h"

#include "refl/refl.h"
#include "refl/enum.h"

#include "math/math.h"

#include "cmd/parse_args.h"

namespace clt
{
  template<typename Target, typename Input>
  [[nodiscard]]
  /// @brief Short-hand for static_cast<Target>(Input), with some debug checks
  /// @tparam Target The resulting type
  /// @tparam Input The type to convert from
  /// @param input The value to convert
  /// @return static_cast<Target>(input)
  constexpr Target as(Input&& input)
  {
    if constexpr (is_debug())
    {
      // DYNAMIC CASTING CHECK
      if constexpr (std::is_pointer_v<Target>
        && std::is_pointer_v<std::decay_t<Input>>
        && meta::DynCastableTo<std::remove_pointer_t<std::decay_t<Input>>, std::remove_pointer_t<std::decay_t<Target>>>)
      {
        if (is_a<Target>(input))
          return static_cast<Target>(std::forward<Input>(input));
        colt_unreachable("'as' conversion failed as true type did not match the expected one!");
      }
      // NUMERICAL TRUNCATION CHECK
      else if constexpr (std::is_integral_v<Target>
        && std::is_integral_v<Input>
        && sizeof(Target) < sizeof(Input))
      {
        if (input > std::numeric_limits<Target>::max())
          colt_unreachable("'as' conversion failed as resulting value would be truncated!");
        else if (input < std::numeric_limits<Target>::min())
          colt_unreachable("'as' conversion failed as resulting value would be truncated!");
      }
    }
    return static_cast<Target>(std::forward<Input>(input));
  }

  /// @brief Install default handlers used by the library
  inline void install_colt_handlers() noexcept
  {
#ifndef COLT_NO_PROFILE
    std::atexit([]() { clt::bench::save_tracing_to("tracing.json"); });
#endif
    clt::mem::GlobalAllocator.register_on_null([]() noexcept { io::print_fatal("Not enough memory! Aborting..."); });
  }
}

#endif //!HG_COLT_CPP