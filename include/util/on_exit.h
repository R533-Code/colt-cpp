/** @file enum.h
* Contains ON_SCOPE_EXIT helper.
*/

#ifndef HG_COLT_ON_EXIT
#define HG_COLT_ON_EXIT

namespace clt::details
{
  template<typename Fun>
  struct ScopeGuard
  {
    Fun fn;

    ScopeGuard(Fun&& fn) noexcept
      : fn(std::forward<Fun>(fn)) {}

    ~ScopeGuard() noexcept { fn(); }
  };

  enum class ScopeGuardOnExit {};
  
  template<typename Fun>
  ScopeGuard<Fun> operator+(ScopeGuardOnExit, Fun&& fn) noexcept
  {
    return ScopeGuard<Fun>(std::forward<Fun>(fn));
  }
}

/// @brief Register an action to be called at the end of the scope.
/// Example:
/// ```c++
/// {
///		auto hello = 10;
///		ON_SCOPE_EXIT {
///			std::cout << hello;
///		}; // <- do not forget the semicolon
/// }
/// ```
#define ON_SCOPE_EXIT auto COLT_CONCAT(SCOPE_EXIT_HELPER, __LINE__) = clt::details::ScopeGuardOnExit() + [&]() 

#endif //!HG_COLT_ON_EXIT