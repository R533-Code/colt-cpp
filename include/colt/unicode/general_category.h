#ifndef HG_COLT_UNICODE_GENERAL_CATEGORY
#define HG_COLT_UNICODE_GENERAL_CATEGORY

#include <colt/typedefs.h>

namespace clt::unicode
{
  template<typename T, typename... Ts>
  constexpr bool is_one_of(const T& value, const T& first, const Ts&... values)
  {
    return (value == first) || ((value == values) || ...);
  }

  /// @brief Represents the different categories of code points.
  /// See: `https://www.unicode.org/reports/tr44/#General_Category_Values`
  struct GeneralCategory
  {
    /// @brief The different possible category
    enum class Category : u8
    {
      ////////////////////////////
      // CASED LETTER
      ////////////////////////////

      /// @brief an uppercase letter
      Uppercase_Letter = 0,
      /// @brief an uppercase letter
      Lu = Uppercase_Letter,

      /// @brief a lowercase letter
      Lowercase_Letter = 1,
      /// @brief a lowercase letter
      Ll = Lowercase_Letter,

      /// @brief a digraph encoded as a single character, with first part uppercase
      Titlecase_Letter = 2,
      /// @brief a digraph encoded as a single character, with first part uppercase
      Lt = Titlecase_Letter,

      ////////////////////////////
      // LETTER
      ////////////////////////////

      /// @brief a modifier letter
      Modifier_Letter = 3,
      /// @brief a modifier letter
      Lm = Modifier_Letter,

      /// @brief other letters, including syllables and ideographs
      Other_Letter = 4,
      /// @brief other letters, including syllables and ideographs
      Lo = Other_Letter,

      ////////////////////////////
      // MARK
      ////////////////////////////

      /// @brief a nonspacing combining mark (zero advance width)
      Nonspacing_Mark = 5,
      /// @brief a nonspacing combining mark (zero advance width)
      Mn = Nonspacing_Mark,

      /// @brief a spacing combining mark (positive advance width)
      Spacing_Mark = 6,
      /// @brief a spacing combining mark (positive advance width)
      Mc = Spacing_Mark,

      /// @brief an enclosing combining mark
      Enclosing_Mark = 7,
      /// @brief an enclosing combining mark
      Me = Enclosing_Mark,

      ////////////////////////////
      // NUMBER
      ////////////////////////////

      /// @brief a decimal digit
      Decimal_Number = 8,
      /// @brief a decimal digit
      Nd = Decimal_Number,

      /// @brief a letterlike numeric character
      Letter_Number = 9,
      /// @brief a letterlike numeric character
      Nl = Letter_Number,

      /// @brief a numeric character of other type
      Other_Number = 10,
      /// @brief a numeric character of other type
      No = Other_Number,

      ////////////////////////////
      // PUNCTUATION
      ////////////////////////////

      /// @brief a connecting punctuation mark, like a tie
      Connector_Punctuation = 11,
      /// @brief a connecting punctuation mark, like a tie
      Pc = Connector_Punctuation,

      /// @brief a dash or hyphen punctuation mark
      Dash_Punctuation = 12,
      /// @brief a dash or hyphen punctuation mark
      Pd = Dash_Punctuation,

      /// @brief an opening punctuation mark (of a pair)
      Open_Punctuation = 13,
      /// @brief an opening punctuation mark (of a pair)
      Ps = Open_Punctuation,

      /// @brief a closing punctuation mark (of a pair)
      Close_Punctuation = 14,
      /// @brief a closing punctuation mark (of a pair)
      Pe = Close_Punctuation,

      /// @brief an initial quotation mark
      Initial_Punctuation = 15,
      /// @brief an initial quotation mark
      Pi = Initial_Punctuation,

      /// @brief a final quotation mark
      Final_Punctuation = 16,
      /// @brief a final quotation mark
      Pf = Final_Punctuation,

      /// @brief a punctuation mark of other type
      Other_Punctuation = 17,
      /// @brief a punctuation mark of other type
      Po = Other_Punctuation,

      ////////////////////////////
      // SYMBOL
      ////////////////////////////

      /// @brief a symbol of mathematical use
      Math_Symbol = 18,
      /// @brief a symbol of mathematical use
      Sm = Math_Symbol,

      /// @brief a currency sign
      Currency_Symbol = 19,
      /// @brief a currency sign
      Sc = Currency_Symbol,

      /// @brief a non-letterlike modifier symbol
      Modifier_Symbol = 20,
      /// @brief a non-letterlike modifier symbol
      Sk = Modifier_Symbol,

      /// @brief a symbol of other type
      Other_Symbol = 21,
      /// @brief a symbol of other type
      So = Other_Symbol,

      ////////////////////////////
      // SEPERATOR
      ////////////////////////////

      /// @brief a space character (of various non-zero widths)
      Space_Separator = 22,
      /// @brief a space character (of various non-zero widths)
      Zs = Space_Separator,

      /// @brief U+2028 LINE SEPARATOR only
      Line_Separator = 23,
      /// @brief U+2028 LINE SEPARATOR only
      Zl = Line_Separator,

      /// @brief U+2029 PARAGRAPH SEPARATOR only
      Paragraph_Separator = 24,
      /// @brief U+2029 PARAGRAPH SEPARATOR only
      Zp = Paragraph_Separator,

      ////////////////////////////
      // Other
      ////////////////////////////

      /// @brief a C0 or C1 control code
      Control = 25,
      /// @brief a C0 or C1 control code
      Cc = Control,

      /// @brief a format control character
      Format = 26,
      /// @brief a format control character
      Cf = Format,

      /// @brief a surrogate code point
      Surrogate = 27,
      /// @brief a surrogate code point
      Cs = Surrogate,

      /// @brief a private-use character
      Private_Use = 28,
      /// @brief a private-use character
      Co = Private_Use,

      /// @brief a reserved unassigned code point or a noncharacter
      Unassigned = 29,
      /// @brief a reserved unassigned code point or a noncharacter
      Cn = Unassigned,
    };
    using enum Category;

    /// @brief The category
    Category category;

    constexpr bool operator==(const GeneralCategory&) const noexcept = default;

    /// @brief Check if the category is one of (Lu | Ll | Lt).
    /// Same as `is_LC()`.
    /// @return True if (Lu | Ll | Lt)
    constexpr bool is_cased_letter() const noexcept
    {
      return is_one_of(category, Lu, Ll, Lt);
    }
    /// @brief Check if the category is one of (Lu | Ll | Lt)
    /// Same as `is_cased_letter()`.
    /// @return True if (Lu | Ll | Lt)
    constexpr bool is_LC() const noexcept { return is_cased_letter(); }

    /// @brief Check if the category is one of (Lu | Ll | Lt | Lm | Lo)
    /// Same as `is_L()`
    /// @return True if (Lu | Ll | Lt | Lm | Lo)
    constexpr bool is_letter() const noexcept
    {
      return is_one_of(category, Lo, Lm) || is_cased_letter();
    }
    /// @brief Check if the category is one of (Lu | Ll | Lt | Lm | Lo)
    /// Same as `is_letter()`
    /// @return True if (Lu | Ll | Lt | Lm | Lo)
    constexpr bool is_L() const noexcept { return is_letter(); }

    /// @brief Check if the category is one of (Mn | Mc | Me)
    /// Same as `is_M()`
    /// @return True if (Mn | Mc | Me)
    constexpr bool is_mark() const noexcept
    {
      return is_one_of(category, Mn, Mc, Me);
    }
    /// @brief Check if the category is one of (Mn | Mc | Me)
    /// Same as `is_mark()`
    /// @return True if (Mn | Mc | Me)
    constexpr bool is_M() const noexcept { return is_one_of(category, Mn, Mc, Me); }

    /// @brief Check if the category is one of (Nd | Nl | No)
    /// Same as `is_N()`
    /// @return True if (Nd | Nl | No)
    constexpr bool is_number() const noexcept
    {
      return is_one_of(category, Nd, Nl, No);
    }
    /// @brief Check if the category is one of (Nd | Nl | No)
    /// Same as `is_number()`
    /// @return True if (Nd | Nl | No)
    constexpr bool is_N() const noexcept { return is_number(); }
    
    /// @brief Check if the category is one of (Pc | Pd | Ps | Pe | Pi | Pf | Po)
    /// Same as `is_P()`
    /// @return True if (Pc | Pd | Ps | Pe | Pi | Pf | Po)
    constexpr bool is_punctuation() const noexcept
    {
      return is_one_of(category, Nd, Nl, No);
    }
    /// @brief Check if the category is one of (Pc | Pd | Ps | Pe | Pi | Pf | Po)
    /// Same as `is_punctuation()`
    /// @return True if (Pc | Pd | Ps | Pe | Pi | Pf | Po)
    constexpr bool is_P() const noexcept { return is_punctuation(); }

    /// @brief Check if the category is one of (Sm | Sc | Sk | So)
    /// Same as `is_S()`
    /// @return True if (Sm | Sc | Sk | So)
    constexpr bool is_symbol() const noexcept
    {
      return is_one_of(category, Sm, Sc, Sk, So);
    }
    /// @brief Check if the category is one of (Sm | Sc | Sk | So)
    /// Same as `is_symbol()`
    /// @return True if (Sm | Sc | Sk | So)
    constexpr bool is_S() const noexcept { return is_symbol(); }

    /// @brief Check if the category is one of (Zs | Zl | Zp)
    /// Same as `is_Z()`
    /// @return True if (Zs | Zl | Zp)
    constexpr bool is_separator() const noexcept
    {
      return is_one_of(category, Zs, Zl, Zp);
    }
    /// @brief Check if the category is one of (Zs | Zl | Zp)
    /// Same as `is_separator()`
    /// @return True if (Zs | Zl | Zp)
    constexpr bool is_Z() const noexcept { return is_separator(); }

    /// @brief Check if the category is one of (Cc | Cf | Cs | Co | Cn)
    /// Same as `is_C()`
    /// @return True if (Cc | Cf | Cs | Co | Cn)
    constexpr bool is_other() const noexcept
    {
      return is_one_of(category, Cc, Cf, Cs, Co, Cn);
    }
    /// @brief Check if the category is one of (Cc | Cf | Cs | Co | Cn)
    /// Same as `is_other()`
    /// @return True if (Cc | Cf | Cs | Co | Cn)
    constexpr bool is_C() const noexcept { return is_other(); }
  };

} // namespace clt::unicode

#endif // !HG_COLT_UNICODE_GENERAL_CATEGORY