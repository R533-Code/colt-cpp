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
  void install_colt_handlers() noexcept
  {
#ifndef COLT_NO_PROFILE
    std::atexit([]() { clt::bench::save_tracing_to("tracing.json"); });
#endif
    clt::mem::GlobalAllocator.register_on_null([]() noexcept { io::print_fatal("Not enough memory! Aborting..."); });
  }
}

#endif //!HG_COLT_CPP