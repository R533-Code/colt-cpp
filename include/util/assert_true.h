#ifndef HG_COLT_ASSERT_TRUE
#define HG_COLT_ASSERT_TRUE

#include "./contracts.h"

namespace clt::details
{
  /// @brief Used to cause a compile-time failure
  inline void constexpr_assert_true_failed() noexcept
  {
    //ASSERTION FAILED AT COMPILE TIME!
  }

  template<typename... BoolTs> requires (sizeof...(BoolTs) != 0)
  /// @brief Asserts that multiple conditions are true, and if not,
  /// stops the application and prints the failed assertions.
  /// @tparam ...BoolTs The type parameter pack
  /// @param fn_name The function name
  /// @param ...bools The Assertion pack
  constexpr void assert_true_multiple(const char* message, const char* fn_name, BoolTs... bools) noexcept
  {
    static_assert((std::is_same_v<Assertion, std::decay_t<BoolTs>> && ...),
      "This function expects 'Assertion'! Use assert_true rather than calling it directly!");
    if (std::is_constant_evaluated())
    {
      Assertion* array[sizeof...(BoolTs)] = { &bools... };
      for (size_t i = 0; i < sizeof...(BoolTs); i++)
      {
        if (array[i]->value)
          continue;
        constexpr_assert_true_failed();
      }
    }
    else if constexpr (ColtDebugLevel != COLT_NO_DEBUG)
    {
      Assertion* array[sizeof...(BoolTs)] = { &bools... };

      bool error = false;
      for (size_t i = 0; i < sizeof...(BoolTs); i++)
      {
        if (array[i]->value)
          continue;
        if (!error) {
          std::printf("ASSERTION FAILED: in function:\n\"%s\":\n%s\n", fn_name, message);
          error = true;
        }
        std::printf("%u) %s => false\n", static_cast<unsigned int>(i + 1), array[i]->str);
      }
      if (error)
        colt_intrinsic_dbreak();
    }
  }
}

/// @brief Asserts that all condition are true
#define assert_true(MESSAGE, COND, ...) clt::details::assert_true_multiple(MESSAGE, COLT_FUNCTION_NAME COLT_DETAILS_TO_ASSERTION(COND) COLT_FOR_EACH(COLT_DETAILS_TO_ASSERTION, __VA_ARGS__))

#endif //!HG_COLT_ASSERT_TRUE