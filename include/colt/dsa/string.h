#ifndef HG_DSA_STRING
#define HG_DSA_STRING

#include "string_view.h"
#include "colt/mem/composable_alloc.h"

namespace clt
{
  namespace meta
  {
    /// @brief Used to customize BasicString
    struct StringCustomization
    {
      /// @brief The encoding of the current string
      StringEncoding ENCODING;
      /// @brief Buffer size
      /// @warning Use buffer_bytesize to access
      size_t BUFFER_SIZE;
      /// @brief If the buffer is big enough, cache the count of code point.
      /// This might affect performance if the count is not being used, as
      /// the string must count each characters that is appended or removed.
      bool CACHE_COUNT;
      /// @brief If the buffer is big enough, cache the middle code point.
      /// This can be improve indexing speed by counting from the middle
      /// of the string.
      bool CACHE_MIDDLE;
      /// @brief If the current platform supports it, the string will not make copy
      /// of literals inside of the 'rdata' section unless a modification occurs.
      bool NO_COPY_IF_CONST;

      /// @brief Check if the string has a Small String Optimization buffer.
      /// The size of that buffer is calculated by 'buffer_bytesize'.
      /// @return True if the string has a buffer
      constexpr bool has_buffer() const noexcept { return BUFFER_SIZE != 0; }
      /// @brief True if the string must cache the count of code point stored.
      /// This can never be true if the buffer does not provide enough memory
      /// to store the count.
      /// @return True if CACHE_COUNT
      constexpr bool has_count() const noexcept
      {
        return is_variadic_encoding(ENCODING)
               && buffer_bytesize() >= sizeof(void*) + sizeof(size_t) && CACHE_COUNT;
      }
      /// @brief True if the string must cache the offset to the middle code point.
      /// This can never be true if the count is not cached as computing the middle
      /// already computes the count.
      /// This can never be true if the buffer does not provide enough memory
      /// to store the count.
      /// @return True if CACHE_MIDDLE
      constexpr bool has_middle() const noexcept
      {
        return is_variadic_encoding(ENCODING)
               && buffer_bytesize() >= sizeof(void*) + 2 * sizeof(size_t)
               && CACHE_COUNT && CACHE_MIDDLE;
      }
      /// @brief True if the string will not make copy of literal unless a modification occurs.
      /// If the current platform does not support it, then returns false.
      /// @return True if the current platform supports it and NO_COPY_IF_CONST is true.
      constexpr bool has_cow() const noexcept
      {
#if defined(COLT_WINDOWS) || defined(COLT_LINUX)
        return NO_COPY_IF_CONST;
#else
        return false;
#endif // COLT_WINDOWS
      }

      /// @brief Returns the SSO buffer size.
      /// The buffer size is NOT equal to BUFFER_SIZE, but is rather aligned to
      /// the next multiple of 8.
      /// @return The SSO buffer size
      constexpr size_t buffer_bytesize() const noexcept
      {
        // Align to 8 byte boundary.
        return (BUFFER_SIZE + 7) & static_cast<size_t>(-8);
      }
    };
  } // namespace meta

  namespace details
  {
    struct SSOMiddle
    {
      size_t value{};
    };

    struct SSOCount
    {
      size_t value{};
    };

    template<bool CACHE_MIDDLE>
    using SSOCacheMiddle = std::conditional_t<CACHE_MIDDLE, SSOCount, meta::empty_t<SSOCount>>;
    template<bool CACHE_COUNT>
    using SSOCacheCount = std::conditional_t<CACHE_COUNT, SSOMiddle, meta::empty_t<SSOMiddle>>;

    template<meta::CharType Ty, bool CACHE_MIDDLE, bool CACHE_COUNT>
    class SSOLongInfo
        : private SSOCacheCount<CACHE_COUNT>
        , private SSOCacheMiddle<CACHE_MIDDLE>
    {
      Ty* _ptr;

    public:
      constexpr SSOLongInfo() noexcept = default;
      MAKE_DEFAULT_COPY_AND_MOVE_FOR(SSOLongInfo);

      constexpr auto& ptr() noexcept { return _ptr; }
      constexpr auto& ptr() const noexcept { return _ptr; }

      constexpr auto& count() noexcept
        requires CACHE_COUNT
      {
        return SSOCacheCount<CACHE_COUNT>::value;
      }
      constexpr auto& count() const noexcept
        requires CACHE_COUNT
      {
        return SSOCacheCount<CACHE_COUNT>::value;
      }
      constexpr auto& middle() noexcept
        requires CACHE_MIDDLE
      {
        return SSOCacheMiddle<CACHE_MIDDLE>::value;
      }
      constexpr auto& middle() const noexcept
        requires CACHE_COUNT
      {
        return SSOCacheMiddle<CACHE_MIDDLE>::value;
      }
    };

    template<meta::StringCustomization CUSTOMIZATION>
    class SSOBuffer
    {
    public:
      using char_t = meta::encoding_to_char_t<CUSTOMIZATION.ENCODING>;

    private:
      union
      {
        SSOLongInfo<
            meta::encoding_to_char_t<CUSTOMIZATION.ENCODING>,
            CUSTOMIZATION.CACHE_MIDDLE, CUSTOMIZATION.CACHE_COUNT>
            _long;

        char_t _buffer[math::max(CUSTOMIZATION.buffer_bytesize() / sizeof(char_t), size_t(sizeof(void*)))];
      };

    public:
      constexpr auto& ptr() noexcept { return _long.ptr(); }
      constexpr auto& ptr() const noexcept { return _long.ptr(); }

      constexpr auto& buffer() noexcept
        requires(CUSTOMIZATION.has_buffer())
      {
        return _buffer;
      }
      constexpr auto& buffer() const noexcept
        requires(CUSTOMIZATION.has_buffer())
      {
        return _buffer;
      }
      constexpr auto& count() noexcept
        requires(CUSTOMIZATION.has_count())
      {
        return _long.count();
      }
      constexpr auto& count() const noexcept
        requires(CUSTOMIZATION.has_count())
      {
        return _long.count();
      }
      constexpr auto& middle() noexcept
        requires(CUSTOMIZATION.has_middle())
      {
        return _long.middle();
      }
      constexpr auto& middle() const noexcept
        requires(CUSTOMIZATION.has_middle())
      {
        return _long.middle();
      }

      SSOBuffer() noexcept {}
      MAKE_DEFAULT_COPY_AND_MOVE_FOR(SSOBuffer);
    };
  } // namespace details

  template<meta::StringCustomization CUSTOMIZATION, typename ALLOCATOR>
  class BasicString : private ALLOCATOR
  {
  public:
    using char_t = meta::encoding_to_char_t<CUSTOMIZATION.ENCODING>;
    /// @brief The string encoding
    static constexpr StringEncoding STR_ENCODING = CUSTOMIZATION.ENCODING;
    /// @brief True if the string has a SSO buffer
    static constexpr bool HAS_BUFFER = CUSTOMIZATION.has_buffer();
    /// @brief True if the string caches the code point count
    static constexpr bool HAS_COUNT = CUSTOMIZATION.has_count();
    /// @brief True if the string caches the middle code point
    static constexpr bool HAS_MIDDLE = CUSTOMIZATION.has_middle();
    /// @brief True if the string supports Copy On Write for literals
    static constexpr bool HAS_COW = CUSTOMIZATION.has_cow();
    /// @brief The size of the SSO buffer
    static constexpr size_t SSO_SIZE =
        CUSTOMIZATION.buffer_bytesize() / sizeof(char_t);

  private:
    details::SSOBuffer<CUSTOMIZATION> _ptr_or_buffer;
    size_t _size;
    size_t _capacity;

    HEDLEY_ALWAYS_INLINE
    bool _is_long() const noexcept
      requires HAS_BUFFER
    {
      return _capacity != SSO_SIZE;
    }

    /// @brief The true size of the string (including NUL terminator)
    /// @return _size + 1 to account for terminator
    size_t _true_size() const noexcept { return _size + (size_t)(_size != 0); }

    /// @brief Allocates more memory for the string.
    /// This modifies the capacity (and data()).
    /// @param new_capacity The new capacity
    void alloc(size_t new_capacity) noexcept
    {
      if constexpr (HAS_BUFFER)
      {
        // No allocations needed
        if (new_capacity < SSO_SIZE)
        {
          _capacity = SSO_SIZE;
          return;
        }
      }
      auto blk = ALLOCATOR::alloc((new_capacity + _capacity) * sizeof(char_t));
      ::memcpy(data(), blk.ptr(), _true_size());
      dealloc();
      _ptr_or_buffer.ptr() = ptr_to<char_t*>(blk.ptr());
      _capacity            = new_capacity;
    }

    /// @brief Deallocates current allocated memory.
    /// This does not modify any of the members.
    void dealloc() noexcept
    {
      if constexpr (HAS_BUFFER)
      {
        if (_is_long())
          ALLOCATOR::dealloc({_ptr_or_buffer.ptr(), _capacity * sizeof(char_t)});
        // No need to deallocate as internal buffer is used instead
      }
      else
        ALLOCATOR::dealloc(
            {(void*)((uintptr_t)_ptr_or_buffer.ptr() * (size_t)(_capacity != 0)),
             _capacity * sizeof(char_t)});
      // The multiplication by (_capacity != 0) is used to eliminate
      // the case where the BasicString is initialized to "".
    }

  public:
    /// @brief Constructs an empty string
    BasicString(const ALLOCATOR& ALLOC) noexcept
      requires HAS_BUFFER
        : ALLOCATOR(ALLOC)
        , _size(0)
        , _capacity(SSO_SIZE)
    {
      _ptr_or_buffer.buffer()[0] = '\0';
      if constexpr (HAS_COUNT)
      {
        if (_is_long())
          _ptr_or_buffer.count() = 0;
      }
      if constexpr (HAS_MIDDLE)
      {
        if (_is_long())
          _ptr_or_buffer.middle() = 0;
      }
    }

    BasicString(const ALLOCATOR& ALLOC) noexcept
      requires(!HAS_BUFFER)
        : ALLOCATOR(ALLOC)
        , _size(0)
        , _capacity(0)
    {
      _ptr_or_buffer.ptr() = meta::empty_string_literal<char_t>();
    }

    template<bool IS_ZSTRING>
    BasicString(
        const ALLOCATOR& ALLOC, BasicStringView<STR_ENCODING, IS_ZSTRING> str,
        size_t added_capacity = 0) noexcept
        : ALLOCATOR(ALLOC)
        , _size(0)
        , _capacity(SSO_SIZE)
    {
      const size_t size = str.unit_len();
      // The size must still be 0 when call is done to not copy anything
      alloc(size + added_capacity + 1);
      _size = size;

      const auto cache_data = data();
      ::memcpy(cache_data, str.data(), sizeof(char_t) * _size);
      cache_data[_size] = '\0';
      if constexpr (HAS_MIDDLE)
      {
        if (_is_long())
        {
          auto [count, middle]    = uni::count_and_middle(cache_data, _size);
          _ptr_or_buffer.count()  = count;
          _ptr_or_buffer.middle() = middle;
        }
      }
      else if constexpr (HAS_COUNT)
      {
        if (_is_long())
          _ptr_or_buffer.count() = uni::countlen(cache_data, _size);
      }
    }

    ~BasicString() { dealloc(); }

    /// @brief Returns a pointer to the data owned by the string
    /// @return Pointer to the data owned by the string
    const char_t* data() const noexcept
    {
      if constexpr (HAS_BUFFER)
        return _is_long() ? _ptr_or_buffer.ptr() : _ptr_or_buffer.buffer();
      else
        return _ptr_or_buffer.ptr();
    }

    /// @brief Returns a pointer to the data owned by the string
    /// @return Pointer to the data owned by the string
    char_t* data() noexcept
    {
      // Thanks Scott!
      return const_cast<char_t*>(static_cast<const BasicString&>(*this).data());
    }

    /// @brief Returns the capacity of the string
    /// @return The capacity of the string
    size_t capacity() const noexcept { return _capacity; }

    /// @brief Returns the count of code points in the string
    /// This does not include the NUL terminator.
    /// @return strlen of the string
    size_t size() const noexcept
    {
      if constexpr (HAS_COUNT)
        return _is_long() ? _ptr_or_buffer.count()
                          : uni::countlen(_ptr_or_buffer.buffer(), _size);
      else
        return uni::countlen(data(), _size);
    }

    /// @brief Returns the unit count over which the view is spanning.
    /// This does not include the NUL terminator.
    /// @return The unit count
    size_t unit_len() const noexcept { return _size; }

    /// @brief Check if the current string is empty.
    /// Empty strings are still NUL terminated.
    /// @return True if size() == 0
    bool is_empty() const noexcept { return unit_len() == 0; }

    /// @brief Returns the char at index 'index'.
    /// Do not use this operator to iterate over the view:
    /// use a ranged for loop for performance.
    /// Same as 'index_front'.
    /// @pre index < size()
    /// @param index The index of the char to return
    /// @return The char at index 'index'
    constexpr char32_t operator[](size_t index) const noexcept
    {
      return index_front(index);
    }

    /// @brief Returns the char at index 'index'.
    /// @pre index < size()
    /// @param index The index of the char to return
    /// @return The char at index 'index'
    constexpr char32_t index_front(size_t index) const noexcept;
    /// @brief Returns the char at index 'index' starting from the end.
    /// @pre index < size()
    /// @param index The index of the char to return
    /// @return The char at index 'index' starting from the end
    constexpr char32_t index_back(size_t index) const noexcept;    

    /// @brief Get the front of the view.
    /// @return The first item of the view
    char32_t front() const noexcept
    {
      assert_true("Invalid size!", !is_empty());
      return index_front(0);
    }

    /// @brief Get the back of the view.
    /// @return The last item of the view
    char32_t back() const noexcept
    {
      assert_true("Invalid size!", !is_empty());
      return index_back(0);
    }

    /// @brief Returns a NUL-terminated string.
    /// This is always guaranteed to work without allocation.
    /// @return NUL-terminated string
    auto c_str() const noexcept { return data(); }
    /// @brief Returns a NUL-terminated string.
    /// This is always guaranteed to work without allocation.
    /// @return NUL-terminated string
    auto c_str() noexcept { return data(); }

    /// @brief Returns a StringView over the characters
    /// @return StringView over all the characters
    BasicStringView<STR_ENCODING> to_view() const noexcept
    {
      return {data(), _size};
    }
    /// @brief Returns a StringView over the characters
    operator BasicStringView<STR_ENCODING>() const noexcept
    {
      return {data(), _size};
    }

    /// @brief Returns a ZStringView over the characters
    /// @return StringView over all the characters
    BasicZStringView<STR_ENCODING> to_zview() const noexcept
    {
      return {data(), _size};
    }
    /// @brief Returns a ZStringView over the characters
    operator BasicZStringView<STR_ENCODING>() const noexcept
    {
      return {data(), _size};
    }

    /// @brief Lexicographically compare two views
    /// @param v1 The first view
    /// @param v2 The second view
    /// @return Result of comparison
    template<StringEncoding ENCODING2, bool ZSTRING2>
    friend constexpr auto operator<=>(
        const BasicString& v1,
        const BasicStringView<ENCODING2, ZSTRING2>& v2) noexcept
    {
      auto view = v1.to_view();
      return std::lexicographical_compare_three_way(
          view.begin(), view.end(), v2.begin(), v2.end());
    }
  };

  /// @brief Check if a pointer (maybe) points into the const segment
  /// of the executable.
  /// @param ptr The pointer to check for
  /// @return True if in const segment
  bool maybe_in_const_segment(const void* ptr) noexcept;

  /// @brief Represents a String with the default customization
  /// @tparam ALLOCATOR The allocator
  /// @tparam ENCODING The encoding of the string
  template<typename ALLOCATOR, StringEncoding ENCODING = StringEncoding::ASCII>
  using String = BasicString<
      meta::StringCustomization{ENCODING, 24, true, true, true}, ALLOCATOR>;
  /// @brief Represents a UTF8 string with the default customization
  /// @tparam ALLOCATOR The allocator
  template<typename ALLOCATOR>
  using u8String = String<ALLOCATOR, StringEncoding::UTF8>;
  /// @brief Represents a UTF16 with platform endianness string with the default customization
  /// @tparam ALLOCATOR The allocator
  template<typename ALLOCATOR>
  using u16String = String<ALLOCATOR, StringEncoding::UTF16>;
  /// @brief Represents a UTF32 with platform endianness string with the default customization
  /// @tparam ALLOCATOR The allocator
  template<typename ALLOCATOR>
  using u32String = String<ALLOCATOR, StringEncoding::UTF32>;
  
  
  template<meta::StringCustomization CUSTOMIZATION, typename ALLOCATOR>
  constexpr char32_t BasicString<CUSTOMIZATION, ALLOCATOR>::index_front(
      size_t index) const noexcept
  {
    assert_true("Invalid index!", index < size());
    // If the size is not cached, then computing the size
    // will take more time than just indexing.
    if constexpr (HAS_MIDDLE)
    {
      if (_is_long())
      {
        const size_t cached_size         = size();
        const size_t middle_cached_size  = cached_size / 2;
        const size_t quarter_cached_size = cached_size / 4;
        if (index >= middle_cached_size)
        {
          return uni::index_front(
              _ptr_or_buffer.ptr() + _ptr_or_buffer.middle(),
              index - middle_cached_size);
        }
        if (index > quarter_cached_size)
        {
          return uni::index_back(
              _ptr_or_buffer.ptr() + _ptr_or_buffer.middle(),
              middle_cached_size - index);
        }
        return uni::index_front(_ptr_or_buffer.ptr(), index);
      }
      return uni::index_front(_ptr_or_buffer.buffer(), index);
    }
    else if constexpr (HAS_COUNT)
    {
      if (_is_long())
      {
        if (index >= _ptr_or_buffer.count() / 2)
          return uni::index_back(_ptr_or_buffer.ptr(), _ptr_or_buffer.count() - index);
        return uni::index_front(_ptr_or_buffer.ptr(), index);
      }
      return uni::index_front(_ptr_or_buffer.buffer(), index);
    }
    else
      return uni::index_front(data(), index);
  }
  
  template<meta::StringCustomization CUSTOMIZATION, typename ALLOCATOR>
  constexpr char32_t BasicString<CUSTOMIZATION, ALLOCATOR>::index_back(
      size_t index) const noexcept
  {
    assert_true("Invalid index!", index < size());
    // If the size is not cached, then computing the size
    // will take more time than just indexing.
    if constexpr (HAS_MIDDLE)
    {
      if (_is_long())
      {
        const size_t cached_size         = size();
        const size_t middle_cached_size  = cached_size / 2;
        const size_t quarter_cached_size = 3 * (cached_size / 4);
        if (index >= middle_cached_size)
        {
          return uni::index_back(
              _ptr_or_buffer.ptr() + _ptr_or_buffer.middle(),
              middle_cached_size - index);
        }
        if (index > quarter_cached_size)
        {
          return uni::index_front(
              _ptr_or_buffer.ptr() + _ptr_or_buffer.middle(),
              index - middle_cached_size);
        }
        return uni::index_back(_ptr_or_buffer.ptr() + _size - 1, index);
      }
      return uni::index_back(_ptr_or_buffer.buffer() + _size - 1, index);
    }
    else if constexpr (HAS_COUNT)
    {
      if (_is_long())
      {
        if (index >= _ptr_or_buffer.count() / 2)
          return uni::index_front(
              _ptr_or_buffer.ptr(), _ptr_or_buffer.count() - index);
        return uni::index_back(data() + _size - 1, index);
      }
      return uni::index_back(_ptr_or_buffer.buffer(), index);
    }
    else
      return uni::index_back(data() + _size - 1, index);
  }
} // namespace clt

template<clt::meta::StringCustomization CUSTOMIZATION, typename ALLOCATOR>
struct fmt::formatter<clt::BasicString<CUSTOMIZATION, ALLOCATOR>>
{
  using to_format = clt::BasicString<CUSTOMIZATION, ALLOCATOR>;

  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const to_format& vec, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{}", vec.to_view());
  }
};

#endif // !HG_DSA_STRING
