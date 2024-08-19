/*****************************************************************/ /**
 * @file   big_int.h
 * @brief  Contains BigRational and BigRational, wrappers around MPIR.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_NUM_BIG_RATIONAL
#define HG_NUM_BIG_RATIONAL

#include <concepts>
#include <utility>
#include <compare>

#include "big_int.h"

#define COLT_MAKE_OPERATOR(op, mpq_fn, type, arg_expr)                      \
  BigRational& operator COLT_CONCAT(op, =)(type rhs) noexcept               \
  {                                                                         \
    mpq_fn(storage, storage, arg_expr);                                     \
    return *this;                                                           \
  }                                                                         \
  friend BigRational operator op(const BigRational& lhs, type rhs) noexcept \
  {                                                                         \
    BigRational result = lhs;                                               \
    result COLT_CONCAT(op, =) rhs;                                          \
    return result;                                                          \
  }

#define COLT_MAKE_ASSIGNMENT(mpq_fn, type)  \
  BigRational& operator=(type rhs) noexcept \
  {                                         \
    mpq_fn(storage, rhs);                   \
    return *this;                           \
  }

#define COLT_MAKE_OVERLOAD_OPERATOR_BigRational(op, mpq_fn) \
  COLT_MAKE_OPERATOR(op, mpq_fn, const BigRational&, rhs.storage)

#define COLT_MAKE_OVERLOAD_ASSIGNMENT(mpz_fn) \
  COLT_MAKE_ASSIGNMENT(COLT_CONCAT(mpz_fn, _d), f64)

namespace clt::num
{
  /// @brief Represent a signed integer of arbitrary precision.
  /// All BigRational allocate memory, thus avoid temporaries, and
  /// always make use of '[+-*/]=' whenever possible.
  class BigRational
  {
    mpq_t storage{};

  public:
    /// @brief Constructor, sets the value to 0
    BigRational() noexcept { mpq_init(storage); }
    /// @brief Constructor, sets the value to 'value'
    /// @param num The numerator
    /// @param denom The denominator
    explicit BigRational(i32 num, u32 denom = 1) noexcept
    {
      mpq_init(storage);
      mpq_set_si(storage, num, denom);
      mpq_canonicalize(storage);
    }
    /// @brief Constructor, sets the value to 'value'
    /// @param num The numerator
    /// @param denom The denominator
    explicit BigRational(u32 num, u32 denom = 1) noexcept
    {
      mpq_init(storage);
      mpq_set_ui(storage, num, denom);
      mpq_canonicalize(storage);
    }
    /// @brief Constructor, sets the value to 'value'
    /// @param value The initial value
    explicit BigRational(const BigInt& value) noexcept
    {
      mpz_t _value;
      mpq_init(storage);
      value.internal_storage(_value);
      mpq_set_z(storage, _value);
    }
    /// @brief Constructor, sets the value to 'value'
    /// @param value The initial value
    explicit BigRational(double value) noexcept
    {
      mpq_init(storage);
      mpq_set_d(storage, value);
    }
    /// @brief Copy constructor, sets the value to 'value'
    /// @param copy The initial value
    BigRational(const BigRational& copy) noexcept
    {
      mpq_init(storage);
      mpq_set(storage, copy.storage);
    }
    /// @brief Copy assignment operator
    /// @param copy The value to copy
    /// @return Self
    BigRational& operator=(const BigRational& copy) noexcept
    {
      assert_true("Self assignment is prohibited!", this != &copy);
      mpq_set(storage, copy.storage);
      return *this;
    }
    /// @brief Move constructor
    /// @param move The initial value
    BigRational(BigRational&& move) noexcept
    {
      mpq_init(storage);
      mpq_swap(storage, move.storage);
    }
    /// @brief Move assignment operator
    /// @param move The value to move
    /// @return Self
    BigRational& operator=(BigRational&& move) noexcept
    {
      assert_true("Self assignment is prohibited!", this != &move);
      mpq_swap(storage, move.storage);
      return *this;
    }

    COLT_MAKE_OVERLOAD_OPERATOR_BigRational(+, mpq_add);
    template<typename T>
    BigRational& add(const T& value) noexcept
    {
      return *this += value;
    }

    COLT_MAKE_OVERLOAD_OPERATOR_BigRational(-, mpq_sub);
    template<typename T>
    BigRational& sub(const T& value) noexcept
    {
      return *this -= value;
    }

    COLT_MAKE_OVERLOAD_OPERATOR_BigRational(*, mpq_mul);
    template<typename T>
    BigRational& mul(const T& value) noexcept
    {
      return *this *= value;
    }

    COLT_MAKE_OVERLOAD_OPERATOR_BigRational(/, mpq_div);
    template<typename T>
    BigRational& div(const T& value) noexcept
    {
      return *this /= value;
    }

    COLT_MAKE_OVERLOAD_ASSIGNMENT(mpq_set);

    BigRational& operator++() noexcept
    {
      *this += BigRational(1U);
      return *this;
    }

    BigRational operator++(int) noexcept
    {
      BigRational tmp = *this;
      operator++();
      return tmp;
    }

    BigRational& operator--() noexcept
    {
      *this -= BigRational(1U);
      return *this;
    }

    BigRational operator--(int) noexcept
    {
      BigRational tmp = *this;
      operator++();
      return tmp;
    }

    /// @brief Returns the sign of the number.
    /// Return 0 for 0, +1 for positive numbers, and -1 for negative numbers.
    /// @return The sign of the number
    int sgn() const noexcept { return mpq_sgn(storage); }

    /// @brief Negates the current number
    /// @return Self
    BigRational& neg() noexcept
    {
      mpq_neg(storage, storage);
      return *this;
    }

    /// @brief Returns a negated copy of the current number
    /// @return Negated copy of the current number
    BigRational operator-() const noexcept
    {
      BigRational copy = *this;
      copy.neg();
      return copy;
    }

    std::strong_ordering operator<=>(const BigRational& b) const noexcept
    {
      // TODO: make use of mpq_equal for performance
      auto cmp = mpq_cmp(storage, b.storage);
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
      const bool is_neg = mpq_sgn(storage) == -1;
      return mpz_sizeinbase(mpq_numref(storage), base) + 1 + (size_t)is_neg
             + mpz_sizeinbase(mpq_denref(storage), base);
    }

    /// @brief Returns the internal mpq_t.
    /// @warning Never call mpq_clear on the result
    /// @param out The out parameter to which to write the storage
    void internal_storage(mpq_t out) const noexcept { out[0] = *storage; }

    /// @brief Swaps two BigRational
    /// @param with The BigRational to swap with
    void swap(BigRational& with) noexcept { mpq_swap(storage, with.storage); }

    /// @brief Destructor, frees any resource used
    ~BigRational() noexcept { mpq_clear(storage); }

    /// @brief Creates a BigRational from a string (integer or fraction separated by '/')
    /// Set the value of rop from str, a null-terminated C string in base base.
    /// White space is allowed in the string, and is simply ignored.
    /// The base may vary from 2 to 62, or if base is 0, then the leading characters are used:
    /// 0[xX] for hexadecimal, 0[bB] for binary, 0 for octal, or decimal otherwise.
    /// For bases up to 36, case is ignored, upper-case and lower-case letters have the same
    /// value. For bases 37 to 62, upper-case letter represent the usual 10..35 while lower-case
    /// letter represent 36..61.
    /// @param str The string to convert from
    /// @param base The base
    /// @return None if 'str' is not a valid string
    static Option<BigRational> from(const char* str, int base = 0) noexcept
    {
      if (BigRational value; mpq_set_str(value.storage, str, base) == 0)
      {
        mpq_canonicalize(value.storage);
        return std::move(value);
      }
      return None;
    }
  };
} // namespace clt::num

#undef COLT_MAKE_OPERATOR
#undef COLT_MAKE_ASSIGNMENT
#undef COLT_MAKE_OVERLOAD_OPERATOR_BigRational
#undef COLT_MAKE_OVERLOAD_ASSIGNMENT

template<>
/// @brief {fmt} specialization of BigRational
struct fmt::formatter<clt::num::BigRational>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    // TODO: add format option
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const clt::num::BigRational& op, FormatContext& ctx) const
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

    mpq_t value;
    op.internal_storage(value);
    auto ptr = static_cast<char*>(blk.ptr());
    mpq_get_str(ptr, 10, value);

    return fmt::format_to(ctx.out(), "{}", ptr);
  }
};

#endif // !HG_NUM_BIG_RATIONAL
