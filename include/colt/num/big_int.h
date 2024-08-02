/*****************************************************************/ /**
 * @file   big_int.h
 * @brief  Contains BigInt and BigRational, wrappers around MPIR.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_NUM_BIG_INT
#define HG_NUM_BIG_INT

#include <concepts>
#include <utility>

#include "fmt/format.h"
#include "colt/num/typedefs.h"
#include "colt/macro/assert.h"
#include "colt/macro/on_scope_exit.h"
#include "colt/mem/composable_alloc.h"
#include "gmp.h"

namespace clt::num
{
  /// @brief Represent a signed integer of arbitrary precision.
  /// All BigInt allocate memory, thus avoid temporaries, and
  /// always make use of '[+-*/]=' whenever possible.
  class BigInt
  {
  public:
    mpz_t storage{};
    /// @brief Constructor, sets the value to 0
    BigInt() noexcept { mpz_init(storage); }
    /// @brief Constructor, sets the value to 'value'
    /// @param value The initial value
    explicit BigInt(i32 value) noexcept { mpz_init_set_si(storage, value); }
    /// @brief Constructor, sets the value to 'value'
    /// @param value The initial value
    explicit BigInt(u32 value) noexcept { mpz_init_set_ui(storage, value); }
    /// @brief Constructor, sets the value to 'value'
    /// @param value The initial value
    explicit BigInt(double value) noexcept { mpz_init_set_d(storage, value); }
    /// @brief Copy constructor, sets the value to 'value'
    /// @param copy The initial value
    BigInt(const BigInt& copy) noexcept { mpz_init_set(storage, copy.storage); }
    /// @brief Copy assignment operator
    /// @param copy The value to copy
    /// @return Self
    BigInt& operator=(const BigInt& copy) noexcept
    {
      assert_true("Self assignment is prohibited!", this != &copy);
      mpz_set(storage, copy.storage);
      return *this;
    }
    /// @brief Move constructor
    /// @param move The initial value
    BigInt(BigInt&& move) noexcept
    {
      mpz_init(storage);
      mpz_swap(storage, move.storage);
    }
    /// @brief Move assignment operator
    /// @param move The value to move
    /// @return Self
    BigInt& operator=(BigInt&& move) noexcept
    {
      assert_true("Self assignment is prohibited!", this != &move);
      mpz_swap(storage, move.storage);
      return *this;
    }

    BigInt& operator+=(const BigInt& rhs)
    {
      mpz_add(storage, storage, rhs.storage);
      return *this;
    }

    friend BigInt operator+(const BigInt& lhs, const BigInt& rhs)
    {
      BigInt result = lhs;
      result += rhs;
      return result;
    }

    BigInt& operator-=(const BigInt& rhs)
    {
      mpz_sub(storage, storage, rhs.storage);
      return *this;
    }

    friend BigInt operator-(const BigInt& lhs, const BigInt& rhs)
    {
      BigInt result = lhs;
      result -= rhs;
      return result;
    }

    BigInt& operator*=(const BigInt& rhs)
    {
      mpz_mul(storage, storage, rhs.storage);
      return *this;
    }

    friend BigInt operator*(const BigInt& lhs, const BigInt& rhs)
    {
      BigInt result = lhs;
      result *= rhs;
      return result;
    }

    BigInt& operator/=(const BigInt& rhs)
    {
      mpz_div(storage, storage, rhs.storage);
      return *this;
    }

    friend BigInt operator/(const BigInt& lhs, const BigInt& rhs)
    {
      BigInt result = lhs;
      result /= rhs;
      return result;
    }

    /// @brief Returns the number of characters needed to represent
    ///        the integer in a specific base.
    /// @param base The base used to represent the string
    /// @return The number of characters (without counting a NUL-terminator)
    size_t str_size(int base = 10) const noexcept
    {
      //TODO: add check for value
      const bool is_neg = mpz_sgn(storage) == -1;
      return mpz_sizeinbase(storage, base) + (size_t)is_neg;
    }

    void internal_storage(mpz_t out) const noexcept { out[0] = *storage; }

    /// @brief Destructor, frees any resource used
    ~BigInt() noexcept { mpz_clear(storage); }
  };
} // namespace clt::num

template<>
/// @brief {fmt} specialization of Color
struct fmt::formatter<clt::num::BigInt>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    // TODO: add format option
    return ctx.begin();
  }

  template<typename FormatContext>
  /// @brief fmt overload
  /// @tparam FormatContext The context to write
  /// @param op The BinaryOperator to write
  /// @param ctx The context
  /// @return context
  auto format(const clt::num::BigInt& op, FormatContext& ctx) const
  {
    using namespace clt::mem;
    FallbackAllocator<StackAllocator<2048, 1>, Mallocator> allocator;
    auto blk = allocator.alloc(op.str_size());

    // Dealloc used memory
    ON_SCOPE_EXIT
    {
      allocator.dealloc(blk);
    };

    mpz_t value;
    op.internal_storage(value);
    auto ptr = static_cast<char*>(blk.ptr());
    mpz_get_str(ptr, 10, value);

    return fmt::format_to(ctx.out(), "{}", ptr);
  }
};

#endif // !HG_NUM_BIG_INT
