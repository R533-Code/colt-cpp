#ifndef HG_COLT_UNIQUE_PTR
#define HG_COLT_UNIQUE_PTR

#include "../mem/global_alloc.h"
#include "../util/on_exit.h"

namespace clt
{
  template<typename T, auto ALLOCATOR = mem::GlobalAllocatorDescription>
    requires meta::AllocatorScope<ALLOCATOR>
  /// @brief Unique pointer that automatically frees an allocated resource.
  /// A unique pointer is movable but not copyable, which makes it own its resource.
  class UniquePtr
  {
    /// @brief Type of the allocator
    using Allocator = mem::allocator_ref<ALLOCATOR>;

    /// @brief The allocator used for allocation/deallocation
    [[no_unique_address]]
    Allocator allocator;
    mem::MemBlock blk = {};

  public:
    template<typename, auto ALL> requires meta::AllocatorScope<ALL>
    friend class UniquePtr;

    //Non-copyable
    UniquePtr(const UniquePtr&) = delete;
    //Non-copy-assignable
    UniquePtr& operator=(const UniquePtr&) = delete;
    
    /// @brief Constructs an empty UniquePtr
    constexpr UniquePtr() noexcept = default;
    
    /// @brief Constructs a UniquePtr from a MemBlock.
    /// On debug, sets the block to nullblk.
    /// @param blk The block to use
    constexpr UniquePtr(meta::for_debug_for_release_t<mem::MemBlock&, mem::MemBlock> blk) noexcept
      : blk(blk)
    {
      if constexpr (is_debug())
        blk = mem::nullblk;
    }
    
    /// @brief Move constructor, steals the resources of 'u'
    /// @param u The unique pointer whose resources to steal
    constexpr UniquePtr(UniquePtr<T, ALLOCATOR>&& u) noexcept
      : blk(std::exchange(u.blk, mem::nullblk)) {}
    
    template<typename U> requires std::convertible_to<U*, T*>
    /// @brief Converting move constructor, steals the resources of 'u'
    /// @tparam U Type that is convertible to T
    /// @param u The unique pointer whose resources to steal
    constexpr UniquePtr(UniquePtr<U, ALLOCATOR>&& u) noexcept
      : blk(std::exchange(u.blk, mem::nullblk)) {}
    /// @brief Destructor, frees resource
    constexpr ~UniquePtr() noexcept(std::is_nothrow_destructible_v<T>)
    {
      if (!blk.is_null())
      {
        ON_SCOPE_EXIT{
          allocator.dealloc(blk);
        };
        //run destructor
        static_cast<T*>(blk.ptr())->~T();
      }
    }

    /// @brief Move assignment operator
    /// @param r The pointer whose content to swap with
    /// @return Self
    UniquePtr& operator=(UniquePtr&& r) noexcept
    {
      auto cpy = blk;
      blk = r.blk;
      r.blk = cpy;
      return *this;
    }

    template<typename U> requires std::convertible_to<U*, T*>
    /// @brief Move assignment operator
    /// @param r The pointer whose content to swap with
    /// @return Self
    UniquePtr& operator=(UniquePtr<U, ALLOCATOR>&& u) noexcept
    {
      auto cpy = blk;
      blk = u.blk;
      u.blk = cpy;
      return *this;
    }

    /// @brief Releases the owned block
    /// @return The owned block
    mem::MemBlock release() noexcept
    {
      return std::exchange(blk, mem::nullblk);
    }

    /// @brief Frees resources and take ownership of 'ptr'
    /// @param ptr The block whose ownership to take
    void reset(mem::MemBlock ptr = mem::nullblk) noexcept(std::is_nothrow_destructible_v<T>)
    {
      auto cpy = blk;
      blk = ptr;
      if (!cpy.is_null())
      {
        ON_SCOPE_EXIT{
          allocator.dealloc(cpy);
        };
        //run destructor
        static_cast<T*>(cpy.ptr())->~T();
      }
    }

    /// @brief Returns a reference to the owned MemBlock
    /// @return Reference to the owned MemBlock
    const mem::MemBlock& get() const noexcept
    {
      return blk;
    }

    /// @brief Check if the owned MemBlock is null
    /// @return True if null
    constexpr bool is_null() const noexcept { return get().is_null(); }

    /// @brief Check if the owned MemBlock is not null
    explicit constexpr operator bool() const noexcept { return !is_null(); }

    /// @brief Dereference operator
    /// @return Dereferences pointer
    std::add_lvalue_reference_t<T> operator*() const noexcept(noexcept(*std::declval<T*>()))
      COLT_PRE(!is_null())
      return *static_cast<T*>(blk.ptr());
    COLT_POST()

    /// @brief Dereference operator
    /// @return Dereferences pointer
    T* operator->() const noexcept
      COLT_PRE(!is_null())
      return *static_cast<T*>(blk.ptr());
    COLT_POST()
  };

  template<typename T, auto ALLOCATOR = mem::GlobalAllocatorDescription, typename... Args>
  UniquePtr<T, ALLOCATOR> make_unique(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
  {
    using Allocator = mem::allocator_ref<ALLOCATOR>;

    Allocator allocator;
    auto blk = allocator.alloc(sizeof(T));
    if constexpr (std::is_nothrow_constructible_v<T, Args...>)
    {
      new(blk.ptr()) T(std::forward<Args>(args)...);
    }
    else
    {
      try
      {
        new(blk.ptr()) T(std::forward<Args>(args)...);
      }
      catch (...)
      {
        allocator.dealloc(blk);
        throw;
      }
    }
    return UniquePtr<T, ALLOCATOR>(blk);
  }

  template<typename T, auto ALLOCATOR>
  /// @brief clt::hash overload for UniquePtr
  struct hash<UniquePtr<T, ALLOCATOR>>
  {
    /// @brief Hashing operator
    /// @param value The value to hash
    /// @return Hash
    constexpr size_t operator()(const UniquePtr<T, ALLOCATOR>& value) const noexcept
    {
      return GetHash(value.get());
    }
  };
}

#endif //!HG_COLT_UNIQUE_PTR