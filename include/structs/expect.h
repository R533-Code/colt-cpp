#ifndef HG_COLT_EXPECT
#define HG_COLT_EXPECT

#include "./helper.h"

namespace clt
{
  template<typename ExpectedTy, typename ErrorTy>
  /// @brief A helper class that can hold either a valid value or an error.
  /// This class can be seen as an Optional, that carries error informations.
  /// To document source code more, to constructs an Expected containing
  /// an error, clt::Error is passed to the constructor.
  /// The default constructor of an Expected default constructs an expected value:
  /// While this might seen counterintuitive, this class is named Expected, as it
  /// will most likely contain the expected value, not an error.
  ///
  /// Example Usage:
  /// ```C++
  /// Expected<int, const char*> div(int a, int b)
  /// {
  ///   if (b != 0)
  ///     return a / b;
  ///   return { Error, "Division by zero is prohibited!" };
  /// }
  /// ```
  /// @tparam ExpectedTy The expected type
  /// @tparam ErrorTy The error type
  class Expected
  {
    /// @brief Buffer for both error type and expected value
    union
    {
      /// @brief The expected value (active when is_error_v == false)
      ExpectedTy expected;
      /// @brief The error value (active when is_error_v == true)
      ErrorTy error_v;
    };

    /// @brief True if an error is stored in the Expected
    bool is_error_v;

  public:
    /// @brief Default constructs an error in the Expected
    /// @param  ErrorT tag
    constexpr Expected(meta::ErrorT)
      noexcept(std::is_nothrow_constructible_v<ErrorTy>)
      : is_error_v(true)
    {
      new(&error_v) ErrorTy();
    }

    /// @brief Copy constructs an error in the Expected
    /// @param  ErrorT tag
    /// @param value The value to copy
    constexpr Expected(meta::ErrorT, meta::copy_trivial_t<const ErrorTy&> value)
      noexcept(std::is_nothrow_copy_constructible_v<ErrorTy>)
      : is_error_v(true)
    {
      new(&error_v) ErrorTy(value);
    }


    /// @brief Move constructs an error in the Expected
    /// @param  ErrorT tag
    /// @param to_move The value to move
    constexpr Expected(meta::ErrorT, ErrorTy&& to_move)
      noexcept(std::is_nothrow_move_constructible_v<ErrorTy>)
      requires (!std::is_trivial_v<ErrorTy>)
      : is_error_v(true)
    {
      new(&error_v) ErrorTy(std::move(to_move));
    }

    template<typename... Args>
    /// @brief Constructs an error in place in the Expected
    /// @tparam ...Args Parameter pack
    /// @param  InPlaceT tag
    /// @param  ErrorT tag
    /// @param ...args Argument pack forwarded to the constructor
    constexpr Expected(meta::InPlaceT, meta::ErrorT, Args&&... args)
      noexcept(std::is_nothrow_constructible_v<ErrorTy, Args...>)
      : is_error_v(true)
    {
      new(&error_v) ErrorTy(std::forward<Args>(args)...);
    }

    /// @brief Default constructs an expected value in the Expected
    constexpr Expected()
      noexcept(std::is_default_constructible_v<ExpectedTy>)
      : is_error_v(false)
    {
      new(&expected) ExpectedTy();
    }

    /// @brief Copy constructs an expected value in the Expected
    /// @param value The value to copy
    constexpr Expected(meta::copy_trivial_t<const ExpectedTy&> value)
      noexcept(std::is_nothrow_copy_constructible_v<ExpectedTy>)
      : is_error_v(false)
    {
      new(&expected) ExpectedTy(value);
    }

    /// @brief Move constructs an expected value in the Expected
    /// @param to_move The value to move
    constexpr Expected(ExpectedTy&& to_move)
      noexcept(std::is_nothrow_move_constructible_v<ExpectedTy>)
      requires (!std::is_trivial_v<ExpectedTy>)
    : is_error_v(false)
    {
      new(&expected) ExpectedTy(std::move(to_move));
    }

    template<typename... Args, typename Ty>
    /// @brief Constructs an expected value in place in the Expected
    /// @tparam ...Args Parameter pack
    /// @param  InPlaceT tag
    /// @param  ErrorT tag
    /// @param ...args Argument pack forwarded to the constructor
    constexpr Expected(meta::InPlaceT, Ty&& arg, Args&&... args)
      noexcept(std::is_nothrow_constructible_v<ExpectedTy, Ty, Args...>)
      requires (!std::is_same_v<Ty, meta::ErrorT>)
      : is_error_v(false)
    {
      new(&expected) ExpectedTy(std::forward<Ty>(arg), std::forward<Args>(args)...);
    }

    /// @brief Copy constructs an Expected
    /// @param copy The Expected to copy
    constexpr Expected(const Expected& copy)
      noexcept(std::is_nothrow_copy_constructible_v<ExpectedTy>
        && std::is_nothrow_copy_constructible_v<ErrorTy>)
      : is_error_v(copy.is_error_v)
    {
      if (is_error_v)
        new(&error_v) ErrorTy(copy.error_v);
      else
        new(&expected) ExpectedTy(copy.expected);
    }

    /// @brief Copy assignment operator
    /// @param copy The Expected to copy
    /// @return Self
    constexpr Expected& operator=(const Expected& copy)
      noexcept(std::is_nothrow_copy_constructible_v<ExpectedTy>
        && std::is_nothrow_copy_constructible_v<ErrorTy>
        && std::is_nothrow_destructible_v<ErrorTy>
        && std::is_nothrow_destructible_v<ExpectedTy>)
      COLT_PRE(&copy != this)
    {
      if (is_error_v)
        error_v.~ErrorTy();
      else
        expected.~ExpectedTy();

      is_error_v = copy.is_error_v;
      if (is_error_v)
        new(&error_v) ErrorTy(copy.error_v);
      else
        new(&expected) ExpectedTy(copy.expected);

      return *this;
    }
    COLT_POST()

    /// @brief Move constructs an Expected
    /// @param move The Expected to move
    constexpr Expected(Expected&& move)
      noexcept(std::is_nothrow_move_constructible_v<ExpectedTy>
        && std::is_nothrow_move_constructible_v<ErrorTy>)
      : is_error_v(move.is_error_v)
    {
      if (is_error_v)
        new(&error_v) ErrorTy(std::move(move.error_v));
      else
        new(&expected) ExpectedTy(std::move(move.expected));
    }

    /// @brief Move assignment operator
    /// @param move The Expected to move
    /// @return Self
    constexpr Expected& operator=(Expected&& move)
      noexcept(std::is_nothrow_move_constructible_v<ExpectedTy>
        && std::is_nothrow_move_constructible_v<ErrorTy>
        && std::is_nothrow_destructible_v<ErrorTy>
        && std::is_nothrow_destructible_v<ExpectedTy>)
      COLT_PRE(&move != this)
    {
      if (is_error_v)
        error_v.~ErrorTy();
      else
        expected.~ExpectedTy();

      is_error_v = move.is_error_v;
      if (is_error_v)
        new(&error_v) ErrorTy(std::move(move.error_v));
      else
        new(&expected) ExpectedTy(std::move(move.expected));

      return *this;
    }
    COLT_POST()

    /// @brief Destructs the value/error contained in the Expected
    constexpr ~Expected() noexcept(std::is_nothrow_destructible_v<ExpectedTy>
      && std::is_nothrow_destructible_v<ErrorTy>)
    {
      if (is_error_v)
        error_v.~ErrorTy();
      else
        expected.~ExpectedTy();
    }

    /// @brief Check if the Expected contains an error
    /// @return True if the Expected contains an error
    constexpr bool is_error() const noexcept { return is_error_v; }
    /// @brief Check if the Expected contains an expected value
    /// @return True if the Expected contains an expected value
    constexpr bool is_expect() const noexcept { return !is_error_v; }

    /// @brief Check if the Expected contains an error.
    /// Same as is_error().
    /// @return True if the Expected contains an error
    constexpr bool operator!() const noexcept { return is_error_v; }
    /// @brief Check if the Expected contains an expected value.
    /// Same as is_expected().
    /// @return True if the Expected contains an expected value
    explicit constexpr operator bool() const noexcept { return !is_error_v; }

    /// @brief Returns the stored Expected value.
    /// @return The Expected value
    constexpr const ExpectedTy* operator->() const noexcept
      COLT_PRE(is_expect())
      return &expected;
    COLT_POST()
    
    /// @brief Returns the stored Expected value.
    /// @return The Expected value
    constexpr ExpectedTy* operator->() noexcept
      COLT_PRE(is_expect())
      return &expected;
    COLT_POST()

    /// @brief Returns the stored Expected value.
    /// @return The Expected value.
    constexpr meta::copy_trivial_t<const ExpectedTy&> operator*()
      const& noexcept
      COLT_PRE(is_expect())
      return expected;
    COLT_POST()

    /// @brief Returns the stored Expected value.
    /// @return The Expected value.
    /// @pre is_expected() (colt_expected_is_expected).
    constexpr ExpectedTy& operator*()
      & noexcept
      COLT_PRE(is_expect())
      return expected;
    COLT_POST()
    
    /// @brief Returns the stored Expected value.
    /// @return The Expected value.
    constexpr meta::copy_trivial_t<const ExpectedTy&&> operator*()
      const&& noexcept
      COLT_PRE(is_expect())
      return expected;
    COLT_POST()
    
    /// @brief Returns the stored Expected value.
    /// @return The Expected value.
    constexpr ExpectedTy&& operator*()
      && noexcept
      COLT_PRE(is_expect())
      return std::move(expected);
    COLT_POST()

    /// @brief Returns the stored Expected value.
    /// @return The Expected value.
    constexpr meta::copy_trivial_t<const ExpectedTy&> value()
      const& noexcept
      COLT_PRE(is_expect())
      return expected;
    COLT_POST()

    /// @brief Returns the stored Expected value.
    /// @return The Expected value.
    constexpr ExpectedTy& value()
      & noexcept
      COLT_PRE(is_expect())
      return expected;
    COLT_POST()
    
    /// @brief Returns the stored Expected value.
    /// @return The Expected value.
    constexpr meta::copy_trivial_t<const ExpectedTy&&> value()
      const&& noexcept
      COLT_PRE(is_expect())
      return expected;
    COLT_POST()
    
    /// @brief Returns the stored Expected value.
    /// @return The Expected value.
    constexpr ExpectedTy&& value()
      && noexcept
      COLT_PRE(is_expect())
      return std::move(expected);
    COLT_POST()

    /// @brief Returns the stored error value.
    /// @return The error value.
    constexpr meta::copy_trivial_t<const ErrorTy&> error()
      const& noexcept
      COLT_PRE(is_error())
      return error_v;
    COLT_POST()

    /// @brief Returns the stored error value.
    /// @return The error value.
    constexpr ErrorTy& error()
      & noexcept
      COLT_PRE(is_error())
      return error_v;
    COLT_POST()
    
    /// @brief Returns the stored error value.
    /// @return The error value.
    constexpr meta::copy_trivial_t<const ErrorTy&&> error()
      const&& noexcept
      COLT_PRE(is_error())
      return error_v;
    COLT_POST()
    
    /// @brief Returns the stored error value.
    /// @return The error value.
    constexpr ErrorTy&& error()
      && noexcept
      COLT_PRE(is_error())
      return std::move(error_v);
    COLT_POST()

    /// @brief Returns the Expected value if contained, else 'default_value'
    /// @param default_value The value to return if the Expected contains an error
    /// @return The Expected value or 'default_value'
    constexpr ExpectedTy value_or(ExpectedTy && default_value) const&
    {
      return is_error_v ? static_cast<ExpectedTy>(std::forward<ExpectedTy>(default_value)) : **this;
    }
    /// @brief Returns the Expected value if contained, else 'default_value'
    /// @param default_value The value to return if the Expected contains an error
    /// @return The Expected value or 'default_value'
    constexpr ExpectedTy value_or(ExpectedTy&& default_value)&&
    {
      return is_error_v ? static_cast<ExpectedTy>(std::forward<ExpectedTy>(default_value)) : std::move(**this);
    }

    /// @brief Returns the expected value, or aborts if it does not exist.
    /// @param on_abort The function to call before aborting or null
    /// @return The expected value
    constexpr meta::copy_trivial_t<const ExpectedTy&> value_or_abort(void(*on_abort)(void) noexcept = nullptr) const& noexcept
    {
      if (is_error_v)
      {
        if (on_abort)
          on_abort();
        std::abort();
      }
      else
        return expected;
    }

    /// @brief Returns the expected value, or aborts if it does not exist.
    /// @param on_abort The function to call before aborting or null
    /// @return The expected value
    constexpr ExpectedTy& value_or_abort(void(*on_abort)(void) noexcept = nullptr) & noexcept
    {
      if (is_error_v)
      {
        if (on_abort)
          on_abort();
        std::abort();
      }
      else
        return expected;
    }
    
    /// @brief Returns the expected value, or aborts if it does not exist.
    /// @param on_abort The function to call before aborting or null
    /// @return The expected value
    constexpr meta::copy_trivial_t<const ExpectedTy&&> value_or_abort(void(*on_abort)(void) noexcept = nullptr) const&& noexcept
    {
      if (is_error_v)
      {
        if (on_abort)
          on_abort();
        std::abort();
      }
      else
        return expected;
    }
    /// @brief Returns the expected value, or aborts if it does not exist.
    /// @param on_abort The function to call before aborting or null
    /// @return The expected value
    constexpr ExpectedTy&& value_or_abort(void(*on_abort)(void) noexcept = nullptr) && noexcept
    {
      if (is_error_v)
      {
        if (on_abort)
          on_abort();
        std::abort();
      }
      else
        return std::move(expected);
    }
  };
}

#endif //!HG_COLT_EXPECT