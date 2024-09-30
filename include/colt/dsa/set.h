/*****************************************************************/ /**
 * @file   set.h
 * @brief  Contains Set (using tsl).
 * 
 * @author RPC
 * @date   September 2024
 *********************************************************************/
#ifndef HG_COLT_SET
#define HG_COLT_SET

#include <tsl/hopscotch_set.h>
#include <colt/hash.h>

namespace clt
{
  /// @brief Hash Set
  /// @tparam T The value to store
  /// @tparam HASH The hasher object
  template<typename T, typename HASH = clt::uhash<clt::murmur64a_h>>
  class Set : public tsl::hopscotch_set<T, HASH, u32>
  {
  };
} // namespace clt

#endif // !HG_COLT_SET
