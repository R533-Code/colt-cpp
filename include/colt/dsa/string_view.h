#ifndef HG_DSA_STRING_VIEW
#define HG_DSA_STRING_VIEW

#include "unicode.h"
#include "colt/dsa/iterator.h"

namespace clt
{
  /// @brief Non-owning view over contiguous characters
  /// If ZSTRING is true, no operations affecting the end of
  /// BasicStringView can happen to not lose the NUL-terminator.
  /// @tparam ENCODING The encoding of the characters
  /// @tparam ZSTRING If true, then the view is NUL-terminated
  template<StringEncoding ENCODING, bool ZSTRING>
  class BasicStringView;

  template<bool ZSTRING>
  class BasicStringView<StringEncoding::ASCII, ZSTRING>
  {
    /// @brief The pointer to the characters
    const char* _ptr{};
    /// @brief The size of the view
    size_t _size{};

  public:
    /// @brief The string encoding of the current BasicStringView
    static constexpr StringEncoding string_encoding = StringEncoding::ASCII;
    /// @brief True if null terminated view
    static constexpr bool is_zstring_view = ZSTRING;

    /// @brief Constructs an empty StringView
    constexpr BasicStringView()
      requires(!ZSTRING)
    = default;
    
    /// @brief Constructs an empty ZStringView
    constexpr BasicStringView()
      requires ZSTRING
        : _ptr("")
        , _size(0)
    {
      utf8::distance()
    }
    
    /// @brief Constructs a view starting at 'ptr', of size 'size'.
    /// For ZStringView, the character AFTER the last character MUST
    /// be a NUL-terminator: `ptr[size] == '\0'`.
    /// For StringView, ptr can be null only if 'size' is zero.
    /// @param ptr The start of the view
    /// @param size The size of the view (in bytes)
    constexpr BasicStringView(const char* ptr, size_t size) noexcept
        : _ptr(ptr)
        , _size(size)
    {
      if constexpr (!ZSTRING)
      {
        assert_true(
            "ptr cannot be null if size != 0!", implies(_ptr == nullptr, _size == 0));
      }
      else
      {
        assert_true("ptr cannot be null for ZStringView!", _ptr != nullptr);
        assert_true("ZStringView must be NUL-terminated!", _ptr[_size] == '\0');
      }
    }

    /// @brief Constructs a view starting at 'ptr', of size 'begin - end'.
    /// @pre begin <= end
    /// For ZStringView, the character AFTER the last character MUST
    /// be a NUL-terminator: `begin[size()] == '\0'`.
    /// For StringView, ptr can be null only if 'size' is zero.
    /// @param begin The start of the view
    /// @param end The size of the view (in bytes)
    constexpr BasicStringView(const char* begin, const char* end) noexcept
        : _ptr(begin)
        , _size(end - begin)
    {
      assert_true("begin must be <= end!", begin <= end);
      if constexpr (!ZSTRING)
      {
        assert_true("ptr cannot be null if size != 0!",
            implies(_ptr == nullptr, _size == 0));
      }
      else
      {
        assert_true("ptr cannot be null for ZStringView!", begin != nullptr, end != nullptr);
        assert_true("ZStringView must be NUL-terminated!", *end == '\0');
      }
    }

    /// @brief Constructs a view from a NUL-terminated string.
    /// @pre ptr != nullptr
    /// @param ptr The NUL-terminated string
    constexpr BasicStringView(const char* ptr) noexcept
        : _ptr(ptr)
        , _size(clt::strlen(ptr))
    {
      assert_true("ptr cannot be null!", _ptr != nullptr);
    }

    MAKE_DEFAULT_COPY_AND_MOVE_FOR(BasicStringView);

    /// @brief Iterator type
    using iterator = transform_iter<const char*, [](char value){ return char32{(char32_t)value}; }>;

    /// @brief Returns an iterator to the start of the view
    /// @return Iterator to the start of the view
    constexpr iterator begin() const noexcept { return _ptr; }
    /// @brief Returns an iterator to the end of the view.
    /// This iterator should not be dereferenced.
    /// @return Iterator to the end of the view
    constexpr iterator end() const noexcept { return _ptr + _size; }

    /// @brief Returns a pointer to the beginning of the data.
    /// The pointer may be NUL only if size() == 0.
    /// @return Pointer to the beginning of the data
    constexpr auto data() const noexcept { return _ptr; }

    /// @brief The number of characters over which the view is spanning.
    /// For ZStringView, this does not include the NUL-terminator
    /// @return The byte size
    constexpr size_t size() const noexcept { return _size; }
    /// @brief Returns the byte size over which the view is spanning.
    /// For ASCII, this is the same as 'size()'.
    /// @return The byte size
    constexpr size_t byte_size() const noexcept { return _size; }
    /// @brief Check if the size is zero
    /// @return True if size() == 0
    constexpr bool is_empty() const noexcept { return size() == 0; }

    /// @brief Returns the char at index 'index'
    /// @pre index < size()
    /// @param index The index of the char to return
    /// @return The char at index 'index'
    constexpr char32 operator[](size_t index) const noexcept
    {
      assert_true("Invalid index!", index < size());
      return static_cast<char32_t>(_ptr[index]);
    }

    /// @brief Get the front of the view.
    /// @return The first item of the view
    constexpr char32 front() const noexcept { return (*this)[0]; }

    /// @brief Get the back of the view.
    /// @return The last item of the view
    constexpr char32 back() const noexcept
    {
      assert_true("Invalid size!", size() > 0);
      return (*this)[_size - 1];
    }

    /// @brief Returns a NUL-terminated string.
    /// This is always guaranteed to work for ZStringView.
    /// @return NUL-terminated string
    constexpr const char* c_str() const noexcept
      requires ZSTRING
    {
      return data();
    }

    /// @brief Shortens the view from the front by 1.
    /// @return Self
    constexpr BasicStringView& pop_front() noexcept
    {
      assert_true("StringView was empty!", !is_empty());
      ++_ptr;
      --_size;
      return *this;
    }

    /// @brief Shortens the view from the front by N.
    /// @param N The number of objects to pop
    /// @return Self
    constexpr BasicStringView& pop_front_n(size_t N) noexcept
    {
      assert_true("StringView does not contain enough objects!", N <= size());
      _ptr += N;
      _size -= N;
      return *this;
    }

    /// @brief Shortens the view from the back by 1.
    /// @return Self
    constexpr BasicStringView& pop_back() noexcept
      requires(!ZSTRING)
    {
      assert_true("StringView was empty!", !is_empty());
      --_size;
      return *this;
    }

    /// @brief Shortens the view from the back by N.
    /// @param N The number of objects to pop
    /// @return Self
    constexpr BasicStringView pop_back_n(size_t N) noexcept
      requires(!ZSTRING)
    {
      assert_true("StringView does not contain enough objects!", N <= size());
      _size -= N;
      return *this;
    }

    /// @brief Lexicographically compare two views
    /// @param v1 The first view
    /// @param v2 The second view
    /// @return Result of comparison
    friend constexpr auto operator<=>(
        const BasicStringView& v1, const BasicStringView& v2) noexcept
    {
      return std::lexicographical_compare_three_way(
          v1.begin(), v1.end(), v2.begin(), v2.end());
    }
  };

  /// @brief Represents a view over contiguous characters
  /// @tparam ENCODING The encoding of the StringView
  template<StringEncoding ENCODING>
  using StringView = BasicStringView<ENCODING, false>;

  /// @brief Represents a NUL-terminated StringView
  /// @tparam ENCODING The encoding of the StringView
  template<StringEncoding ENCODING>
  using ZStringView = BasicStringView<ENCODING, true>;
} // namespace clt

template<bool ZSTRING>
struct fmt::formatter<clt::BasicStringView<clt::StringEncoding::ASCII, ZSTRING>>
{
  using to_format = clt::BasicStringView<clt::StringEncoding::ASCII, ZSTRING>;

  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const to_format& vec, FormatContext& ctx) const
  {
    return fmt::format_to(ctx.out(), "{}", fmt::string_view{vec.data(), vec.size()});
  }
};

#endif // !HG_DSA_STRING_VIEW
