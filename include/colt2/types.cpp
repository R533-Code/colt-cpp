#include "types.h"
#include <cstdlib>
#include <cstdio>

void clt::debug_break() noexcept
{
  if (clt::is_debugger_present())
    clt::breakpoint();
  else
    std::abort();
}

void clt::unreachable(const char* error, clt::source_location src)
{
  if constexpr (is_debug_build())
  {
    printf(
        "FATAL: Unreachable branch hit in function '%s' (line %u) in "
        "file:\n'%s'\n%s",
        src.function_name(), src.line(), src.file_name(), error);
  }
  debug_break(); // does not guarantee no return...
  std::abort();
}
