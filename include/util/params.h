/** @file params.h
* Contains parameter helpers and uninit<> template.
*/

#ifndef HG_COLT_PARAMS
#define HG_COLT_PARAMS

#include <type_traits>

#include "../structs/helper.h"

namespace clt
{
  namespace details
  {
    template<typename To, typename From>
    /// @brief Helper to converts a pointer to a type to a pointer to another type
    /// @tparam To The type to convert
    /// @tparam From The type to convert from
    /// @param frm The value to convert
    /// @return Converted value
    constexpr To ptr_to(From frm) noexcept
      requires std::is_pointer_v<To>&& std::is_pointer_v<From>
    {
      return static_cast<To>(
        static_cast<
        meta::match_cv_t<std::remove_pointer_t<From>, void>*
        >(frm));
    }
  }

  template<typename T>
  class uninit
    : public meta::type_on_debug<bool>
  {
    using OnDebugIsConstructed = meta::type_on_debug<bool>;

    /// @brief Aligned buffer providing storage for the object
    alignas(T) char buffer[sizeof(T)];

  public:
    /// @brief Default copy constructor if trivially copyable
    constexpr uninit(const uninit&) noexcept requires std::is_trivially_default_constructible_v<T> = default;
    /// @brief No copy constructor if not trivially copyable
    constexpr uninit(const uninit&) noexcept = delete;
    /// @brief Default copy constructor if trivially copyable
    constexpr uninit(uninit&&) noexcept requires std::is_trivially_move_constructible_v<T> = default;
    /// @brief No copy constructor if not trivially copyable
    constexpr uninit(uninit&&) noexcept = delete;
    /// @brief Copy assignment operator
    constexpr uninit& operator=(const uninit&) noexcept requires std::is_trivially_copy_assignable_v<T> = default;
    /// @brief No default copy assignment operator
    constexpr uninit& operator=(const uninit&) noexcept = delete;
    /// @brief Copy assignment operator
    constexpr uninit& operator=(uninit&&) noexcept requires std::is_trivially_move_assignable_v<T> = default;
    /// @brief No default copy assignment operator
    constexpr uninit& operator=(uninit&&) noexcept = delete;
    /// @brief Constructor, does nothing
    constexpr uninit() noexcept requires (clt::is_release()) = default;
    /// @brief Constructor, does nothing (and sets a debug flag)
    constexpr uninit() noexcept requires (clt::is_debug())
      : OnDebugIsConstructed(false) {}

    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr meta::copy_trivial_t<const T&> data() const& noexcept
    {
      if constexpr (is_debug())
        assert_true("Uninitialized object not constructed!", OnDebugIsConstructed::value);
      return *details::ptr_to<T*>(buffer);
    }
    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr T& data() & noexcept
    {
      if constexpr (is_debug())
        assert_true("Uninitialized object not constructed!", OnDebugIsConstructed::value);
      return *details::ptr_to<T*>(buffer);
    }

    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr T&& data() && noexcept
    {
      if constexpr (is_debug())
        assert_true("Uninitialized object not constructed!", OnDebugIsConstructed::value);
      return std::move(*details::ptr_to<T*>(buffer));
    }

    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr meta::copy_trivial_t<const T&&> data() const&& noexcept
    {
      if constexpr (is_debug())
        assert_true("Uninitialized object not constructed!", OnDebugIsConstructed::value);
      return *details::ptr_to<T*>(buffer);
    }

    template<typename... Args> requires std::is_constructible_v<T, Args...>
    constexpr void construct(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
      if constexpr (is_debug())
      {
        assert_true("Uninitialized object already constructed!", !OnDebugIsConstructed::value);
        OnDebugIsConstructed::value = true;
      }
      new(buffer) T(std::forward<Args>(args)...);
    }
    
    /// @brief Destructs the underlying object
    constexpr void destruct() noexcept(std::is_nothrow_destructible_v<T>)
    {
      if constexpr (is_debug())
      {
        assert_true("Uninitialized object not constructed!", OnDebugIsConstructed::value);
        OnDebugIsConstructed::value = false;
      }
      (details::ptr_to<T*>(buffer))->~T();
    }
    
    /// @brief Does not destroy internal object
    constexpr ~uninit() noexcept
    {
      if constexpr (is_debug())
        assert_true("Initialized object not destructed!", !OnDebugIsConstructed::value);
    }

    /// @brief DEBUG ONLY: check if the internal object is constructed
    /// @return True if constructed
    constexpr bool _DEBUG_is_constructed() requires (clt::is_debug()) { return OnDebugIsConstructed::value; }
  };

  template<typename T>
  /// @brief 'in' parameters are const references
  using in = const T&;

  template<typename T>
  /// @brief 'ref' are references
  using ref = T&;

  template<typename T>
  /// @brief Represents uninitialized parameters that should end up initialized
  /// before the end of the function's body.
  class out
  {
    /// @brief The internal uninitialized object to use
    uninit<T>& internal_object;

  public:
    /// @brief Constructs an 'out' parameter from an uninitialized object
    /// @param object The object to initialize
    constexpr out(uninit<T>& object) noexcept
      : internal_object(object) {}

    /// @brief Returns the underlying storage in which to construct the object
    /// @return Reference to the underlying storage
    constexpr uninit<T>& underlying() noexcept { return internal_object; }
    /// @brief Returns the underlying storage in which to construct the object
    /// @return Reference to the underlying storage
    constexpr const uninit<T>& underlying() const noexcept { return internal_object; }

    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr meta::copy_trivial_t<const T&> data() const& noexcept
    {
      return internal_object.data();
    }
    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr T& data() & noexcept
    {
      return internal_object.data();
    }

    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr T&& data() && noexcept
    {
      return internal_object.data();
    }

    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr meta::copy_trivial_t<const T&&> data() const&& noexcept
    {
      return internal_object.data();
    }

    template<typename... Args> requires std::is_constructible_v<T, Args...>
    constexpr void construct(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
      internal_object.construct(std::forward<Args>(args)...);
    }

    /// @brief Verifies that the object was initialized before the end of the function
    constexpr ~out()
    {
      if constexpr (is_debug())
        assert_true("Function returned without initializing 'uninit' value!",
          internal_object._DEBUG_is_constructed());
    }
  };

  template<typename T>
  /// @brief Represents uninitialized parameters that might end up being initialized
  /// before the end of the function's body. This is considered more unsafe
  /// than 'out' as this API requires the caller of the function to verify
  /// the return of the function to check if the parameter was initialized.
  class maybe_out
  {
    /// @brief The internal uninitialized object to use
    uninit<T>& internal_object;

  public:
    /// @brief Constructs an 'out' parameter from an uninitialized object
    /// @param object The object to initialize
    constexpr maybe_out(uninit<T>& object) noexcept
      : internal_object(object) {}

    /// @brief Returns the underlying storage in which to construct the object
    /// @return Reference to the underlying storage
    constexpr uninit<T>& underlying() noexcept { return internal_object; }
    /// @brief Returns the underlying storage in which to construct the object
    /// @return Reference to the underlying storage
    constexpr const uninit<T>& underlying() const noexcept { return internal_object; }

    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr meta::copy_trivial_t<const T&> data() const& noexcept
    {
      return internal_object.data();
    }
    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr T& data() & noexcept
    {
      return internal_object.data();
    }

    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr T&& data() && noexcept
    {
      return internal_object.data();
    }

    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr meta::copy_trivial_t<const T&&> data() const&& noexcept
    {
      return internal_object.data();
    }

    template<typename... Args> requires std::is_constructible_v<T, Args...>
    constexpr void construct(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
      internal_object.construct(std::forward<Args>(args)...);
    }
  };
}

#endif //!HG_COLT_PARAMS