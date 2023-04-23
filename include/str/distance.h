/** @file distance.h
* Contains helpers to compute distances between strings of text.
*/

#ifndef HG_COLT_DISTANCE
#define HG_COLT_DISTANCE

#include <type_traits>
#include <cstring>
#include "../structs/string_view.h"
#include "../structs/vector.h"

namespace clt::str
{
  /// @brief Computes the Levenshtein distance of two StringView
  /// @param source The first StringView
  /// @param target The second StringView
  /// @return Required transformation count from source to target
  constexpr u64 levenshtein_distance(StringView source, StringView target) noexcept
  {
    if (source.size() > target.size()) {
      return levenshtein_distance(target, source);
    }

    const u64 min_size = source.size();
    const u64 max_size = target.size();
    mem::FallbackAllocator<mem::StackAllocator<1024>, mem::Mallocator> ALLOCATOR;
    auto lev_dist = clt::make_local_vector<u64>(ALLOCATOR, min_size + 1, InPlace);

    for (u64 i = 0; i <= min_size; ++i)
      lev_dist[i] = i;

    for (u64 j = 1; j <= max_size; ++j) {
      u64 previous_diagonal = lev_dist[0];
      u64 previous_diagonal_save;
      ++lev_dist[0];

      for (u64 i = 1; i <= min_size; ++i) {
        previous_diagonal_save = lev_dist[i];

        if (source[i - 1] == target[j - 1])
          lev_dist[i] = previous_diagonal;
        else
          lev_dist[i] = clt::min({ lev_dist[i - 1], lev_dist[i], previous_diagonal }) + 1;

        previous_diagonal = previous_diagonal_save;
      }
    }

    return lev_dist[min_size];
  }
}

#endif //!HG_COLT_DISTANCE