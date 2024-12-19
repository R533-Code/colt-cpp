/*****************************************************************/ /**
 * @file   iterator.h
 * @brief  Contains iterator related helpers.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_DSA_ITERATOR
#define HG_DSA_ITERATOR

#include <concepts>
#include <iterator>
#include <functional>

#include "colt/typedefs.h"

namespace clt
{
  /// @brief Iterator that transforms the output of another iterator
  /// @tparam Iter The underlying iterator
  /// @tparam TRANSFORM The transformation to apply
  template<std::input_iterator Iter, auto TRANSFORM>
    requires std::invocable<decltype(TRANSFORM), std::iter_value_t<Iter>>
  class transform_iter
  {
    /// @brief The underlying iterator
    Iter iterator;

  public:
    /// @brief Default constructor that default constructs the underlying iterator
    constexpr transform_iter()
      requires std::default_initializable<Iter>
        : iterator({})
    {
    }

    /// @brief Constructor that forwards its arguments to the underlying iterator
    template<typename Arg, typename... Args>
      requires(!std::same_as<std::remove_cvref_t<Arg>, transform_iter>)
    constexpr transform_iter(Arg&& arg, Args&&... args)
        : iterator(std::forward<Arg>(arg), std::forward<Args>(args)...)
    {
    }

    MAKE_DEFAULT_COPY_AND_MOVE_FOR(transform_iter);

    /// @brief Prefix increment
    /// @return Self
    constexpr transform_iter& operator++()
    {
      ++iterator;
      return *this;
    }

    /// @brief Postfix increment
    /// @param  Postfix
    /// @return Copy of the current iterator
    constexpr transform_iter operator++(int)
    {
      auto copy = *this;
      ++(*this);
      return copy;
    }

    /// @brief Prefix decrement
    /// @return Self
    constexpr transform_iter& operator--()
    {
      --iterator;
      return *this;
    }
    /// @brief Postfix decrement
    /// @param  Postfix
    /// @return Copy of the current iterator
    constexpr transform_iter operator--(int)
    {
      auto copy = *this;
      --(*this);
      return copy;
    }

    /// @brief Returns the current transformed value
    /// @return The current transformed value
    constexpr auto operator*() { return std::invoke(TRANSFORM, *iterator); }

    /// @brief Compares the two iterators
    /// @param  First value
    /// @param  Second value
    /// @return True if equal
    friend constexpr bool operator==(
        const transform_iter&, const transform_iter&) noexcept = default;
  };

  template<u64 begin, u64 end>
  /// @brief Iterator from that returns value from [begin, end]
  class range_iterator
  {
    /// @brief The beginning of the Range
    u64 current = begin;

  public:
    /// @brief Default constructor
    constexpr range_iterator() noexcept = default;
    /// @brief Set the starting value to current
    /// @param current The starting value
    constexpr range_iterator(u64 current) noexcept
        : current(current)
    {
    }

    /// @brief Reads the current value of the range
    /// @return The current value of the range
    constexpr u64 operator*() const noexcept { return current; }

    /// @brief Increments the iterator to the next value in the range
    /// @return Self
    constexpr range_iterator& operator++() noexcept
    {
      ++current;
      return *this;
    }

    /// @brief Increments the iterator to the next value in the range, returning the old value
    /// @return Copy of old iterator
    constexpr range_iterator operator++(int) noexcept
    {
      range_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    /// @brief Decrements the iterator to the next value in the range
    /// @return Self
    constexpr range_iterator& operator--() noexcept
    {
      --current;
      return *this;
    }

    /// @brief Decrements the iterator to the next value in the range, returning the old value
    /// @return Copy of old iterator
    constexpr range_iterator operator--(int) noexcept
    {
      range_iterator tmp = *this;
      --(*this);
      return tmp;
    }

    friend constexpr bool operator==(const range_iterator&, const range_iterator&) =
        default;
  };
  template<typename To, typename Iter>
  /// @brief Converts the return of an iterator to another type
  class convert_iterator
  {
    /// @brief The internal iterator
    Iter iterator;

  public:
    /// @brief Default constructor
    constexpr convert_iterator() noexcept = default;

    template<typename U, typename... Args>
      requires(!std::same_as<convert_iterator, std::remove_cvref_t<U>>)
    constexpr convert_iterator(U&& ref, Args&&... args) noexcept
        : iterator(std::forward<U>(ref), std::forward<Args>(args)...)
    {
    }

    /// @brief Reads the current value of the range
    /// @return The current value of the range
    constexpr To operator*() const noexcept { return static_cast<To>(*iterator); }

    /// @brief Increments the iterator to the next value in the range
    /// @return Self
    constexpr convert_iterator& operator++() noexcept
    {
      ++iterator;
      return *this;
    }

    /// @brief Increments the iterator to the next value in the range, returning the old value
    /// @return Copy of old iterator
    constexpr convert_iterator operator++(int) noexcept
    {
      convert_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    /// @brief Decrements the iterator to the next value in the range
    /// @return Self
    constexpr convert_iterator& operator--() noexcept
    {
      --iterator;
      return *this;
    }

    /// @brief Decrements the iterator to the next value in the range, returning the old value
    /// @return Copy of old iterator
    constexpr convert_iterator operator--(int) noexcept
    {
      convert_iterator tmp = *this;
      --(*this);
      return tmp;
    }

    friend constexpr bool operator==(
        const convert_iterator&, const convert_iterator&) = default;
  };

  template<typename Enum, u64 BEGIN, u64 END>
  /// @brief Iterator over an contiguous enum range
  struct enum_iterator
  {
    /// @brief Returns the start of the enum range
    /// @return Start of the enum range
    constexpr convert_iterator<Enum, range_iterator<BEGIN, END>> begin()
        const noexcept
    {
      return {};
    }
    /// @brief Returns the end of the enum range
    /// @return End of the enum range
    constexpr convert_iterator<Enum, range_iterator<BEGIN, END>> end() const noexcept
    {
      return END + 1;
    }
  };
} // namespace clt

#endif // !HG_DSA_ITERATOR
