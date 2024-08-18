/*****************************************************************/ /**
 * @file   hash.h
 * @brief  
 * 
 * This file implements helpers as described in:
 * https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3980.html#Universal
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
    using result_type = size_t;

    constexpr void operator()(const void* key, std::size_t len) noexcept
    {
      auto p = static_cast<const u8*>(key);
      auto e = p + len;
      for (; p < e; ++p)
        state_ = (state_ ^ *p) * 1099511628211u;
    }

    explicit operator result_type() const noexcept { return state_; }
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

    // Round function
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

  public:
    using result_type = size_t;

    static constexpr std::array<u8, 16> DEFAULT_KEY = {
        172, 151, 141, 193, 144, 166, 78, 27, 255, 223, 59, 66, 231, 234, 20, 152};

    static constexpr u64 cROUNDS = 2;

    static constexpr u64 dROUNDS = 4;

    constexpr siphash24_h(const std::array<u8, 16>& key = DEFAULT_KEY) noexcept
    {
      v0 ^= u64(key[0]) << 56 | u64(key[1]) << 48 | u64(key[2]) << 40
            | u64(key[3]) << 32 | u64(key[4]) << 24 | u64(key[5]) << 16
            | u64(key[6]) << 8 | u64(key[7]);
      v1 ^= u64(key[8]) << 56 | u64(key[9]) << 48 | u64(key[10]) << 40
            | u64(key[11]) << 32 | u64(key[12]) << 24 | u64(key[13]) << 16
            | u64(key[14]) << 8 | u64(key[15]);
    }

    constexpr void operator()(const void* key, size_t len) noexcept
    {
      auto ptr = static_cast<const u8*>(key);

      const int left = len & 7;
      size_t i       = 0;
      auto b         = (u64)len << 56;
      for (; i < len; i += 8)
      {
        u64 m = 0;
        std::memcpy(&m, ptr + i, sizeof(m));
        v3 ^= m;
        for (u64 r = 0; r < cROUNDS; ++r)
          apply_round(*this);
        v0 ^= m;
      }

      switch (left)
      {
      case 7:
        b |= ((u64)ptr[i + 6]) << 48;
      case 6:
        b |= ((u64)ptr[i + 5]) << 40;
      case 5:
        b |= ((u64)ptr[i + 4]) << 32;
      case 4:
        b |= ((u64)ptr[i + 3]) << 24;
      case 3:
        b |= ((u64)ptr[i + 2]) << 16;
      case 2:
        b |= ((u64)ptr[i + 1]) << 8;
      case 1:
        b |= ((u64)ptr[i + 0]);
        break;
      case 0:
        break;
      }

      v3 ^= b;
      for (u64 r = 0; r < dROUNDS; ++r)
        apply_round(*this);
      v0 ^= b;
      v2 ^= 0xff;
    }

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
    concept hashable = contiguously_hashable<T> ||
      requires(const T& value, fnv1a_h a)
    {
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
    using result_type = typename HashAlgorithm::result_type;

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
