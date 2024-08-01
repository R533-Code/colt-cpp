#ifndef HG_META_MAP
#define HG_META_MAP

#include <algorithm>
#include <utility>
#include <numeric>

#include "colt/macro/assert.h"
#include "colt/dsa/option.h"

namespace clt::meta
{
  template<typename Key, typename Value, std::size_t Size>
  /// @brief constexpr Map for compile-time lookups
  /// @tparam Key The Key type
  /// @tparam Value The Value type
  struct Map
  {
    /// @brief The data in which to search
    std::array<std::pair<Key, Value>, Size> data;

    constexpr Map(std::array<std::pair<Key, Value>, Size> data)
        : data(data)
    {
      using pair_t = std::pair<Key, Value>;

      std::sort(
          this->data.begin(), this->data.end(),
          [](const pair_t& a, const pair_t& b) { return a.first < b.first; });

      assert_true(
          "Items not unique!",
          std::adjacent_find(data.begin(), data.end()) == data.end());
    }

    [[nodiscard]]
    /// @brief Searches for the value associated with 'key'
    /// @param key The key whose value to find
    /// @return None if not found, else the value
    constexpr Option<Value>
        find(const Key& key) const noexcept
    {
      if constexpr (Size == 0)
        return None;

      u64 low  = 0;
      u64 high = Size - 1;

      // Binary search
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

  template<typename Key, typename Value, std::size_t Size>
  /// @brief constexpr Map for compile-time lookups
  /// @tparam Key The Key type
  /// @tparam Value The Value type
  struct BiMap
  {
    /// @brief The data in which to search
    std::array<std::pair<Key, Value>, Size> data_f;
    /// @brief The data in which to search
    std::array<std::pair<Key, Value>, Size> data_b;

    constexpr BiMap(std::array<std::pair<Key, Value>, Size> data)
        : data_f(data)
        , data_b(data)
    {
      using pair_t = std::pair<Key, Value>;

      std::sort(
          this->data_f.begin(), this->data_f.end(),
          [](const pair_t& a, const pair_t& b) { return a.first < b.first; });
      std::sort(
          this->data_b.begin(), this->data_b.end(),
          [](const pair_t& a, const pair_t& b) { return a.second < b.second; });

      assert_true(
          "Items not unique!",
          std::adjacent_find(data_f.begin(), data_f.end(), 
            [](const pair_t& a, const pair_t& b) { return a.first == b.first; })
              == data_f.end());
      assert_true(
          "Items not unique!",
          std::adjacent_find(data_b.begin(), data_b.end(),
              [](const pair_t& a, const pair_t& b) { return a.second == b.second; })
              == data_b.end());
    }

    [[nodiscard]]
    /// @brief Searches for the value associated with 'key'
    /// @param key The key whose value to find
    /// @return None if not found, else the value
    constexpr Option<Value>
        find_value(const Key& key) const noexcept
    {
      if constexpr (Size == 0)
        return None;

      u64 low  = 0;
      u64 high = Size - 1;

      // Binary search
      while (low <= high)
      {
        u64 middle = std::midpoint(low, high);
        if (data_f[middle].first == key)
          return data[middle].second;
        if (data_f[middle].first > key)
        {
          if (middle == 0)
            return None;
          high = middle - 1;
        }
        else if (data_f[middle].first < key)
          low = middle + 1;
      }

      return None;
    }

    [[nodiscard]]
    /// @brief Searches for the value associated with 'value'
    /// @param value The value whose key to find
    /// @return None if not found, else the key
    constexpr Option<Key>
        find_key(const Value& value) const
    {
      if constexpr (Size == 0)
        return None;

      u64 low  = 0;
      u64 high = Size - 1;

      // Binary search
      while (low <= high)
      {
        u64 middle = std::midpoint(low, high);
        if (data_b[middle].second == value)
          return data[middle].first;
        if (data_b[middle].second > value)
        {
          if (middle == 0)
            return None;
          high = middle - 1;
        }
        else if (data_b[middle].second < value)
          low = middle + 1;
      }

      return None;
    }
  };
} // namespace clt::meta

#endif //!HG_META_MAP