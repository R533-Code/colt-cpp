/*****************************************************************/ /**
 * @file   hash.h
 * @brief  Contains all hashing related utilities.
 * 
 * This file implements helpers as described in:
 * https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3980.html
 * Credits to Howard E. Hinnant, Vinnie Falco, John Bytheway.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_COLT_HASH
#define HG_COLT_HASH

#include <bit>
#include <random>
#include "typedefs.h"

namespace clt
{
  /// @brief Hash Algorithm that makes use of FNV1a
  class fnv1a_h
  {
    /// @brief Starting state
    size_t state_ = 14695981039346656037u;

  public:
    /// @brief The result type of hashing
    using result_type = size_t;

    /// @brief Hashes bytes
    /// @param key The key to hash
    /// @param len The length in bytes
    constexpr void operator()(const void* key, size_t len) noexcept
    {
      auto p = static_cast<const u8*>(key);
      auto e = p + len;
      for (; p < e; ++p)
        state_ = (state_ ^ *p) * 1099511628211u;
    }

    /// @brief Returns the result of hashing
    explicit operator result_type() const noexcept { return state_; }
  };

  /// @brief Hash Algorithm that makes use of MurmurHash64a
  class murmur64a_h
  {
    /// @brief The state
    u64 h = 0;
    /// @brief The seed
    u64 seed;

    HEDLEY_ALWAYS_INLINE
    /// @brief Unaligned read and or
    /// @param ptr The pointer from which to read
    /// @param read_count The number of bytes to read [0-7]
    /// @param b The value to which to or the result
    static constexpr void unaligned_xor(
        const u8* ptr, size_t read_count, u64& b) noexcept
    {
      switch (read_count)
      {
      case 7:
        b ^= ((u64)ptr[6]) << 48;
        [[fallthrough]];
      case 6:
        b ^= ((u64)ptr[5]) << 40;
        [[fallthrough]];
      case 5:
        b ^= ((u64)ptr[4]) << 32;
        [[fallthrough]];
      case 4:
        b ^= ((u64)ptr[3]) << 24;
        [[fallthrough]];
      case 3:
        b ^= ((u64)ptr[2]) << 16;
        [[fallthrough]];
      case 2:
        b ^= ((u64)ptr[1]) << 8;
        [[fallthrough]];
      case 1:
        b ^= ((u64)ptr[0]);
        b *= m;
      }
    }

  public:
    /// @brief The result type of hashing
    using result_type = size_t;

    /// @brief Constant m
    static constexpr u64 m = 0xc6a4a7935bd1e995ULL;
    /// @brief Constant r
    static constexpr u64 r = 47;

    /// @brief Constructor
    /// @param seed The starting seed
    constexpr murmur64a_h(u64 seed = 14695981039346656037ULL) noexcept
        : seed(seed)
    {
    }

    /// @brief Hashes bytes
    /// @param key The key to hash
    /// @param len The length in bytes
    void operator()(const void* key, size_t len) noexcept
    {
      h               = seed ^ (len * m);
      auto ptr        = (const u8*)key;
      const u64 left  = uintptr_t(ptr) % 8;
      const u64 right = (len - left) % 8;
      size_t i        = 0;

      unaligned_xor(ptr, left, h);
      i += left;

      h ^= h >> r;
      h *= m;
      h ^= h >> r;

      for (; i < len; i += sizeof(u64))
      {
        uint64_t k = *reinterpret_cast<const u64*>(ptr + i);
        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
      }

      unaligned_xor(ptr + i, right, h);

      h ^= h >> r;
      h *= m;
      h ^= h >> r;
    }

    /// @brief Returns the result of hashing
    explicit operator result_type() const noexcept { return h; }
  };

  /// @brief Hash Algorithm that makes use of SipHash-2-4
  class siphash24_h
  {
    /// @brief State v0
    u64 v0 = 0x736f6d6570736575;
    /// @brief State v1
    u64 v1 = 0x646f72616e646f6d;
    /// @brief State v2
    u64 v2 = 0x6c7967656e657261;
    /// @brief State v3
    u64 v3 = 0x7465646279746573;

    HEDLEY_ALWAYS_INLINE
    /// @brief Applies a round of rotations to the state
    /// @param state The state on which to apply the round
    static constexpr void apply_round(siphash24_h& state) noexcept
    {
      state.v0 += state.v1;
      state.v1 = std::rotl(state.v1, 13);
      state.v1 ^= state.v0;
      state.v0 = std::rotl(state.v0, 32);

      state.v2 += state.v3;
      state.v3 = std::rotl(state.v3, 16);
      state.v3 ^= state.v2;

      state.v0 += state.v3;
      state.v3 = std::rotl(state.v3, 21);
      state.v3 ^= state.v0;
      state.v0 = std::rotl(state.v0, 17);

      state.v2 += state.v1;
      state.v1 = std::rotl(state.v1, 17);
      state.v1 ^= state.v2;
      state.v2 = std::rotl(state.v2, 32);
    }

    HEDLEY_ALWAYS_INLINE
    /// @brief Unaligned read and or
    /// @param ptr The pointer from which to read
    /// @param read_count The number of bytes to read [0-7]
    /// @param b The value to which to or the result
    static constexpr void unaligned_or(
        const u8* ptr, size_t read_count, u64& b) noexcept
    {
      switch (read_count)
      {
      case 7:
        b |= ((u64)ptr[6]) << 48;
        [[fallthrough]];
      case 6:
        b |= ((u64)ptr[5]) << 40;
        [[fallthrough]];
      case 5:
        b |= ((u64)ptr[4]) << 32;
        [[fallthrough]];
      case 4:
        b |= ((u64)ptr[3]) << 24;
        [[fallthrough]];
      case 3:
        b |= ((u64)ptr[2]) << 16;
        [[fallthrough]];
      case 2:
        b |= ((u64)ptr[1]) << 8;
        [[fallthrough]];
      case 1:
        b |= ((u64)ptr[0]);
        break;
      case 0:
        break;
      }
    }

  public:
    /// @brief The result type of hashing
    using result_type = size_t;

    /// @brief The default key used by SipHash-2-4
    static constexpr std::array<u8, 16> DEFAULT_KEY = {
        172, 151, 141, 193, 144, 166, 78, 27, 255, 223, 59, 66, 231, 234, 20, 152};

    /// @brief The numbers of rounds performed for each 8 byte read
    static constexpr u64 cROUNDS = 2;
    /// @brief The number of final rounds performed after all the reads
    static constexpr u64 dROUNDS = 4;

    /// @brief Constructor
    /// @param key The key used to initialize the state of the hasher
    constexpr siphash24_h(const std::array<u8, 16>& key = DEFAULT_KEY) noexcept
    {
      v0 ^= u64(key[0]) << 56 | u64(key[1]) << 48 | u64(key[2]) << 40
            | u64(key[3]) << 32 | u64(key[4]) << 24 | u64(key[5]) << 16
            | u64(key[6]) << 8 | u64(key[7]);
      v1 ^= u64(key[8]) << 56 | u64(key[9]) << 48 | u64(key[10]) << 40
            | u64(key[11]) << 32 | u64(key[12]) << 24 | u64(key[13]) << 16
            | u64(key[14]) << 8 | u64(key[15]);
    }

    /// @brief Hashes bytes
    /// @param key The key to hash
    /// @param len The length in bytes of the key
    void operator()(const void* key, size_t len) noexcept
    {
      auto ptr = static_cast<const u8*>(key);

      const u64 left  = uintptr_t(ptr) % 8;
      const u64 right = (len - left) % 8;
      size_t i        = 0;
      auto b          = len << 56;

      unaligned_or(ptr, left, b);
      v3 ^= b;
      for (u64 r = 0; r < dROUNDS; ++r)
        apply_round(*this);
      v0 ^= b;
      i += left;

      for (; i < len; i += sizeof(u64))
      {
        u64 m = *reinterpret_cast<const u64*>(ptr + i);
        v3 ^= m;
        for (u64 r = 0; r < cROUNDS; ++r)
          apply_round(*this);
        v0 ^= m;
      }

      unaligned_or(ptr + i, right, b);
      v3 ^= b;
      for (u64 r = 0; r < dROUNDS; ++r)
        apply_round(*this);
      v0 ^= b;
      v2 ^= 0xff;
    }

    /// @brief Returns the result of hashing
    explicit operator result_type() const noexcept { return v0 ^ v1 ^ v2 ^ v3; }
  };

  namespace meta
  {
    /// @brief Check if a type is a hashing algorithm
    template<typename T>
    concept hash_algorithm =
        requires(T::result_type a, T hasher, const void* key, size_t len) {
          {
            static_cast<decltype(a)>(hasher)
          } -> std::same_as<typename T::result_type>;
          {
            hasher(key, len)
          } -> std::same_as<void>;
        };
  } // namespace meta

  /// @brief Hash Append function must be overloaded for each hashable type.
  /// The goal of this function is to expose which data to hash using Algo.
  /// @tparam Algo The hashing algorithm
  /// @tparam T The type to hash
  /// @param algo The hashing algorithm
  /// @param value The value to hash
  template<meta::hash_algorithm Algo, typename T>
  constexpr void hash_append(Algo& algo, const T& value) = delete;

  namespace meta
  {
    /// @brief If true, then the type can be hashed by only iterating
    /// over the contiguous bytes of the object.
    /// @tparam T The type
    template<typename T>
    struct is_contiguously_hashable : public std::false_type
    {
    };

    /// @brief All ints are contiguously hashable as C++ mandates two's complement
    /// signed integers.
    /// @tparam T The type
    template<std::integral T>
    struct is_contiguously_hashable<T> : public std::true_type
    {
    };

    /// @brief Shorthand for is_contiguously_hashable<T>::value
    /// @tparam T The type
    template<typename T>
    static constexpr bool is_contiguously_hashable_v =
        is_contiguously_hashable<T>::value;

    /// @brief Shorthand for is_contiguously_hashable<T>::value
    template<typename T>
    concept contiguously_hashable = is_contiguously_hashable_v<T>;

    /// @brief Check if a type is can be hashed
    template<typename T>
    concept hashable =
        contiguously_hashable<T> || requires(const T& value, fnv1a_h a) {
          {
            clt::hash_append(a, value)
          } -> std::same_as<void>;
        };
  } // namespace meta

  /// @brief Hash Append for types whose hash is computed only by iterating
  /// over contiguous bytes.
  /// @tparam T The contiguously_hashable type
  /// @tparam Algo The hashing algorithm
  /// @param h The hashing algorithm object
  /// @param v The value to hash
  template<meta::hash_algorithm Algo, meta::contiguously_hashable T>
  constexpr void hash_append(Algo& h, const T& v)
  {
    h(std::addressof(v), sizeof(v));
  }

  /// @brief Hash Append for floating point types.
  /// This overload is needed as 0.0 and -0.0 must give the same hash.
  /// @tparam T The floating point type
  /// @tparam Algo The hashing algorithm
  /// @param h The hashing algorithm object
  /// @param v The value to hash
  template<meta::hash_algorithm Algo, std::floating_point T>
  constexpr void hash_append(Algo& h, const T& v)
  {
    if (v == static_cast<T>(0))
      v = static_cast<T>(0);
    h(std::addressof(v), sizeof(v));
  }

  /// @brief Universal hasher.
  /// @tparam HashAlgorithm The hashing algorithm that must be used
  template<meta::hash_algorithm HashAlgorithm>
  struct uhash
  {
    /// @brief The result type of the universal hasher (which is the same as the
    /// algorithm result type)
    using result_type = typename HashAlgorithm::result_type;

    /// @brief Hashes a type
    /// @tparam T The type to hash
    /// @param t The value to hash
    /// @return The hash of the value
    template<meta::hashable T>
    constexpr result_type operator()(const T& t) const noexcept
    {
      HashAlgorithm h;
      hash_append(h, t);
      return static_cast<result_type>(h);
    }
  };
} // namespace clt

#endif // !HG_COLT_HASH
