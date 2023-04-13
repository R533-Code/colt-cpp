#ifndef HG_COLT_PARAMS
#define HG_COLT_PARAMS

#include <type_traits>

#include "../structs/helper.h"

namespace clt
{
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
    constexpr uninit() noexcept = default;
    /// @brief Destructor does nothing
    constexpr ~uninit() noexcept = default;

    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr const T& data() const noexcept
    {
      if constexpr (is_debug())
        assert_true("Uninitialized object not constructed!", OnDebugIsConstructed::value);
      return *details::ptr_to<T*>(buffer);
    }
    /// @brief Returns a reference to the underlying object, but do construct it first!
    /// @return Reference to the data
    constexpr const T& data() const noexcept
    {
      if constexpr (is_debug())
        assert_true("Uninitialized object not constructed!", OnDebugIsConstructed::value);
      return *details::ptr_to<T*>(buffer);
    }

    template<typename... Args> requires std::is_constructible_v<T, Args>
    constexpr void construct(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args>)
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
      (&this->data()).~T();
    }
    
    /// @brief Does not destroy internal object
    constexpr ~uninit() noexcept
    {
      if constexpr (is_debug())
        assert_true("Uninitialized object not destructed!", OnDebugIsConstructed::value);
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
  /// @brief Represents uninitialized parameters that should end up be initialized
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
    constexpr uninit& object() noexcept { return internal_object; }
    /// @brief Returns the underlying storage in which to construct the object
    /// @return Reference to the underlying storage
    constexpr const uninit& object() const noexcept { return internal_object; }

    /// @brief Verifies that the object was initialized before the end of the function
    constexpr ~out()
    {
      if constexpr (is_debug())
        assert_true("Function returned without initializing 'uninit' value!",
          internal_object._DEBUG_is_constructed());
    }
  };
}

#endif //!HG_COLT_PARAMS