#ifndef HG_OS_DYNAMIC_LIB
#define HG_OS_DYNAMIC_LIB

#include <filesystem>
#include "colt/macro/config.h"
#include "colt/macro/assert.h"
#include "colt/dsa/option.h"
#include "colt/dsa/string_view.h"

namespace clt::os
{
  class DynamicLib
  {
    /// @brief The handle to the library
    void* _handle = nullptr;

    /// @brief Constructs a library with
    /// @param handle The pointer to the handle
    DynamicLib(void* handle) noexcept
      : _handle(handle)
    {
    }
  public:
    /// @brief Default constructor. Returns a library that is not open
    DynamicLib()                             = default;
    DynamicLib(const DynamicLib&)            = delete;
    DynamicLib& operator=(const DynamicLib&) = delete;
    
    DynamicLib(DynamicLib&& other) noexcept
        : _handle(std::exchange(other._handle, nullptr))
    {
    }
    DynamicLib& operator=(DynamicLib&& other) noexcept
    {
      assert_true("Self assignment is prohibited!", &other != this);
      std::swap(_handle, other._handle);
      return *this;
    }
    /// @brief Destructor, calls close if not already closed.
    ~DynamicLib()
    {
      if (_handle)
        close();
    }

    /// @brief True if the current library is not open.
    /// A default constructed library is closed.
    /// @return True if not open
    bool is_closed() const noexcept { return _handle == nullptr; }
    /// @brief True if the current library is open/
    /// @return True if not closed
    bool is_open() const noexcept { return _handle != nullptr; }

    /// @brief Closes the library.
    /// This is done automatically by the destructor.
    void close();
    /// @brief Searches for a symbol in the currently loaded library
    /// @param name The symbol's name
    /// @return null if not found or pointer to that symbol
    void* find_symbol(const char* name);
    /// @brief Returns true if the current library contains a symbol 'name'
    /// @param name The symbol name
    /// @return True if 'name' exists in the current library.
    bool has_symbol(const char* name) { return find_symbol(name) != nullptr; }
    /// @brief Searches for a symbol in the currently loaded library.
    /// If `is_closed()`, always returns None.
    /// @tparam Ty The type of the symbol
    /// @param name The name of the symbol
    /// @return 
    template<typename Ty> requires std::is_pointer_v<Ty>
    Option<Ty> find(ZStringView name)
    {
      auto sym = find_symbol(name.c_str());
      return sym ? reinterpret_cast<Ty>(sym) : None;
    }

    /// @brief Searches for a symbol in the currently loaded library.
    /// If `is_closed()`, always returns None.
    /// @tparam Ty The type of the symbol
    /// @param name The name of the symbol
    /// @return
    template<typename Ty>
      requires std::is_pointer_v<Ty>
    Option<Ty> find(const char* name)
    {
      auto sym = find_symbol(name.c_str());
      return sym ? reinterpret_cast<Ty>(sym) : None;
    }

    /// @brief Opens the current process as a dynamic library
    /// @return None on errors
    static Option<DynamicLib> open() noexcept;
    /// @brief Opens a dynamic library with path 'path'
    /// @param path The path to the dynamic library
    /// @return None on errors
    static Option<DynamicLib> open(const char* path) noexcept;
    /// @brief Opens a dynamic library with path 'path'
    /// @param path The path to the dynamic library
    /// @return None on errors
    static Option<DynamicLib> open(ZStringView path) noexcept
    {
      return open(path.c_str());
    }
  };
} // namespace clt::os

#endif // !HG_OS_DYNAMIC_LIB
