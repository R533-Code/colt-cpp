#ifndef HG_COLT_STATIC_VECTOR
#define HG_COLT_STATIC_VECTOR

#include "./vector.h"

namespace clt
{
  template<typename T, size_t MAX_SIZE>
    requires (MAX_SIZE != 0)
  /// @brief Dynamic size array, that can make use of a local allocator
  /// @tparam T The type stored in the Vector
  class StaticVector
  {
    /// @brief Count of active objects in the block
    size_t blk_size = 0;
    /// @brief Stack buffer for the objects
    alignas(alignof(T)) char buffer[sizeof(T) * MAX_SIZE];

    /// @brief Returns a pointer to the internal buffer
    /// @return Pointer to the buffer
    constexpr T* ptr() noexcept { return details::ptr_to<T*>(buffer); }
    /// @brief Returns a pointer to the internal buffer
    /// @return Pointer to the buffer
    constexpr const T* ptr() const noexcept { return details::ptr_to<T*>(buffer); }

  public:
    /// @brief Default constructor (when allocator is global)
    constexpr StaticVector() noexcept = default;

    /// @brief Default constructor (when allocator is global)
    /// @param reserve The count of objects to allocate for
    constexpr StaticVector(View<T> to_copy) noexcept
      COLT_PRE(to_copy.size() <= MAX_SIZE)
    {
      details::contiguous_copy(to_copy.begin(), ptr(), to_copy.size());
      blk_size = to_copy.size();
    }
    COLT_POST()

    template<typename... Args>
    /// @brief Constructs 'size' objects using 'args'
    /// @tparam ...Args The parameter pack
    /// @param size The count of object to construct
    /// @param  Tag helper
    /// @param ...args The argument pack
    constexpr StaticVector(size_t sz, meta::InPlaceT, Args&&... args)
      noexcept(std::is_nothrow_constructible_v<T, Args...>)
      COLT_PRE(sz <= MAX_SIZE)
    {
      details::contiguous_construct(ptr(), sz, std::forward<Args>(args)...);
      blk_size = sz;
    }
    COLT_POST()

    /// @brief Constructs a Vector from an initializer_list
    /// @param list The initializer list
    constexpr StaticVector(std::initializer_list<T> list)
      noexcept(std::is_nothrow_copy_constructible_v<T>)
      : blk_size(std::size(list))
      COLT_PRE(blk_size <= MAX_SIZE)
    {
      details::contiguous_copy(std::data(list), ptr(), blk_size);
    }
    COLT_POST()

    /// @brief Copy constructor, copy the content from 'to_copy'
    /// @param to_copy 
    /// @return Self
    template<size_t OTHER_SIZE> requires (OTHER_SIZE <= MAX_SIZE)
    explicit constexpr StaticVector(const StaticVector<T, OTHER_SIZE>& to_copy)
      noexcept(std::is_nothrow_copy_constructible_v<T>)
      : blk_size(to_copy.blk_size)
    {
      details::contiguous_copy(to_copy.ptr(), ptr(), blk_size);
    }

    /// @brief Destroy the active objects and copy the content from 'to_copy'.
    /// @param to_copy The Vector whose objects to copy
    /// @return Self
    template<size_t OTHER_SIZE> requires (OTHER_SIZE <= MAX_SIZE)
    constexpr StaticVector& operator=(const StaticVector<T, OTHER_SIZE>& to_copy)
      noexcept(std::is_nothrow_copy_constructible_v<T>
        && std::is_nothrow_destructible_v<T>)
      COLT_PRE(&to_copy != this)
    {
      details::contiguous_destruct(ptr(), blk_size);
      details::contiguous_copy(to_copy.ptr(), ptr(), to_copy.blk_size);
      blk_size = to_copy.blk_size;
      return *this;
    }
    COLT_POST()

    /// @brief Move constructor
    /// @param to_move The Vector whose resources to steal
    template<size_t OTHER_SIZE> requires (OTHER_SIZE <= MAX_SIZE)      
    explicit constexpr StaticVector(StaticVector<T, OTHER_SIZE>&& to_move)
      noexcept(std::is_nothrow_move_constructible_v<T>
        && std::is_nothrow_destructible_v<T>)
      : blk_size(std::exchange(to_move.blk_size, 0))
    {
      details::contiguous_destructive_move(to_move.ptr(), ptr(), blk_size);
    }

    /// @brief Move assignment operator, move-destroys content of 'to_move' to this
    /// @param to_move The StaticVector being assigned
    /// @return Self
    template<size_t OTHER_SIZE> requires (OTHER_SIZE <= MAX_SIZE)
    constexpr StaticVector& operator=(StaticVector<T, OTHER_SIZE>&& to_move) noexcept
      COLT_PRE(&to_move != this)
    {
      blk_size = std::exchange(to_move.blk_size, 0);
      details::contiguous_destructive_move(to_move.ptr(), ptr(), blk_size);
      return *this;
    }
    COLT_POST()

    /// @brief Destructor, destroy all active objects and free memory
    constexpr ~StaticVector()
      noexcept(std::is_nothrow_destructible_v<T>)
    {
      details::contiguous_destruct(ptr(), blk_size);
    }

    /// @brief Returns a pointer to the beginning of the data
    /// @return Const pointer to the beginning of the data (never null)
    constexpr const T* data() const noexcept { return ptr(); }
    /// @brief Returns a pointer to the beginning of the data
    /// @return Pointer to the beginning of the data (never null)
    constexpr T* data() noexcept { return ptr(); }

    /// @brief Returns the count of active objects in the StaticVector
    /// @return The count of objects in the Vector
    constexpr size_t size() const noexcept { return blk_size; }
    /// @brief Returns the capacity of the current allocation
    /// @return The capacity of the current allocation
    constexpr size_t capacity() const noexcept { return MAX_SIZE; }

    /// @brief Returns the object at index 'index' of the StaticVector.
    /// @param index The index of the object
    /// @return The object at index 'index'
    constexpr meta::copy_trivial_t<const T&> operator[](size_t index) const noexcept
      COLT_PRE(index < this->size())
      return ptr()[index];
    COLT_POST()

    /// @brief Returns a reference to the object at index 'index' of the StaticVector.
    /// @param index The index of the object
    /// @return The object at index 'index'
    constexpr T& operator[](size_t index) noexcept
      COLT_PRE(index < this->size())
      return ptr()[index];
    COLT_POST()

    /// @brief Check if the StaticVector does not contain any object.
    /// Same as: size() == 0
    /// @return True if the StaticVector is empty
    constexpr bool is_empty() const noexcept { return blk_size == 0; }

    /// @brief Push an object at the end of the StaticVector by copying if there is enough capacity
    /// @param to_copy The object to copy at the end of the StaticVector
    constexpr void push_back(meta::copy_trivial_t<const T&> to_copy)
      noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
      if (blk_size == MAX_SIZE)
        return;
      new(ptr() + blk_size) T(to_copy);
      ++blk_size;
    }

    /// @brief Push an object at the end of the StaticVector by moving if there is enough capacity
    /// @param to_move The object to move at the end of the StaticVector
    constexpr void push_back(T&& to_move)
      noexcept(std::is_nothrow_move_constructible_v<T>) requires (!std::is_trivial_v<T>)
    {
      if (blk_size == MAX_SIZE)
        return;
      new(ptr() + blk_size) T(std::move(to_move));
      ++blk_size;
    }

    template<typename... Args>
    /// @brief Emplace an object at the end of the StaticVector if there is enough capacity
    /// @tparam ...Args The parameter pack
    /// @param  InPlaceT tag
    /// @param ...args The argument pack to forward to the constructor
    constexpr void push_back(meta::InPlaceT, Args&&... args)
      noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
      if (blk_size == MAX_SIZE)
        return;
      new(ptr() + blk_size) T(std::forward<Args>(args)...);
      ++blk_size;
    }

    /// @brief Pops an item from the back of the StaticVector.
    constexpr void pop_back()
      noexcept(std::is_nothrow_destructible_v<T>)
      COLT_PRE(!this->is_empty())
    {
      --blk_size;
      ptr()[blk_size].~T();
    }
    COLT_POST()

    /// @brief Pops N item from the back of the StaticVector.
    /// @param N The number of item to pop from the back
    constexpr void pop_back_n(size_t N)
      noexcept(std::is_nothrow_destructible_v<T>)
      COLT_PRE(N <= this->size())
    {
      for (size_t i = blk_size - N; i < blk_size; i++)
        ptr()[i].~T();
      blk_size -= N;
    }
    COLT_POST()

    /// @brief Removes all the item from the StaticVector
    constexpr void clear()
      noexcept(std::is_nothrow_destructible_v<T>)
    {
      details::contiguous_destruct(ptr(), blk_size);
      blk_size = 0;
    }

    /// @brief Returns the first item in the StaticVector.
    /// @return The first item in the StaticVector.
    constexpr meta::copy_trivial_t<const T&> front() const noexcept
      COLT_PRE(!this->is_empty())
      return *ptr();
    COLT_POST()

    /// @brief Returns the first item in the StaticVector.
    /// @return The first item in the StaticVector.
    constexpr T& front() noexcept
      COLT_PRE(!this->is_empty())
      return *ptr();
    COLT_POST()

    /// @brief Returns the last item in the StaticVector.
    /// @return The last item in the StaticVector.
    constexpr meta::copy_trivial_t<const T&> back() const noexcept
      COLT_PRE(!this->is_empty())
      return ptr()[blk_size - 1];
    COLT_POST()

    /// @brief Returns the last item in the StaticVector.
    /// @return The last item in the StaticVector.
    constexpr T& back() noexcept
      COLT_PRE(!this->is_empty())
      return ptr()[blk_size - 1];
    COLT_POST()

    /// @brief Returns an iterator the beginning of the StaticVector
    /// @return Iterator to the beginning of the StaticVector
    constexpr T* begin() noexcept { return ptr(); }
    /// @brief Returns an iterator the beginning of the StaticVector
    /// @return Iterator to the beginning of the Vector
    constexpr const T* begin() const noexcept { return ptr(); }

    /// @brief Returns an iterator the end of the StaticVector
    /// @return Iterator to the end of the StaticVector
    constexpr T* end() noexcept { return ptr() + blk_size; }
    /// @brief Returns an iterator the end of the StaticVector
    /// @return Iterator to the end of the StaticVector
    constexpr const T* end() const noexcept { return ptr() + blk_size; }

    /// @brief Converts a Vector to a View
    /// @return View over the whole Vector
    constexpr operator View<T>() const noexcept
    {
      return Span{ begin(), end() };
    }

    /// @brief Converts a Vector to a Span
    /// @return Span over the whole Vector
    constexpr operator Span<T>() noexcept
    {
      return Span{ begin(), end() };
    }

    /// @brief Check if every object of v1 and v2 are equal
    /// @param v1 The first Vector
    /// @param v2 The second Vector
    /// @return True if both Vector are equal
    friend constexpr bool operator==(const StaticVector& v1, const View<T>& v2) noexcept
    {
      if (v1.size() != v2.size())
        return false;
      for (size_t i = 0; i < v1.size(); i++)
        if (v1[i] != v2[i])
          return false;
      return true;
    }

    /// @brief Lexicographically compare two vectors
    /// @param v1 The first vector
    /// @param v2 The second vector
    /// @return Result of comparison
    friend constexpr auto operator<=>(const StaticVector& v1, const View<T>& v2) noexcept
    {
      return std::lexicographical_compare_three_way(
        v1.begin(), v1.end(), v2.begin(), v2.end()
      );
    }
  };

  template<meta::Parsable T, size_t MAX_SIZE>
  /// @brief Overload for Vector
  struct str::parser<StaticVector<T, MAX_SIZE>>
    : str::Recommended<str::recommended_size<T>() * 6>
  {
    constexpr ParseResult operator()(maybe_out<StaticVector<T, MAX_SIZE>> result, StringView to_parse) const noexcept
    {
      if (to_parse.size() < 2 || to_parse.front() != '[' || to_parse.back() != ']' || to_parse.count(',', 1) >= MAX_SIZE)
        return ParseResult{ to_parse.end(), ParseErrorCode::INVALID_FMT };
      auto end = to_parse.end();
      //Pop '[' and ']'
      to_parse.pop_front(); to_parse.pop_back();

      //Construct the Vector
      result.construct();

      if (to_parse.is_empty())
        return ParseResult{ end, ParseErrorCode::SUCCESS };

      u64 offset = 0;
      u64 find_i = to_parse.find(',', offset);
      do
      {
        auto obj = to_parse.substr(offset, find_i - offset);
        obj.strip();

        uninit<T> try_obj;
        auto [ptr, err] = clt::str::parser<T>{}(try_obj, obj);
        if (err != ParseErrorCode::SUCCESS)
        {
          //Destroy the object as it was constructed...
          result.underlying().destruct();
          return ParseResult{ obj.end(), err };
        }

        ON_SCOPE_EXIT{
          //Destroy the object as it was constructed...
          try_obj.destruct();
        };

        if (ptr != obj.end())
        {
          //Destroy the object as it was constructed...
          result.underlying().destruct();
          return ParseResult{ ptr, ParseErrorCode::INVALID_FMT };
        }

        result.data().push_back(std::move(try_obj.data()));
        //After ','
        offset = find_i + 1;
        find_i = to_parse.find(',', offset);
        //Due to overflow, when find_i is npos, find_i + 1 == 0.
        if (offset == 0)
          break;
      } while (true);

      return ParseResult{ end, ParseErrorCode::SUCCESS };
    }
  };
}

template<typename T, size_t MAX_CAPACITY> requires fmt::is_formattable<T>::value
struct fmt::formatter<clt::StaticVector<T, MAX_CAPACITY>>
{
  bool human_readable = false;

  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    auto end = ctx.end();
    if (it == end)
      return it;
    if (*it == 'h')
    {
      ++it;
      human_readable = true;
    }
    assert_true("Possible format for Vector are: {} or {:h}!", *it == '}');
    return it;
  }

  template<typename FormatContext>
  auto format(const clt::StaticVector<T, MAX_CAPACITY>& vec, FormatContext& ctx)
  {
    auto fmt_to = ctx.out();
    if (human_readable)
    {
      if (!vec.is_empty())
        fmt_to = fmt::format_to(fmt_to, "{}", vec.front());
      if (vec.size() > 1)
      {
        for (size_t i = 1; i < vec.size() - 1; i++)
          fmt_to = fmt::format_to(fmt_to, ", {}", vec[i]);
        fmt_to = fmt::format_to(fmt_to, " and {}", vec.back());
      }
      return fmt_to;
    }
    else
    {
      fmt_to = fmt::format_to(fmt_to, "[");
      if (!vec.is_empty())
        fmt_to = fmt::format_to(fmt_to, "{}", vec.front());
      for (size_t i = 1; i < vec.size(); i++)
        fmt_to = fmt::format_to(fmt_to, ", {}", vec[i]);
      return fmt::format_to(fmt_to, "]");
    }
  }
};

#endif //!HG_COLT_STATIC_VECTOR