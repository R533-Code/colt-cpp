/** @file map.h
* Contains ConstexprMap.
*/

#ifndef HG_COLT_MAP
#define HG_COLT_MAP

#include <algorithm>
#include <utility>

#include "../structs/option.h"

namespace clt::meta
{
  template <typename Key, typename Value, std::size_t Size>
  /// @brief constexpr Map for compile-time lookups
  /// @tparam Key The Key type
  /// @tparam Value The Value type
  struct ConstexprMap
  {
    /// @brief The data in which to search
    std::array<std::pair<Key, Value>, Size> data;

    constexpr ConstexprMap(std::array<std::pair<Key, Value>, Size> data)
      : data(data)
    {
      using pair_t = std::pair<Key, Value>;

      std::sort(this->data.begin(), this->data.end(),
        [](const pair_t& a, const pair_t& b) { return a.first < b.first; });

      assert_true("Items not unique!",
        std::adjacent_find(data.begin(), data.end()) == data.end());
    }

    [[nodiscard]]
    /// @brief Searches for the value associated with 'key'
    /// @param key The key whose value to find
    /// @return None if not found, else the value
    constexpr Option<Value> find(const Key& key) const
    {
      if constexpr (Size == 0)
        return None;

      u64 low = 0;
      u64 high = Size - 1;
      
      while (low <= high)
      {
        u64 middle = std::midpoint(low, high);
        if (data[middle].first == key)
          return data[middle].second;
        if (data[middle].first > key)
        {
          if (middle == 0)
            return None;
          high = middle - 1;
        }
        else if (data[middle].first < key)
          low = middle + 1;
      }

      return None;
    }
  };
}

#endif //!HG_COLT_MAP