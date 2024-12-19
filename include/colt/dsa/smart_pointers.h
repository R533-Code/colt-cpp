#ifndef HG_COLT_SMART_POINTERS
#define HG_COLT_SMART_POINTERS

#include <colt/mem/allocator_ref.h>

namespace clt
{
  template<typename T, meta::Allocator ALLOCATOR>
  /// @brief Unique pointer that automatically frees an allocated resource.
  /// A unique pointer is movable but not copyable, which makes it own its resource.
  class UniquePtr : private ALLOCATOR
  {
    /// @brief The memory block owned
    mem::MemBlock blk = {};

  public:
    template<typename, meta::Allocator>
    friend class UniquePtr;

    UniquePtr()                            = delete;
    UniquePtr(const UniquePtr&)            = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    /// @brief Constructs an empty UniquePtr
    /// @param alloc The allocator
    constexpr UniquePtr(const ALLOCATOR& alloc) noexcept
        : ALLOCATOR(alloc)
    {
    }

    /// @brief Constructs a UniquePtr from a MemBlock.
    /// @param alloc The allocator
    /// @param blk The block to use
    constexpr UniquePtr(const ALLOCATOR& alloc, mem::MemBlock blk) noexcept
        : ALLOCATOR(alloc)
        , blk(blk)
    {
    }

    /// @brief Move constructor, steals the resources of 'u'
    /// @param u The unique pointer whose resources to steal
    constexpr UniquePtr(UniquePtr<T, ALLOCATOR>&& u) noexcept
        : ALLOCATOR(u)
        , blk(std::exchange(u.blk, mem::nullblk))
    {
    }

    template<typename U>
      requires std::convertible_to<U*, T*>
    /// @brief Converting move constructor, steals the resources of 'u'
    /// @tparam U Type that is convertible to T
    /// @param u The unique pointer whose resources to steal
    constexpr UniquePtr(UniquePtr<U, ALLOCATOR>&& u) noexcept
        : ALLOCATOR(u)
        , blk(std::exchange(u.blk, mem::nullblk))
    {
    }

    /// /// @brief Destructor, frees resource
    constexpr ~UniquePtr() noexcept(std::is_nothrow_destructible_v<T>)
    {
      if (blk.is_null())
        return;
      ON_SCOPE_EXIT
      {
        ALLOCATOR::dealloc(blk);
      };
      //run destructor
      static_cast<T*>(blk.ptr())->~T();
    }

    /// @brief Move assignment operator
    /// @param r The pointer whose content to swap with
    /// @return Self
    constexpr UniquePtr& operator=(UniquePtr&& r) noexcept
    {
      std::swap(static_cast<ALLOCATOR&>(r), static_cast<ALLOCATOR&>(*this));
      std::swap(blk, r.blk);
      return *this;
    }

    template<typename U>
      requires std::convertible_to<U*, T*>
    /// @brief Move assignment operator
    /// @param r The pointer whose content to swap with
    /// @return Self
    constexpr UniquePtr& operator=(UniquePtr<U, ALLOCATOR>&& u) noexcept
    {
      std::swap(static_cast<ALLOCATOR&>(u), static_cast<ALLOCATOR&>(*this));
      std::swap(blk, u.blk);
      return *this;
    }

    /// @brief Releases the owned block
    /// @return The owned block
    constexpr mem::MemBlock release() noexcept
    {
      return std::exchange(blk, mem::nullblk);
    }

    /// @brief Frees resources and take ownership of 'ptr'
    /// @param ptr The block whose ownership to take
    constexpr void reset(mem::MemBlock ptr = mem::nullblk) noexcept(
        std::is_nothrow_destructible_v<T>)
    {
      auto cpy = blk;
      blk      = ptr;
      if (cpy.is_null())
        return;
      ON_SCOPE_EXIT
      {
        ALLOCATOR::dealloc(cpy);
      };
      //run destructor
      static_cast<T*>(cpy.ptr())->~T();
    }

    /// @brief Returns a reference to the owned MemBlock
    /// @return Reference to the owned MemBlock
    constexpr const mem::MemBlock& get() const noexcept { return blk; }

    /// @brief Check if the owned MemBlock is null
    /// @return True if null
    constexpr bool is_null() const noexcept { return get().is_null(); }

    /// @brief Check if the owned MemBlock is not null
    explicit constexpr operator bool() const noexcept { return !is_null(); }

    /// @brief Dereference operator
    /// @return Dereferences pointer
    constexpr std::add_lvalue_reference_t<T> operator*() const
        noexcept(noexcept(*std::declval<T*>()))
    {
      assert_true("unique_ptr was null!", !is_null());
      return *static_cast<T*>(blk.ptr());
    }

    /// @brief Dereference operator
    /// @return Dereferences pointer
    constexpr T* operator->() const noexcept
    {
      assert_true("unique_ptr was null!", !is_null());
      return *static_cast<T*>(blk.ptr());
    }
  };

  template<typename T, typename... Args>
  /// @brief Constructs a UniquePtr using a global allocator
  /// @tparam T The type to construct
  /// @param args... The arguments to forward to the constructor
  /// @return UniquePtr
  constexpr auto make_unique(Args&&... args) noexcept(
      std::is_nothrow_constructible_v<T, Args...>)
  {
    using namespace mem;

    auto blk = GlobalAllocator.alloc(sizeof(T));
    if constexpr (std::is_nothrow_constructible_v<T, Args...>)
    {
      new (blk.ptr()) T(std::forward<Args>(args)...);
    }
    else
    {
      try
      {
        new (blk.ptr()) T(std::forward<Args>(args)...);
      }
      catch (...)
      {
        GlobalAllocator.dealloc(blk);
        throw;
      }
    }
    return UniquePtr<T, decltype(GlobalAllocator)>(GlobalAllocator, blk);
  }

  template<typename T, meta::Allocator Alloc, typename... Args>
  /// @brief Constructs a UniquePtr using a local allocator
  /// @tparam T The type to construct
  /// @param ref The local allocator
  /// @param args... The arguments to forward to the constructor
  /// @return UniquePtr
  constexpr auto make_local_unique(Alloc& ref, Args&&... args) noexcept(
      std::is_nothrow_constructible_v<T, Args...>)
  {
    auto blk = ref.alloc(sizeof(T));
    if constexpr (std::is_nothrow_constructible_v<T, Args...>)
    {
      new (blk.ptr()) T(std::forward<Args>(args)...);
    }
    else
    {
      try
      {
        new (blk.ptr()) T(std::forward<Args>(args)...);
      }
      catch (...)
      {
        ref.dealloc(blk);
        throw;
      }
    }
    return UniquePtr<T, mem::LocalAllocatorRef<std::remove_cvref_t<decltype(ref)>>>(ref, blk);
  }
} // namespace clt

#endif // !HG_COLT_SMART_POINTERS
