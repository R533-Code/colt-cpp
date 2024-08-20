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

#include "colt/macro/assert.h"

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
} // namespace clt

#endif // !HG_DSA_ITERATOR
