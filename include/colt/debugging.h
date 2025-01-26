/*****************************************************************/ /**
 * @file   debugging.h
 * @brief  Contains utilities for programmatically adding breakpoints.
 * 
 * @author RPC
 * @date   January 2025
 *********************************************************************/
#ifndef _HG_COLT_DEBUGGING_
#define _HG_COLT_DEBUGGING_

#include <colt/macros.h>
#include <colt/coltcpp_export.h>

namespace clt
{
  /// @brief Attempts to temporarily stop program execution and pass control
  /// to the debugger, regardless of whether the presence of a debugger can
  /// be detected.
  COLTCPP_EXPORT void breakpoint() noexcept;
  /// @brief Attempts to determine if the program is being executed with
  /// debugger present.
  /// @return Returns true if, to the best of its knowledge, the program is
  /// executed under a debugger.
  COLTCPP_EXPORT bool is_debugger_present() noexcept;
  /// @brief Attempts to transfer control to the debugger if it is present,
  /// no-op otherwise.
  COLTCPP_EXPORT void breakpoint_if_debugging() noexcept;
} // namespace clt

#endif // !_HG_COLT_DEBUGGING_
