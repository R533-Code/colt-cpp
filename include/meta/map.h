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

    [[nodiscard]]
    /// @brief Searches for the value associated with 'key'
    /// @param key The key whose value to find
    /// @return None if not found, else the value
    constexpr Option<Value> find(const Key& key) const
    {      
      if (const auto itr = std::find_if(begin(data), end(data), [&key](const auto& v) { return v.first == key; });
        itr != std::end(data))
        return itr->second;
      return None;
    }

  };
}

#endif //!HG_COLT_MAP