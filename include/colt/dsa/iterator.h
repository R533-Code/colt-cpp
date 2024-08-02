#ifndef HG_DSA_ITERATOR
#define HG_DSA_ITERATOR

#include <concepts>
#include <iterator>

#include "colt/macro/assert.h"

namespace clt
{
  template<std::input_iterator Iter, auto TRANSFORM>
    requires std::invocable<decltype(TRANSFORM), std::iter_value_t<Iter>>
  class transform_iter
  {
    Iter iterator;
  
  public:
    constexpr transform_iter()
      requires std::default_initializable<Iter>
        : iterator({})
      {}
    
    template<typename Arg, typename... Args>
      requires(!std::same_as<std::remove_cvref_t<Arg>, transform_iter>)
    constexpr transform_iter(Arg&& arg, Args&&... args)
        : iterator(std::forward<Arg>(arg), std::forward<Args>(args)...)
    {
    }

    MAKE_DEFAULT_COPY_AND_MOVE_FOR(transform_iter);
    

    constexpr transform_iter& operator++()
    {
      ++iterator;
      return *this;
    }

    constexpr transform_iter operator++(int)
    {
      auto copy = *this;
      ++(*this);
      return copy;
    }

    constexpr transform_iter& operator--()
    {
      --iterator;
      return *this;
    }

    constexpr transform_iter operator--(int)
    {
      auto copy = *this;
      --(*this);
      return copy;
    }
    
    constexpr auto operator*()
    {
      return std::invoke(TRANSFORM, *iterator);
    }

    friend constexpr bool operator==(
        const transform_iter&, const transform_iter&) noexcept = default;
  };
}

#endif // !HG_DSA_ITERATOR
