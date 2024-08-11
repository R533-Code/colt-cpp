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
  class BasicStringView
  {
  public:
    using underlying_type = meta::encoding_to_char_t<ENCODING>;
    /// @brief The string encoding of the current BasicStringView
    static constexpr StringEncoding string_encoding = ENCODING;
    /// @brief True if null terminated view
    static constexpr bool is_zstring_view = ZSTRING;

  private:
    /// @brief The pointer to the characters
    const underlying_type* _ptr{};
    /// @brief The count of characters in the view
    size_t _size{};

  public:
    /// @brief Constructs an empty StringView
    constexpr BasicStringView() noexcept
      requires(!ZSTRING)
    = default;

    /// @brief Constructs an empty ZStringView
    constexpr BasicStringView() noexcept
      requires ZSTRING
        : _ptr(meta::empty_string_literal<underlying_type>())
        , _size(0)
    {
    }

    template<size_t N>
    constexpr BasicStringView(const underlying_type (&str)[N]) noexcept
        : _ptr(str)
        , _size(N)
    {
    }

    /// @brief Constructs a view starting at 'ptr', of size 'size'.
    /// For ZStringView, the character AFTER the last character MUST
    /// be a NUL-terminator: `ptr[size] == '\0'`.
    /// For StringView, ptr can be null only if 'size' is zero.
    /// @param ptr The start of the view
    /// @param size The size of the view (in count of objects)
    constexpr BasicStringView(const underlying_type* ptr, size_t size) noexcept
        : _ptr(ptr)
        , _size(size)
    {
      if constexpr (!ZSTRING)
      {
        assert_true(
            "ptr cannot be null if size != 0!",
            implies(_ptr == nullptr, _size == 0));
      }
      else
      {
        assert_true("ptr cannot be null for ZStringView!", _ptr != nullptr);
        assert_true(
            "ZStringView must be NUL-terminated!", _ptr[_size] == underlying_type{});
      }
    }

    /// @brief Constructs a view starting at 'ptr', of size 'begin - end'.
    /// @pre begin <= end
    /// For ZStringView, the character AFTER the last character MUST
    /// be a NUL-terminator: `begin[size()] == '\0'`.
    /// For StringView, ptr can be null only if 'size' is zero.
    /// @param begin The start of the view
    /// @param end The end of the view
    constexpr BasicStringView(
        const underlying_type* begin, const underlying_type* end) noexcept
        : _ptr(begin)
        , _size(end - begin)
    {
      assert_true("begin must be <= end!", begin <= end);
      if constexpr (!ZSTRING)
      {
        assert_true(
            "ptr cannot be null if size != 0!",
            implies(_ptr == nullptr, _size == 0));
      }
      else
      {
        assert_true(
            "ptr cannot be null for ZStringView!", begin != nullptr, end != nullptr);
        assert_true(
            "ZStringView must be NUL-terminated!", *end == underlying_type{});
      }
    }

    /// @brief Constructs a view from a NUL-terminated string.
    /// @pre ptr != nullptr
    /// @param ptr The NUL-terminated string
    constexpr BasicStringView(const underlying_type* ptr) noexcept
        : _ptr(ptr)
        , _size(clt::bytelen(ptr) / sizeof(underlying_type))
    {
      assert_true("ptr cannot be null!", _ptr != nullptr);
    }

    MAKE_DEFAULT_COPY_AND_MOVE_FOR(BasicStringView);

    /// @brief Returns an iterator to the start of the view
    /// @return Iterator to the start of the view
    constexpr uni::CodePointIterator<ENCODING> begin() const noexcept
    {
      return _ptr;
    }
    /// @brief Returns an iterator to the end of the view.
    /// This iterator should not be dereferenced.
    /// @return Iterator to the end of the view
    constexpr uni::CodePointIterator<ENCODING> end() const noexcept
    {
      return _ptr + _size;
    }

    /// @brief Returns a pointer to the beginning of the data.
    /// The pointer may be NUL only if size() == 0.
    /// @return Pointer to the beginning of the data
    constexpr auto data() const noexcept { return _ptr; }

    /// @brief The number of characters over which the view is spanning.
    /// For ZStringView, this does not include the NUL-terminator
    /// @return The byte size
    constexpr size_t size() const noexcept
    {
      if constexpr (meta::is_any_of<underlying_type, char, Char32BE, Char32LE>)
        return _size;
      else
        return clt::strlen(_ptr);
    }
    /// @brief Returns the byte size over which the view is spanning.
    /// For ASCII, this is the same as 'size()'.
    /// @return The byte size
    constexpr size_t byte_size() const noexcept
    {
      return _size * sizeof(underlying_type);
    }
    /// @brief Check if the size is zero
    /// @return True if size() == 0
    constexpr bool is_empty() const noexcept { return size() == 0; }

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
    /// Do not use this operator to iterate over the view:
    /// use a ranged for loop for performance.
    /// Same as 'index_front'.
    /// @pre index < size()
    /// @param index The index of the char to return
    /// @return The char at index 'index'
    constexpr char32_t index_front(size_t index) const noexcept
    {
      assert_true("Invalid index!", index < size());
      return uni::index_front(_ptr, index);
    }
    
    constexpr char32_t index_back(u32 index) const noexcept
    {
      assert_true("Invalid index!", index < size());
      return uni::index_back(_ptr + _size - 1, index);
    }

    /// @brief Get the front of the view.
    /// @return The first item of the view
    constexpr char32_t front() const noexcept
    {
      assert_true("Invalid size!", !is_empty());
      return index_front(0);
    }

    /// @brief Get the back of the view.
    /// @return The last item of the view
    constexpr char32_t back() const noexcept
    {
      assert_true("Invalid size!", !is_empty());
      return index_back(0);
    }

    /// @brief Returns a NUL-terminated string.
    /// This is always guaranteed to work for ZStringView.
    /// @return NUL-terminated string
    constexpr const underlying_type* c_str() const noexcept
      requires ZSTRING
    {
      return data();
    }

    /// @brief Shortens the view from the front by 1.
    /// @return Self
    constexpr BasicStringView& pop_front() noexcept
    {
      assert_true("StringView was empty!", !is_empty());
      if constexpr (meta::is_any_of<underlying_type, char, Char32BE, Char32LE>)
      {
        ++_ptr;
        --_size;
      }
      if constexpr (meta::is_any_of<underlying_type, Char8>)
      {
        auto len = _ptr->sequence_length();
        assert_true("Invalid UTF8!", len.is_value());
        _ptr += *len;
        _size -= *len;
      }
      if constexpr (meta::is_any_of<underlying_type, Char16BE, Char16LE>)
      {
        auto len = _ptr->sequence_length();
        assert_true(
            "Invalid UTF16!", implies(len == 2, _ptr[1].is_trail_surrogate()));
        _ptr += len;
        _size -= len;
      }
      return *this;
    }

    /// @brief Shortens the view from the front by N.
    /// @param N The number of objects to pop
    /// @return Self
    constexpr BasicStringView& pop_front_n(size_t N) noexcept
    {
      assert_true("StringView does not contain enough objects!", N <= size());
      while (N != 0)
      {
        pop_front();
        N--;
      }
      return *this;
    }

    /// @brief Shortens the view from the back by 1.
    /// @return Self
    constexpr BasicStringView& pop_back() noexcept
      requires(!ZSTRING)
    {
      assert_true("StringView was empty!", !is_empty());
      if constexpr (meta::is_any_of<underlying_type, char, Char32BE, Char32LE>)
        --_size;
      if constexpr (meta::is_any_of<underlying_type, Char8>)
      {
        auto ptr       = _ptr + _size - 1;
        auto copy_size = _size;
        do
        {
          --copy_size;
        } while ((ptr--)->is_trail());
        assert_true("Invalid UTF8!", _size - copy_size <= Char8::max_sequence);
        _size = copy_size;
      }
      if constexpr (meta::is_any_of<underlying_type, Char16BE, Char16LE>)
      {
        auto ptr = _ptr + _size - 1;
        if (ptr->is_trail_surrogate())
        {
          assert_true("Invalid UTF16!", ptr[-1].is_lead_surrogate(), _size > 1);
          _size -= 2;
        }
        else
          --_size;
      }
      return *this;
    }

    /// @brief Shortens the view from the back by N.
    /// @param N The number of objects to pop
    /// @return Self
    constexpr BasicStringView pop_back_n(size_t N) noexcept
      requires(!ZSTRING)
    {
      assert_true("StringView does not contain enough objects!", N <= size());
      while (N != 0)
      {
        pop_back();
        N--;
      }
      return *this;
    }

    /// @brief Lexicographically compare two views
    /// @param v1 The first view
    /// @param v2 The second view
    /// @return Result of comparison
    friend constexpr auto operator<=>(
        const BasicStringView& v1, const BasicStringView& v2) noexcept
    {
      //TODO: UTF8 can be compared as unsigned bytes...
      // UTF16 cannot!
      // https://stackoverflow.com/questions/3478985/is-comparing-two-byte-of-utf-8-encoded-strings-the-same-as-comparing-two-unico
      return std::lexicographical_compare_three_way(
          v1.begin(), v1.end(), v2.begin(), v2.end());
    }
  };

  /// @brief Represents a view over contiguous characters
  /// @tparam ENCODING The encoding of the StringView
  template<StringEncoding ENCODING = StringEncoding::ASCII>
  using StringView = BasicStringView<ENCODING, false>;

  /// @brief Represents a NUL-terminated StringView
  /// @tparam ENCODING The encoding of the StringView
  template<StringEncoding ENCODING = StringEncoding::ASCII>
  using ZStringView = BasicStringView<ENCODING, true>;

  /// @brief UTF8 StringView
  using u8StringView = StringView<StringEncoding::UTF8>;
  /// @brief UTF16 StringView (in host endianness)
  using u16StringView = StringView<StringEncoding::UTF16>;
  /// @brief UTF32 StringView (in host endianness)
  using u32StringView = StringView<StringEncoding::UTF32>;
  /// @brief UTF8 Nul-terminated StringView
  using u8ZStringView = ZStringView<StringEncoding::UTF8>;
  /// @brief UTF16 Nul-terminated StringView
  using u16ZStringView = ZStringView<StringEncoding::UTF16>;
  /// @brief UTF32 Nul-terminated StringView
  using u32ZStringView = ZStringView<StringEncoding::UTF32>;
} // namespace clt

template<clt::StringEncoding ENCODING, bool ZSTRING>
struct fmt::formatter<clt::BasicStringView<ENCODING, ZSTRING>>
{
  using to_format = clt::BasicStringView<ENCODING, ZSTRING>;

  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const to_format& vec, FormatContext& ctx) const
  {
    using namespace clt;
    using enum StringEncoding;
    if constexpr (ENCODING == ASCII)
    {
      return fmt::format_to(
          ctx.out(), "{}", fmt::string_view{vec.data(), vec.size()});
    }
    if constexpr (ENCODING == UTF8)
    {
      return fmt::format_to(
          ctx.out(), "{}",
          fmt::string_view{
              reinterpret_cast<const char*>(vec.data()), vec.byte_size()});
    }
  }
};

#endif // !HG_DSA_STRING_VIEW
