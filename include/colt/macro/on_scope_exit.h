/*****************************************************************/ /**
 * @file   on_scope_end.h
 * @brief  Contains `ON_SCOPE_EXIT{};` macro.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_MACRO_ON_SCOPE_EXIT
#define HG_MACRO_ON_SCOPE_EXIT

namespace clt::details
{
  template<typename Fun>
  /// @brief Helper for ON_SCOPE_EXIT
  struct ScopeGuard
  {
    /// @brief The function to execute
    Fun fn;

    /// @brief Takes in a lambda to execute
    /// @param fn The function to execute
    ScopeGuard(Fun&& fn) noexcept
        : fn(std::forward<Fun>(fn))
    {
    }

    /// @brief Runs the lambda in the destructor
    ~ScopeGuard() noexcept { fn(); }
  };

  /// @brief Helper for ON_SCOPE_EXIT
  enum class ScopeGuardOnExit
  {
  };

  template<typename Fun>
  /// @brief Helper for ON_SCOPE_EXIT
  ScopeGuard<Fun> operator+(ScopeGuardOnExit, Fun&& fn) noexcept
  {
    return ScopeGuard<Fun>(std::forward<Fun>(fn));
  }
} // namespace clt::details

/// @brief Register an action to be called at the end of the scope.
/// Example:
/// @code{.cpp}
/// {
///		auto hello = 10;
///		ON_SCOPE_EXIT {
///			std::cout << hello;
///		}; // <- do not forget the semicolon
/// }
/// @endcode
#define ON_SCOPE_EXIT                             \
  auto COLT_CONCAT(SCOPE_EXIT_HELPER, __LINE__) = \
      clt::details::ScopeGuardOnExit() + [&]()

#endif // !HG_MACRO_ON_SCOPE_EXIT
