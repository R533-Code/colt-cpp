#ifndef HG_COLT_SPAN
#define HG_COLT_SPAN

#include <type_traits>

#include "./helper.h"

namespace clt
{
  template<typename T>
  /// @brief [Non]-Owning view over contiguous objects
  /// @tparam T The type of the objects (if const then non-owning)
  class Span
  {
    /// @brief Pointer to the beginning of the view, can be null
    T* begin_ptr;
    /// @brief Count of items in the view
    size_t count;

    /// @brief Helper type for non-owning optimization
    using ref_or_copy = std::conditional_t<std::is_const_v<T>, meta::copy_trivial_t<T&>, T&>;

  public:

    /*********************************
    *         CONSTRUCTORS
    *********************************/

    constexpr Span(const Span&) noexcept = default;
    constexpr Span(Span&&) noexcept = default;
    constexpr Span& operator=(Span&&) noexcept = default;
    constexpr Span& operator=(const Span&) noexcept = default;

    /// @brief Constructs a view over the range [begin, begin + view_size).
    /// @param begin The beginning of the view
    /// @param view_size The size of the view
    constexpr Span(T* begin, size_t view_size) noexcept
      : begin_ptr(begin), count(view_size) {}

    /// @brief Constructs a view over the range [begin, end).
    /// @param begin The beginning of the view
    /// @param end The end of the view
    constexpr Span(T* begin, T* end) noexcept
      : begin_ptr(begin), count(end - begin)
    COLT_PRE(begin < end) COLT_POST()

    /// @brief Returns an iterator to the beginning of the view.
    /// @return Iterator to the beginning of the view
    constexpr T* begin() const noexcept { return begin_ptr; }

    /// @brief Returns an iterator past the end of the view.
    /// @return Iterator to the end of the view
    constexpr T* end() const noexcept { return begin_ptr + count; }

    /// @brief Returns a pointer to the beginning of the view.
    /// @return Pointer to the beginning of the view
    constexpr T* data() const noexcept { return begin_ptr; }

    /// @brief Returns the count of object the view spans on.
    /// @return The count of objects
    constexpr size_t size() const noexcept { return count; }

    /// @brief Check if the view is empty.
    /// @return True if the size of the view is 0
    constexpr bool is_empty() const noexcept { return count == 0; }

    /// @brief Get the front of the view.
    /// @return The first item of the view
    constexpr ref_or_copy front() const noexcept
      COLT_PRE(!is_empty())
      return *begin_ptr;
    COLT_POST()

    /// @brief Get the back of the view.
    /// @return The last item of the view
    constexpr ref_or_copy back() const noexcept
      COLT_PRE(!is_empty())
      return begin_ptr[count - 1];
    COLT_POST()

    /// @brief Shortens the view from the front by 1.
    /// @return Self
    constexpr Span pop_front() noexcept
      COLT_PRE(!is_empty())
    {
      ++begin_ptr;
      --count;
      return *this;
    }
    COLT_POST()

    /// @brief Shortens the view from the front by N.
    /// @param N The number of objects to pop
    /// @return Self
    constexpr Span pop_front_n(size_t N) noexcept
      COLT_PRE(N <= this->size())
    {
      begin_ptr += N;
      count -= N;
      return *this;
    }
    COLT_POST()

    /// @brief Shortens the view from the back by 1.
    /// @return Self
    constexpr Span pop_back() noexcept
      COLT_PRE(!is_empty())
    {
      --count;
      return *this;
    }
    COLT_POST()

    /// @brief Shortens the view from the back by N.
    /// @param N The number of objects to pop
    /// @return Self
    constexpr Span pop_back_n(size_t N) noexcept
      COLT_PRE(N <= this->size())
    {
      count -= N;
      return *this;
    }
    COLT_POST()

    /// @brief Returns the object at index 'index' of the view.
    /// @param index The index of the object
    /// @return The object at index 'index'
    constexpr ref_or_copy operator[](size_t index) const noexcept
      COLT_PRE(index < this->size())
      return begin_ptr[index];
    COLT_POST()

    /// @brief Check if every object of v1 and v2 are equal
    /// @param v1 The first Span
    /// @param v2 The second Span
    /// @return True if both Span are equal
    friend constexpr bool operator==(const Span& v1, const Span& v2) noexcept
    {
      if (v1.size() != v2.size())
        return false;
      for (size_t i = 0; i < v1.size(); i++)
        if (v1[i] != v2[i])
          return false;
      return true;
    }

    /// @brief Lexicographically compare two spans
    /// @param v1 The first Span
    /// @param v2 The second Span
    /// @return Result of comparison
    friend constexpr auto operator<=>(const Span& v1, const Span& v2) noexcept
    {
      return std::lexicographical_compare_three_way(
        v1.begin(), v1.end(), v2.begin(), v2.end()
      );
    }
  };

  template<typename T>
  /// @brief A View is a Span over const objects
  /// @tparam T The type of the View
  using View = Span<std::add_const_t<T>>;
}

template<typename T> requires fmt::is_formattable<T>::value
struct fmt::formatter<clt::Span<T>>
{
  bool human_readable = false;

  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin(), end = ctx.end();
    if (it == end)
      return it;
    if (*it == 'h')
    {
      ++it;
      human_readable = true;
    }
    assert_true("Possible format for Span/View are: {} or {:h}!", *it == '}');
    return it;
  }

  template<typename FormatContext>
  auto format(const clt::Span<T>& vec, FormatContext& ctx)
  {
    auto fmt_to = ctx.out();
    if (human_readable)
    {
      if (!vec.is_empty())
        fmt_to = fmt::format_to(fmt_to, "{}", vec.front());
      for (size_t i = 1; i < vec.size() - 1; i++)
        fmt_to = fmt::format_to(fmt_to, ", {}", vec[i]);
      if (vec.size() != 1)
        fmt_to = fmt::format_to(fmt_to, " and {}", vec.back());
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

#endif //!HG_COLT_SPAN