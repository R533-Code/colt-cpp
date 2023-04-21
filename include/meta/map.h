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
      std::sort(data.begin(), data.end());
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
        u64 M = std::midpoint(L, R);
        if (data[M].first < key)
          L = M + 1;
        else if (data[M].first > key)
          R = M - 1;
        else
          return data[M].second;
      }
      return None;
    }

  };
}

#endif //!HG_COLT_MAP