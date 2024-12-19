#ifndef HG_COLT_MMAP
#define HG_COLT_MMAP

#include <colt/typedefs.h>
#include <colt/dsa/option.h>
#include <colt/dsa/string_view.h>

namespace clt
{
  /// @brief Represents a memory page
  class VirtualPage
  {
    /// @brief The pointer to the start of the block (or null)
    void* begin_ = nullptr;
    /// @brief The size of the block or 0 if null
    size_t size_ = 0;

    /// @brief Constructor
    /// @param begin The start of the block
    /// @param size The block size
    VirtualPage(void* begin, size_t size) noexcept
        : begin_(begin)
        , size_(size)
    {
      assert_true("Invalid block!", implies(begin == nullptr, size == 0));
    }

  public:
    constexpr VirtualPage() noexcept                              = default;
    constexpr VirtualPage(VirtualPage&&) noexcept                 = default;
    constexpr VirtualPage(const VirtualPage&) noexcept            = default;
    constexpr VirtualPage& operator=(VirtualPage&&) noexcept      = default;
    constexpr VirtualPage& operator=(const VirtualPage&) noexcept = default;

    enum class PageAccess : u8
    {
      /// @brief No access: any access result in a fault
      None,
      /// @brief Executable: the page is executable and readable
      ReadExecute,
      /// @brief Executable: the page is executable and writable
      WriteExecute,
      /// @brief Writable: the page is writable and readable
      ReadWrite,
      /// @brief Readable: the page is only readable
      ReadOnly,
    };
    using enum PageAccess;

    /// @brief Returns a pointer to the beginning of the page
    /// @return Pointer to the beginning of the page
    const void* ptr() const noexcept { return begin_; }
    /// @brief Returns a pointer to the beginning of the page
    /// @return Pointer to the beginning of the page
    void* ptr() noexcept { return begin_; }
    /// @brief Returns the size of the allocation of the current page
    /// @return The size of the current page
    size_t size() const noexcept { return size_; }
    /// @brief Check if the current page is null
    /// @return True if the current page is null
    bool is_null() const noexcept { return begin_ == nullptr; }

    /// @brief Allocates a new page
    /// @param byte The size in bytes of the page (not required to be a multiple of `page_size()`
    /// @param access The access type for the allocated page
    /// @param hint Hint address to where to allocate the page
    /// @return Allocated Page or Page for which is_null is true on errors
    COLTCPP_EXPORT
    static VirtualPage allocate(
        bytes byte, PageAccess access, void* hint = nullptr) noexcept;
    /// @brief Deallocates a page that was created through `allocate`
    /// @param page The page to deallocate
    COLTCPP_EXPORT
    static void deallocate(const VirtualPage& page) noexcept;

    /// @brief Returns the default page size of the current OS.
    /// The result of the underlying system call is cached.
    /// @return The default page size of the current OS
    COLTCPP_EXPORT
    static bytes page_size() noexcept;

    /// @brief Flush the instruction cache.
    /// This function should be called if an application generate or
    /// modify code in memory.
    /// @param start The start of the instruction memory to flush (can be null)
    /// @param offset The size of the memory to flush (0 if start is null)
    /// @pre implies(start != nullptr, offset != 0)
    COLTCPP_EXPORT static void flush_icache(
        const void* start = nullptr, size_t offset = 0) noexcept;

    /// @brief Flushes the instruction cache of the current page
    void flush_icache() noexcept { VirtualPage::flush_icache(ptr(), size()); }
  };

  /// @brief Represents a view over a read-only memory mapped file.
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

    /// @brief Destructor, automatically closes associated resources
    ~ViewOfFile()
    {
      if (is_open())
        close();
    }
    /// @brief True if the current library is open/
    /// @return True if not closed
    bool is_open() const noexcept { return !is_closed(); }

    /// @brief True if the current library is not open.
    /// A default constructed library is closed.
    /// @return True if not open
    COLTCPP_EXPORT bool is_closed() const noexcept;
    /// @brief Closes the view.
    /// This is done automatically by the destructor.
    COLTCPP_EXPORT void close();

    /// @brief Returns a view of bytes over the file.
    /// If the file is not opened, returns an empty view.
    /// None is only returned on OS failures.
    /// @return None on OS failures, else view over the file.
    COLTCPP_EXPORT Option<View<u8>> view() const noexcept;

    /// @brief Opens a view of a file
    /// @param ptr The file path
    /// @return None on errors or opened ViewOfFile
    COLTCPP_EXPORT static Option<ViewOfFile> open(const char* ptr);
    /// @brief Opens a view of a file
    /// @param ptr The file path
    /// @return None on errors or opened ViewOfFile
    static Option<ViewOfFile> open(ZStringView ptr) { return open(ptr.c_str()); }
  };
} // namespace clt

#endif // !HG_COLT_MMAP
