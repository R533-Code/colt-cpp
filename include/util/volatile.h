/** @file volatile.h
* Contains 'do_not_optimize()' and volatile_load/store helpers.
*/

#ifndef HG_COLT_VOLATILE
#define HG_COLT_VOLATILE

#include "../meta/traits.h"

namespace clt
{
  template<typename T> requires std::is_trivially_copyable_v<T>
  /// @brief Load a value from a pointer as if the load was volatile.
  /// @tparam T The type to load
  /// @param target The pointer from which to load
  /// @return The loaded value
  constexpr T volatile_load(const T* target)
  {
    return *static_cast<const volatile T*>(target);
  }

  template<typename T> requires std::is_trivially_copyable_v<T>
  /// @brief Store a value to a location as if the store was volatile.
  /// @tparam T The type to store
  /// @param target The location where to perform the store
  /// @param value The value to store
  constexpr void volatile_store(T* target, T value)
  {
    *static_cast<volatile T*>(target) = value;
  }

  namespace details
  {
#ifdef _MSC_VER
#pragma optimize("", off)
    /// @brief Sink function that cannot be optimized away
    /// @param  Unused parameter
    void do_not_optimize_sink(void const*) noexcept
    {
      //Used by do_not_optimize
    }
#pragma optimize("", on)

    template <typename T>
    /// @brief Prevents the argument from being optimized away
    /// @tparam T The type of the argument
    /// @param val The value of the argument
    void do_not_optimize(T const& val)
    {
      do_not_optimize_sink(&val);
    }
#else
    
    template <typename T>
    /// @brief Prevents the argument from being optimized away.
    /// Assembly from Google Benchmark.
    /// @tparam T The type of the argument
    /// @param val The value of the argument
    void do_not_optimize(T const& val)
    {
      asm volatile("" : : "r,m"(val) : "memory");
    }

    template <typename T>
    /// @brief Prevents the argument from being optimized away.
    /// Assembly from Google Benchmark.
    /// @tparam T The type of the argument
    /// @param val The value of the argument
    void do_not_optimize(T& val)
    {
#ifdef __clang__
      asm volatile("" : "+r,m"(val) : : "memory");
#else
      asm volatile("" : "+m,r"(val) : : "memory");
#endif
    }
#endif
  }

  template<typename T>
  /// @brief Prevents the argument from being optimized away by the compiler.
  /// Useful for benchmarks.
  /// @tparam T The type of the argument
  /// @param arg The value of the argument
  void do_not_optimize_away(T&& arg) noexcept
  {
    details::do_not_optimize(std::forward<T>(arg));
  }
}

#endif //!HG_COLT_VOLATILE