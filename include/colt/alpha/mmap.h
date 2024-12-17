#ifndef HG_COLT_MMAP
#define HG_COLT_MMAP

#include <span>
#include <colt/alpha/units.h>

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
    constexpr VirtualPage() noexcept = default;
    constexpr VirtualPage(VirtualPage&&) noexcept = default;
    constexpr VirtualPage(const VirtualPage&) noexcept = default;
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
    static VirtualPage allocate(bytes byte, PageAccess access, void* hint = nullptr) noexcept;
    /// @brief Deallocates a page that was created through `allocate`
    /// @param page The page to deallocate
    static void deallocate(const VirtualPage& page) noexcept;

    /// @brief Returns the default page size of the current OS.
    /// The result of the underlying system call is cached.
    /// @return The default page size of the current OS
    static bytes page_size() noexcept;
  };
}

#endif // !HG_COLT_MMAP
