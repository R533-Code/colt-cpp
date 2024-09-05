#ifndef HG_OS_MEMORY_MAPPED_FILE
#define HG_OS_MEMORY_MAPPED_FILE

#include "colt/macro/config.h"
#include "colt/typedefs.h"
#include "colt/dsa/option.h"
#include "colt/dsa/string_view.h"

namespace clt::os
{
  class ViewOfFile
  {
#ifdef COLT_WINDOWS
    void* file_handle;
    void* mapping_handle;
    void* view_map;

    ViewOfFile(void* file, void* mapping, void* view) noexcept
        : file_handle(file)
        , mapping_handle(mapping)
        , view_map(view)
    {
    }
  public:
    ViewOfFile(ViewOfFile&& other) noexcept
        : file_handle(std::exchange(other.file_handle, nullptr))
        , mapping_handle(std::exchange(other.mapping_handle, nullptr))
        , view_map(std::exchange(other.view_map, nullptr))
    {
    }
    
    ViewOfFile& operator=(ViewOfFile&& other) noexcept
    {
      assert_true("Self assignment is prohibited!", &other != this);

      std::swap(file_handle, other.file_handle);
      std::swap(mapping_handle, other.mapping_handle);
      std::swap(view_map, other.view_map);
      
      return *this;
    }
#else
    void* mmap_handle;
    size_t file_size;

    ViewOfFile(void* file, size_t size) noexcept
        : mmap_handle(file)
        , file_size(size)
    {
    }

  public:
    ViewOfFile(ViewOfFile&& other) noexcept
        : mmap_handle(std::exchange(other.mmap_handle, nullptr))
        , file_size(std::exchange(other.file_size, 0))
    {
    }

    ViewOfFile& operator=(ViewOfFile&& other) noexcept
    {
      assert_true("Self assignment is prohibited!", &other != this);
      std::swap(mmap_handle, other.mmap_handle);
      std::swap(file_size, other.file_size);
      return *this;
    }
#endif // COLT_WINDOWS
    
    /// @brief Default constructor. Returns a library that is not open
    ViewOfFile()                             = default;
    ViewOfFile(const ViewOfFile&)            = delete;
    ViewOfFile& operator=(const ViewOfFile&) = delete;

    ~ViewOfFile()
    {
      if (is_open())
        close();
    }

    /// @brief True if the current library is not open.
    /// A default constructed library is closed.
    /// @return True if not open
    bool is_closed() const noexcept;
    /// @brief True if the current library is open/
    /// @return True if not closed
    bool is_open() const noexcept { return !is_closed(); }
    /// @brief Closes the view.
    /// This is done automatically by the destructor.
    void close();

    Option<View<u8>> view() const noexcept;

    static Option<ViewOfFile> open(const char* ptr);
    static Option<ViewOfFile> open(ZStringView ptr) { return open(ptr.c_str()); }
  };
} // namespace clt::os

#endif // !HG_OS_MEMORY_MAPPED_FILE
