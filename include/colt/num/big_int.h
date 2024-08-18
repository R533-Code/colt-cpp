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
#include <compare>

#include "fmt/format.h"
#include "colt/dsa/option.h"
#include "colt/typedefs.h"
#include "colt/macro/assert.h"
#include "colt/macro/on_scope_exit.h"
#include "colt/mem/composable_alloc.h"
#include "gmp.h"

#define COLT_MAKE_OPERATOR(op, mpz_fn, type, arg_expr)            \
  BigInt& operator COLT_CONCAT(op, =)(type rhs) noexcept          \
  {                                                               \
    mpz_fn(storage, storage, arg_expr);                           \
    return *this;                                                 \
  }                                                               \
  friend BigInt operator op(const BigInt& lhs, type rhs) noexcept \
  {                                                               \
    BigInt result = lhs;                                          \
    result COLT_CONCAT(op, =) rhs;                                \
    return result;                                                \
  }

#define COLT_MAKE_ASSIGNMENT(mpz_fn, type) \
  BigInt& operator=(type rhs) noexcept     \
  {                                        \
    mpz_fn(storage, rhs);                  \
    return *this;                          \
  }

#define COLT_MAKE_OVERLOAD_OPERATOR_U32_BIGINT(op, mpz_fn)   \
  COLT_MAKE_OPERATOR(op, COLT_CONCAT(mpz_fn, _ui), u32, rhs) \
  COLT_MAKE_OPERATOR(op, mpz_fn, const BigInt&, rhs.storage)

#define COLT_MAKE_OVERLOAD_OPERATOR_U32_I32_BIGINT(op, mpz_fn) \
  COLT_MAKE_OPERATOR(op, COLT_CONCAT(mpz_fn, _ui), u32, rhs)   \
  COLT_MAKE_OPERATOR(op, COLT_CONCAT(mpz_fn, _si), i32, rhs)   \
  COLT_MAKE_OPERATOR(op, mpz_fn, const BigInt&, rhs.storage)

#define COLT_MAKE_OVERLOAD_ASSIGNMENT(mpz_fn)         \
  COLT_MAKE_ASSIGNMENT(COLT_CONCAT(mpz_fn, _ui), u32) \
  COLT_MAKE_ASSIGNMENT(COLT_CONCAT(mpz_fn, _si), i32) \
  COLT_MAKE_ASSIGNMENT(COLT_CONCAT(mpz_fn, _d), f64)

namespace clt::num
{
  /// @brief Represent a signed integer of arbitrary precision.
  /// All BigInt allocate memory, thus avoid temporaries, and
  /// always make use of '[+-*/]=' whenever possible.
  class BigInt
  {
    mpz_t storage{};

  public:
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

    COLT_MAKE_OVERLOAD_OPERATOR_U32_BIGINT(+, mpz_add);
    template<typename T>
    BigInt& add(const T& value) noexcept
    {
      return *this += value;
    }

    COLT_MAKE_OVERLOAD_OPERATOR_U32_BIGINT(-, mpz_sub);
    template<typename T>
    BigInt& sub(const T& value) noexcept
    {
      return *this -= value;
    }

    COLT_MAKE_OVERLOAD_OPERATOR_U32_I32_BIGINT(*, mpz_mul);
    template<typename T>
    BigInt& mul(const T& value) noexcept
    {
      return *this *= value;
    }

    COLT_MAKE_OVERLOAD_OPERATOR_U32_BIGINT(/, mpz_div);
    template<typename T>
    BigInt& div(const T& value) noexcept
    {
      return *this /= value;
    }

    COLT_MAKE_OVERLOAD_ASSIGNMENT(mpz_set);

    COLT_MAKE_OPERATOR(&, mpz_and, const BigInt&, rhs.storage);
    COLT_MAKE_OPERATOR(|, mpz_ior, const BigInt&, rhs.storage);
    COLT_MAKE_OPERATOR(^, mpz_xor, const BigInt&, rhs.storage);

    BigInt& operator++() noexcept
    {
      *this += 1U;
      return *this;
    }

    BigInt operator++(int) noexcept
    {
      BigInt tmp = *this;
      operator++();
      return tmp;
    }

    BigInt& operator--() noexcept
    {
      *this -= 1U;
      return *this;
    }

    BigInt operator--(int) noexcept
    {
      BigInt tmp = *this;
      operator++();
      return tmp;
    }

    /// @brief Returns the sign of the number.
    /// Return 0 for 0, +1 for positive numbers, and -1 for negative numbers.
    /// @return The sign of the number
    int sgn() const noexcept { return mpz_sgn(storage); }

    /// @brief Negates the current number
    /// @return Self
    BigInt& neg() noexcept
    {
      mpz_neg(storage, storage);
      return *this;
    }

    /// @brief Returns a negated copy of the current number
    /// @return Negated copy of the current number
    BigInt operator-() const noexcept
    {
      BigInt copy = *this;
      copy.neg();
      return copy;
    }

    std::strong_ordering operator<=>(const BigInt& b) const noexcept
    {
      auto cmp = mpz_cmp(storage, b.storage);
      if (cmp < 0)
        return std::strong_ordering::less;
      else if (cmp == 0)
        return std::strong_ordering::equivalent;
      else
        return std::strong_ordering::greater;
    }

    std::strong_ordering operator<=>(double b) const noexcept
    {
      auto cmp = mpz_cmp_d(storage, b);
      if (cmp < 0)
        return std::strong_ordering::less;
      else if (cmp == 0)
        return std::strong_ordering::equivalent;
      else
        return std::strong_ordering::greater;
    }

    std::strong_ordering operator<=>(u32 b) const noexcept
    {
      auto cmp = mpz_cmp_ui(storage, b);
      if (cmp < 0)
        return std::strong_ordering::less;
      else if (cmp == 0)
        return std::strong_ordering::equivalent;
      else
        return std::strong_ordering::greater;
    }

    std::strong_ordering operator<=>(i32 b) const noexcept
    {
      auto cmp = mpz_cmp_si(storage, b);
      if (cmp < 0)
        return std::strong_ordering::less;
      else if (cmp == 0)
        return std::strong_ordering::equivalent;
      else
        return std::strong_ordering::greater;
    }

    /// @brief Returns the number of characters needed to represent
    ///        the integer in a specific base.
    /// @param base The base used to represent the string
    /// @return The number of characters (without counting a NUL-terminator)
    size_t str_size(int base = 10) const noexcept
    {
      //TODO: add check for base
      const bool is_neg = mpz_sgn(storage) == -1;
      return mpz_sizeinbase(storage, base) + (size_t)is_neg;
    }

    /// @brief Returns the internal mpz_t.
    /// @warning Never call mpz_clear on the result
    /// @param out The out parameter to which to write the storage
    void internal_storage(mpz_t out) const noexcept { out[0] = *storage; }

    /// @brief Swaps two BigInt
    /// @param with The BigInt to swap with
    void swap(BigInt& with) noexcept { mpz_swap(storage, with.storage); }

    /// @brief Destructor, frees any resource used
    ~BigInt() noexcept { mpz_clear(storage); }
  };
} // namespace clt::num

#undef COLT_MAKE_OPERATOR
#undef COLT_MAKE_ASSIGNMENT
#undef COLT_MAKE_OVERLOAD_OPERATOR_U32_BIGINT
#undef COLT_MAKE_OVERLOAD_OPERATOR_U32_I32_BIGINT
#undef COLT_MAKE_OVERLOAD_ASSIGNMENT

template<>
/// @brief {fmt} specialization of BigInt
struct fmt::formatter<clt::num::BigInt>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    // TODO: add format option
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const clt::num::BigInt& op, FormatContext& ctx) const
  {
    using namespace clt::mem;
    // Use stack spaces if possible else use malloc
    FallbackAllocator<StackAllocator<2048, 1>, Mallocator> allocator;
    // + 1 for NUL terminator
    auto blk = allocator.alloc(op.str_size() + 1);

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
