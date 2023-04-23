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
      u64 L = 0;
      u64 R = Size - 1;
      while (L <= R)
      {
        u64 m = std::midpoint(L, R);
        if (data[m].first < key)
          L = m + 1;
        else if (data[m].first > key)
          R = m - 1;
        else
          return data[m].second;
      }
      return None;
    }

  };
}

#endif //!HG_COLT_MAP