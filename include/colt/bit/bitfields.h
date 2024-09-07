/*****************************************************************/ /**
 * @file   bitfields.h
 * @brief  Contains portable bitfield helpers.
 * The bitfield has zero-overhead and compiles to the same code
 * as using a normal bitfield.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_BIT_BITFIELDS
#define HG_BIT_BITFIELDS

#include <concepts>

#include "colt/bit/operations.h"
#include "colt/dsa/common.h"
#include "colt/meta/traits.h"
#include "colt/typedefs.h"
#include "colt/hash.h"
#include "colt/macro/assert.h"

namespace clt::bit
{
  /// @brief Represents a field of 'Bitfields'
  /// @tparam Name The integral type used to identify the bit field
  /// @tparam Size The size of the field
  template<auto Name, size_t Size>
    requires(0 < Size) && (Size <= 64)
  struct Bitfield
  {
    /// @brief Variable to verify that a type is a bit field
    static constexpr bool is_bit_field = true;

    /// @brief The type used to identify the bit field
    using type = decltype(Name);
    /// @brief The value used to identify the bit field
    static constexpr type value = Name;
    /// @brief The size of the bitfield
    static constexpr size_t size = Size;
  };

  /// @brief Portable bit field implementation.
  /// Due to the standard underspecifying bit fields, this class is needed
  /// to make bit fields portable. Only endianess is not taken into account.
  /// The field is stored as left-to-right (with the first BitField being
  /// the leftmost).
  /// @code{.cpp}
  /// // Example usage:
  /// enum class FieldName
  /// {
  ///   OpCode, Payload, Padding
  /// };
  /// using Type = Bitfields<u16,
  ///   Bitfield<FieldName::OpCode, 4>, // ID, bit size
  ///   Bitfield<FieldName::Payload, 8>,
  ///   Bitfield<FieldName::Padding, 4>>;
  /// @endcode
  ///
  /// @tparam Field0 The first field
  /// @tparam ...Fields The other fields
  /// @tparam Ty The underlying data in which to store the value
  template<std::unsigned_integral Ty, typename Field0, typename... Fields>
  class Bitfields
  {
    static_assert(
        (Field0::is_bit_field && ... && Fields::is_bit_field),
        "All the underlying type used to identify the field must be the same");
    static_assert(
        (Field0::size + ... + Fields::size) == sizeof(Ty) * 8,
        "The sum of all the bitfields' size must fill all the bits of the "
        "underlying type!");
    static_assert(
        meta::are_all_same<typename Field0::type, typename Fields::type...>,
        "All the underlying type used to identify the field must be the same");

    /// @brief The type of the field ID
    using index_t = typename Field0::type;
    /// @brief The number of fields
    static constexpr size_t field_count = 1 + sizeof...(Fields);

    /// @brief The underlying storage
    Ty storage = {0};

    COLT_DISABLE_WARNING_PUSH
    COLT_DISABLE_WARNING("-Wreturn-type", 4716)

    /// @brief Returns the informations about the field of name 'index'
    /// @tparam index The field name
    /// @return Pair containing the offset to the field, and size of the field
    template<auto index>
      requires std::same_as<index_t, decltype(index)>
    static consteval std::pair<u64, u64> field_info() noexcept
    {
      std::array<std::pair<index_t, size_t>, field_count> array = {
          std::pair{Field0::value, Field0::size},
          std::pair{Fields::value, Fields::size}...};
      u64 offset = sizeof(Ty) * 8;
      for (size_t i = 0; i < field_count; i++)
      {
        offset -= array[i].second;
        if (array[i].first == index)
          return {offset, array[i].second};
      }
      assert_true("Invalid field name!", false);
    }

    /// @brief Returns the field information of field 'index'
    /// @param index The index
    /// @return Pair containing the offset to the field, and size of the field
    static consteval std::pair<u64, u64> field_info(u64 index) noexcept
    {
      std::array<std::pair<index_t, size_t>, field_count> array = {
          std::pair{Field0::value, Field0::size},
          std::pair{Fields::value, Fields::size}...};
      u64 offset = sizeof(Ty) * 8;
      for (size_t i = 0; i < field_count; i++)
      {
        offset -= array[i].second;
        if (i == index)
          return {offset, array[i].second};
      }
      assert_true("Invalid field index!", false);
    }

    COLT_DISABLE_WARNING_POP

    /// @brief Set the field of index 'index'
    /// @tparam index The field index (starting at 0)
    /// @param value The value to assign for that field
    template<u64 index>
    void set_field(Ty value) noexcept
    {
      const auto info = field_info(index);
      // Set the bits whose value to modify to 0
      storage &= ~(bitmask<Ty>((Ty)info.second) << info.first);
      // OR the new bits
      storage |= ((value & bitmask<Ty>((Ty)info.second)) << info.first);
    }

    /// @brief Initializes the rest of the fields
    /// @tparam ...Is The integer sequence
    /// @tparam ...Ints The value's types
    /// @param ...value The value to use to initialize the fields
    template<size_t... Is, std::convertible_to<Ty>... Ints>
      requires(sizeof...(Is) == sizeof...(Ints))
    void set_fields(std::index_sequence<Is...>, Ints... value) noexcept
    {
      (set_field<Is + 1>(value), ...);
    }

  public:
    /// @brief Constructs an empty Bitfields (set to all zeros)
    constexpr Bitfields() noexcept
        : storage(0)
    {
    }
    /// @brief Constructs a Bitfields
    /// @param value The value to initialize to
    constexpr Bitfields(Ty value) noexcept
        : storage(value)
    {
    }

    /// @brief Constuctor
    /// @tparam ...Ints The integral types
    /// @param field0 The first field's value
    /// @param ...fields The pack of values for the rest of the fields
    template<std::convertible_to<Ty>... Ints>
      requires(sizeof...(Ints) == sizeof...(Fields))
    constexpr Bitfields(in_place_t, Ty field0, Ints... fields) noexcept
        : storage(0)
    {
      set_field<0>(field0);
      set_fields(std::make_index_sequence<sizeof...(Ints)>{}, fields...);
    }

    MAKE_DEFAULT_COPY_AND_MOVE_FOR(Bitfields);

    /// @brief Returns the bit field of ID 'index'.
    /// If this method produces an 'expression cannot be constant evaluated'
    /// or such, this means the ID is invalid.
    /// @tparam index The ID of the field
    /// @return The value stored in the bit field
    template<auto index>
      requires std::same_as<index_t, decltype(index)>
    constexpr Ty get() const noexcept
    {
      const auto pair = field_info<index>();
      return (storage >> pair.first) & bitmask<Ty>(pair.second);
    }

    /// @brief Sets the value of the bit field of ID 'index'
    /// This will only keep as much bits from 'value' as the field can store.
    /// If this method produces an 'expression cannot be constant evaluated'
    /// or such, this means the ID is invalid.
    /// @tparam index The ID of the field
    /// @param value The value to store
    template<auto index>
      requires std::same_as<index_t, decltype(index)>
    constexpr void set(Ty value) noexcept
    {
      const auto info = field_info<index>();
      // Set the bits whose value to modify to 0
      storage &= ~(bitmask<Ty>((Ty)info.second) << info.first);
      // OR the new bits
      storage |= ((value & bitmask<Ty>((Ty)info.second)) << info.first);
    }

    /// @brief Returns the underlying value
    /// @return The underlying value
    constexpr Ty value() const noexcept { return storage; }

    template<typename Ser>
    static constexpr auto serialize(Ser& archive, Bitfields& self) noexcept
    {
      using namespace zpp::bits;
      return archive(self.storage);
    }
  };
} // namespace clt::bit

namespace clt::meta
{
  template<std::unsigned_integral Ty, typename Field0, typename... Fields>
  struct is_contiguously_hashable<clt::bit::Bitfields<Ty, Field0, Fields...>>
      : public std::true_type
  {
  };
} // namespace clt::meta

#endif // !HG_BIT_BITFIELDS
