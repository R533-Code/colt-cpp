/*****************************************************************/ /**
 * @file   trie.h
 * @brief  Contains TrieSet and TrieMap (using tsl).
 * 
 * @author RPC
 * @date   September 2024
 *********************************************************************/
#ifndef HG_COLT_TRIE
#define HG_COLT_TRIE

#include <tsl/htrie_map.h>
#include <tsl/htrie_set.h>
#include <colt/hash.h>

namespace clt
{
  /// @brief TrieSet
  /// @tparam T The value to store
  /// @tparam HASH The hasher object
  template<typename T, typename HASH = clt::uhash<clt::murmur64a_h>>
  class TrieSet : public tsl::htrie_set<T, HASH, u32>
  {
  };

  /// @brief TrieMap
  /// @tparam Key The key type
  /// @tparam Value The value type
  /// @tparam HASH The hasher object
  template<
      typename Key, typename Value, typename HASH = clt::uhash<clt::murmur64a_h>>
  class TrieMap : public tsl::htrie_map<Key, Value, HASH, u32>
  {
  };
} // namespace clt

#endif // !HG_COLT_TRIE
