/** @file Option.h
* Contains the Option class.
* Option<T> can be either a value or None.
*/

#ifndef HG_COLT_OPTIONAL
#define HG_COLT_OPTIONAL

#include "./helper.h"

namespace clt
{
  template<typename T>
  /// @brief Manages an optionally contained value.
  /// @tparam T The optional type to hold
  class Option
  {
    /// @brief Buffer for the optional object
    alignas(T) char opt_buffer[sizeof(T)];
    /// @brief True if no object is contained
    bool is_none_v;

  public:
    /// @brief Destroy the stored value if it exists, and sets the Option to an empty one.
    /// Called automatically by the destructor.
    constexpr void reset()
      noexcept(std::is_nothrow_destructible_v<T>)
    {
      if (!is_none_v)
      {
        details::ptr_to<T*>(opt_buffer)->~T();
        is_none_v = true;
      }
    }

    /// @brief Constructs an empty Option.
    constexpr Option() noexcept
      : is_none_v(true) {}

    /// @brief Constructs an empty Option.
    /// Same as Option().
    /// @param  NoneT: use None
    constexpr Option(meta::NoneT) noexcept
      : is_none_v(true) {}

    /// @brief Copy constructs an object into the Option.
    /// @param to_copy The object to copy
    constexpr Option(meta::copy_trivial_t<const T&> to_copy)
      noexcept(std::is_nothrow_copy_constructible_v<T>)
      : is_none_v(false)
    {
      new(opt_buffer) T(to_copy);
    }

    /// @brief Move constructs an object into the Option
    /// @param to_move The object to move
    constexpr Option(T&& to_move)
      noexcept(std::is_nothrow_move_constructible_v<T>)
      requires (!std::is_trivial_v<T>)
    : is_none_v(false)
    {
      new(opt_buffer) T(std::move(to_move));
    }

    template<typename... Args>
    /// @brief Constructs an object into the Option directly.
    /// @tparam ...Args The parameter pack
    /// @param  InPlaceT, use InPlace
    /// @param ...args The argument pack
    constexpr Option(meta::InPlaceT, Args&&... args)
      noexcept(std::is_nothrow_constructible_v<T, Args...>)
      : is_none_v(false)
    {
      new(opt_buffer) T(std::forward<Args>(args)...);
    }

    /// @brief Copy constructor.
    /// @param to_copy The Option to copy
    constexpr Option(const Option& to_copy)
      noexcept(std::is_nothrow_copy_constructible_v<T>)
      : is_none_v(to_copy.is_none_v)
    {
      if (!is_none_v)
        new(opt_buffer) T(*details::ptr_to<const T*>(to_copy.opt_buffer));
    }

    /// @brief Move constructor.
    /// @param to_move The Option to move
    constexpr Option(Option&& to_move)
      noexcept(std::is_nothrow_move_constructible_v<T>)
      : is_none_v(to_move.is_none_v)
    {
      if (!is_none_v)
        new(opt_buffer) T(std::move(*details::ptr_to<T*>(to_move.opt_buffer)));
    }

    constexpr Option& operator=(const Option& to_copy)
      noexcept(std::is_nothrow_destructible_v<T>
        && std::is_nothrow_copy_constructible_v<T>)
    {
      reset();
      if (to_copy.is_value())
        new(opt_buffer) T(*details::ptr_to<const T*>(to_copy.opt_buffer));
    }

    constexpr Option& operator=(Option&& to_move)
      noexcept(std::is_nothrow_destructible_v<T>
        && std::is_nothrow_move_constructible_v<T>)
    {
      reset();
      if (to_move.is_value())
        new(opt_buffer) T(std::move(*details::ptr_to<T*>(to_move.opt_buffer)));
    }

    /// @brief Resets the Option.
    /// Same as `reset()`.
    /// @param  Error type (Error)
    /// @return Reference to self
    constexpr Option& operator=(meta::NoneT)
      noexcept(std::is_nothrow_destructible_v<T>)
    {
      reset();
    }

    /// @brief Destructor, destructs the value if it exist.
    constexpr ~Option()
      noexcept(std::is_nothrow_destructible_v<T>)
    {
      reset()
    }

    /// @brief Check if the Option contains a value.
    /// @return True if the Option contains a value
    explicit constexpr operator bool() const noexcept { return !is_none_v; }

    /// @brief Check if the Option contains a value.
    /// Same as !is_none().
    /// @return True if the Option contains a value
    constexpr bool is_value() const noexcept { return !is_none_v; }

    /// @brief Check if the Option does not contain a value.
    /// Same as !is_value().
    /// @return True if the Option does not contain a value
    constexpr bool is_none() const noexcept { return is_none_v; }

    /// @brief Returns the stored value.
    /// @return The value
    constexpr const T* operator->() const noexcept
      COLT_PRE(this->is_value())
      return details::ptr_to<const T*>(opt_buffer);
    COLT_POST()
    
    /// @brief Returns the stored value.
    /// @return The value
    constexpr T* operator->() noexcept
      COLT_PRE(this->is_value())
      return details::ptr_to<T*>(opt_buffer);
    COLT_POST()

    /// @brief Returns the stored value.
    /// @return The value.
    constexpr meta::copy_trivial_t<const T&> operator*() const& noexcept
      COLT_PRE(this->is_value())
      return *details::ptr_to<const T*>(opt_buffer);
    COLT_POST()

    /// @brief Returns the stored value.
    /// @return The value.
    constexpr T& operator*() & noexcept
      COLT_PRE(this->is_value())
      return *details::ptr_to<T*>(opt_buffer);
    COLT_POST()

    /// @brief Returns the stored value.
    /// @return The value.
    constexpr meta::copy_trivial_t<const T&&> operator*() const&& noexcept
      COLT_PRE(this->is_value())
      return *details::ptr_to<const T*>(opt_buffer);
    COLT_POST()
    
    /// @brief Returns the stored value.
    /// @return The value.
    /// @pre is_value() (colt_optional_is_value).
    constexpr T&& operator*() && noexcept
      COLT_PRE(this->is_value())
      return std::move(*details::ptr_to<T*>(opt_buffer));
    COLT_POST()

    /// @brief Returns the stored value.
    /// @return The value.
    constexpr meta::copy_trivial_t<const T&> value() const& noexcept
      COLT_PRE(this->is_value())
      return *details::ptr_to<const T*>(opt_buffer);
    COLT_POST()

    /// @brief Returns the stored value.
    /// @return The value.
    constexpr T& value() & noexcept
      COLT_PRE(this->is_value())
      return *details::ptr_to<T*>(opt_buffer);
    COLT_POST()

    /// @brief Returns the stored value.
    /// @return The value.
    constexpr meta::copy_trivial_t<const T&&> value() const&& noexcept
      COLT_PRE(this->is_value())
      return *details::ptr_to<const T*>(opt_buffer);
    COLT_POST()
    
    /// @brief Returns the stored value.
    /// @return The value.
    /// @pre is_value() (colt_optional_is_value).
    constexpr T&& value() && noexcept
      COLT_PRE(this->is_value())
      return std::move(*details::ptr_to<T*>(opt_buffer));
    COLT_POST()

    /// @brief Returns the value if contained, else 'default_value'
    /// @param default_value The value to return if the Option is None
    /// @return The value or 'default_value'
    constexpr T value_or(T&& default_value) const&
    {
      return is_none_v ? static_cast<T>(std::forward<T>(default_value)) : **this;
    }
    
    /// @brief Returns the value if contained, else 'default_value'
    /// @param default_value The value to return if the Option is None
    /// @return The value or 'default_value'
    constexpr T value_or(T&& default_value)&&
    {
      return is_none_v ? static_cast<T>(std::forward<T>(default_value)) : std::move(**this);
    }
  };
}

#endif //!HG_COLT_OPTIONAL