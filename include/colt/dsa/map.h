/*****************************************************************/ /**
 * @file   map.h
 * @brief  Contains Map (using tsl).
 * 
 * @author RPC
 * @date   September 2024
 *********************************************************************/
#ifndef HG_COLT_MAP
#define HG_COLT_MAP

#include <tsl/hopscotch_map.h>
#include <colt/hash.h>

namespace clt
{
  /// @brief Hash Map
  /// @tparam Key The key type
  /// @tparam Value The value type
  /// @tparam HASH The hasher object
  template<typename Key, typename Value, typename HASH = clt::uhash<clt::murmur64a_h>>
  class Map : public tsl::hopscotch_map<Key, Value, HASH>
  {
  };
} // namespace clt

#endif // !HG_COLT_MAP
