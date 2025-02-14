/*****************************************************************/ /**
 * @file   unicode.h
 * @brief  Contains unicode utilities used throughout the library.
 * 
 * `unitlen` returns the number of char[8|16|32]_t that makes up a
 * NUL-terminated string.
 * `strlen` returns the number of CODE POINTS that makes up a
 * NUL-terminated string.
 * `countlen` returns the number of CODE POINTS that makes up
 * a string of 'n' units.
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#ifndef HG_DSA_UNICODE
#define HG_DSA_UNICODE

#include "unicode_types.h"
#include "propvalues.h"

namespace clt
{
  namespace details
  {
    /// @brief Used to extract data from CHAR_INFO_TABLE (see example in 'islower')
    enum CharInfo : u8
    {
      /// @brief Check if the result of indexing into
      /// CHAR_INFO_TABLE is a control character.
      ISCNTRL = (1 << 0),
      /// @brief Check if the result of indexing into
      /// CHAR_INFO_TABLE is a digit character.
      ISDIGIT = (1 << 2),
      /// @brief Check if the result of indexing into
      /// CHAR_INFO_TABLE is a lower case letter.
      ISLOWER = (1 << 3),
      /// @brief Check if the result of indexing into
      /// CHAR_INFO_TABLE is a punctuation character.
      ISPUNCT = (1 << 4),
      /// @brief Check if the result of indexing into
      /// CHAR_INFO_TABLE is a whitespace character.
      ISSPACE = (1 << 5),
      /// @brief Check if the result of indexing into
      /// CHAR_INFO_TABLE is an upper case letter.
      ISUPPER = (1 << 6),

      /// @brief Check if the result of indexing into
      /// CHAR_INFO_TABLE is a letter.
      ISALPHA = ISUPPER | ISLOWER,
      /// @brief Check if the result of indexing into
      /// CHAR_INFO_TABLE is an alpha-numeric character.
      ISALNUM = ISALPHA | ISDIGIT,
      /// @brief Check if the result of indexing into
      /// CHAR_INFO_TABLE is a graphical character.
      ISGRAPH = ISALNUM | ISPUNCT,
    };

    /// @brief Table of all the characters information
    constexpr std::array<u8, 256> CHAR_INFO_TABLE = {
        0b00000001, //[NULL]
        0b00000001, //[START OF HEADING]
        0b00000001, //[START OF TEXT]
        0b00000001, //[END OF TEXT]
        0b00000001, //[END OF TRANSMISSION]
        0b00000001, //[ENQUIRY]
        0b00000001, //[ACKNOWLEDGE]
        0b00000001, //[BELL]
        0b00000001, //[BACKSPACE]
        0b00100001, //[HORIZONTAL TAB]
        0b00100001, //[LINE FEED]
        0b00100001, //[VERTICAL TAB]
        0b00100001, //[FORM FEED]
        0b00100001, //[CARRIAGE RETURN]
        0b00000001, //[SHIFT OUT]
        0b00000001, //[SHIFT IN]
        0b00000001, //[DATA LINK ESCAPE]
        0b00000001, //[DEVICE CONTROL 1]
        0b00000001, //[DEVICE CONTROL 2]
        0b00000001, //[DEVICE CONTROL 3]
        0b00000001, //[DEVICE CONTROL 4]
        0b00000001, //[NEGATIVE ACKNOWLEDGE]
        0b00000001, //[SYNCHRONOUS IDLE]
        0b00000001, //[ENG OF TRANS. BLOCK]
        0b00000001, //[CANCEL]
        0b00000001, //[END OF MEDIUM]
        0b00000001, //[SUBSTITUTE]
        0b00000001, //[ESCAPE]
        0b00000001, //[FILE SEPARATOR]
        0b00000001, //[GROUP SEPARATOR]
        0b00000001, //[RECORD SEPARATOR]
        0b00000001, //[UNIT SEPARATOR]
        0b00100000, //[SPACE]
        0b00010000, // '!'
        0b00010000, // '"'
        0b00010000, // '#'
        0b00010000, // '$'
        0b00010000, // '%'
        0b00010000, // '&'
        0b00010000, // '''
        0b00010000, // '('
        0b00010000, // ')'
        0b00010000, // '*'
        0b00010000, // '+'
        0b00010000, // ','
        0b00010000, // '-'
        0b00010000, // '.'
        0b00010000, // '/'
        0b00000100, // '0'
        0b00000100, // '1'
        0b00000100, // '2'
        0b00000100, // '3'
        0b00000100, // '4'
        0b00000100, // '5'
        0b00000100, // '6'
        0b00000100, // '7'
        0b00000100, // '8'
        0b00000100, // '9'
        0b00010000, // ':'
        0b00010000, // ';'
        0b00010000, // '<'
        0b00010000, // '='
        0b00010000, // '>'
        0b00010000, // '?'
        0b00010000, // '@'
        0b01000000, // 'A'
        0b01000000, // 'B'
        0b01000000, // 'C'
        0b01000000, // 'D'
        0b01000000, // 'E'
        0b01000000, // 'F'
        0b01000000, // 'G'
        0b01000000, // 'H'
        0b01000000, // 'I'
        0b01000000, // 'J'
        0b01000000, // 'K'
        0b01000000, // 'L'
        0b01000000, // 'M'
        0b01000000, // 'N'
        0b01000000, // 'O'
        0b01000000, // 'P'
        0b01000000, // 'Q'
        0b01000000, // 'R'
        0b01000000, // 'S'
        0b01000000, // 'T'
        0b01000000, // 'U'
        0b01000000, // 'V'
        0b01000000, // 'W'
        0b01000000, // 'X'
        0b01000000, // 'Y'
        0b01000000, // 'Z'
        0b00010000, // '['
        0b00010000, // '\'
        0b00010000, // ']'
        0b00010000, // '^'
        0b00010000, // '_'
        0b00010000, // '`'
        0b00001000, // 'a'
        0b00001000, // 'b'
        0b00001000, // 'c'
        0b00001000, // 'd'
        0b00001000, // 'e'
        0b00001000, // 'f'
        0b00001000, // 'g'
        0b00001000, // 'h'
        0b00001000, // 'i'
        0b00001000, // 'j'
        0b00001000, // 'k'
        0b00001000, // 'l'
        0b00001000, // 'm'
        0b00001000, // 'n'
        0b00001000, // 'o'
        0b00001000, // 'p'
        0b00001000, // 'q'
        0b00001000, // 'r'
        0b00001000, // 's'
        0b00001000, // 't'
        0b00001000, // 'u'
        0b00001000, // 'v'
        0b00001000, // 'w'
        0b00001000, // 'x'
        0b00001000, // 'y'
        0b00001000, // 'z'
        0b00010000, // '{'
        0b00010000, // '|'
        0b00010000, // '}'
        0b00010000, // '~'
        0b00000001, //[DEL]
    };
  } // namespace details

  /// @brief Checks if the given character is a control character.
  /// Locale independent.
  /// @param chr The character to check
  /// @return True if the character is a control character
  constexpr bool iscntrl(char chr) noexcept
  {
    using namespace details;
    return CHAR_INFO_TABLE[static_cast<u8>(chr)] & CharInfo::ISCNTRL;
  }

  /// @brief Checks if the given character is a letter.
  /// Locale independent.
  /// @param chr The character to check
  /// @return True if the character is a letter
  constexpr bool isalpha(char chr) noexcept
  {
    using namespace details;
    return CHAR_INFO_TABLE[static_cast<u8>(chr)] & CharInfo::ISALPHA;
  }

  /// @brief Checks if the given character is a digit (0-9).
  /// Locale independent.
  /// @param chr The character to check
  /// @return True if the character is a digit
  constexpr bool isdigit(char chr) noexcept
  {
    using namespace details;
    return CHAR_INFO_TABLE[static_cast<u8>(chr)] & CharInfo::ISDIGIT;
  }

  /// @brief Checks if the given character is a letter or digit.
  /// Locale independent.
  /// @param chr The character to check
  /// @return True if the character is an alpha-numeric character
  constexpr bool isalnum(char chr) noexcept
  {
    using namespace details;
    return CHAR_INFO_TABLE[static_cast<u8>(chr)] & CharInfo::ISALNUM;
  }

  /// @brief Checks if the given character is an lower case letter.
  /// Locale independent.
  /// @param chr The character to check
  /// @return True if the character is an lower case letter
  constexpr bool islower(char chr) noexcept
  {
    using namespace details;
    return CHAR_INFO_TABLE[static_cast<u8>(chr)] & CharInfo::ISLOWER;
  }

  /// @brief Checks if the given character is an upper case letter.
  /// Locale independent.
  /// @param chr The character to check
  /// @return True if the character is an upper case letter
  constexpr bool isupper(char chr) noexcept
  {
    using namespace details;
    return CHAR_INFO_TABLE[static_cast<u8>(chr)] & CharInfo::ISUPPER;
  }

  /// @brief Checks if the given character is any of !\"\#\$\%\&'()*+,-\./:;\<\=\>?\@[\\]^_`\{\|\}\~.
  /// Locale independent.
  /// @param chr The character to check
  /// @return True if the character is a punctuation character
  constexpr bool ispunct(char chr) noexcept
  {
    using namespace details;
    return CHAR_INFO_TABLE[static_cast<u8>(chr)] & CharInfo::ISPUNCT;
  }

  /// @brief Checks if the given character has a graphical representation.
  /// Locale independent.
  /// @param chr The character to check
  /// @return True if the character is graphic
  constexpr bool isgraph(char chr) noexcept
  {
    using namespace details;
    return CHAR_INFO_TABLE[static_cast<u8>(chr)] & CharInfo::ISGRAPH;
  }

  /// @brief Checks if a character is a ' ', '\\f', '\\n', '\\r', '\\t' or '\\v'.
  /// Locale independent.
  /// @param chr The character to check
  /// @return True if the character is whitespace
  constexpr bool isspace(char chr) noexcept
  {
    using namespace details;
    return CHAR_INFO_TABLE[static_cast<u8>(chr)] & CharInfo::ISSPACE;
  }

  /// @brief Checks if a character is a space or horizontal tab.
  /// Locale independent.
  /// @param chr The character to check
  /// @return True if space or horizontal tab
  constexpr bool isblank(char chr) noexcept
  {
    //Most compiler optimize '||' to an OR instruction in
    //this case (to avoid branching), but do it anyway...
    return static_cast<u8>(chr == ' ') | static_cast<u8>(chr == '\t');
  }

  /// @brief Converts a character to its upper case equivalent only if it is a lower case letter.
  /// Locale independent.
  /// @param chr The character to convert
  /// @return Upper case equivalent if 'chr' is lower case, else 'chr'
  constexpr char toupper(char chr) noexcept
  {
    //Most compiler would compile the code into a branch-less equivalent
    //if we made use of an 'if' statement, but do it anyway...
    //The XOR allows us to turn off the bit that makes the letter lower case.
    //The multiplication forces the mask to be 0 (thus not affecting 'chr')
    //if 'chr' is a not lower case letter.
    return chr ^ (0b00100000 * static_cast<u8>(islower(chr)));
  }

  /// @brief Converts a character to its lower case equivalent only if it is an upper case letter.
  /// Locale independent.
  /// @param chr The character to convert
  /// @return Lower case equivalent if 'chr' is upper case, else 'chr'
  constexpr char tolower(char chr) noexcept
  {
    //Most compiler would compile the code into a branch-less equivalent
    //if we made use of an 'if' statement, but do it anyway...
    //To see explanation, look at: 'clt::toupper'.
    return chr | (0b00100000 * static_cast<u8>(isupper(chr)));
  }
} // namespace clt

namespace clt::uni
{
  /// @brief Represents a conversion error
  enum class ConvError
  {
    /// @brief No errors
    NO_ERROR,
    /// @brief Not enough space in the resulting buffer
    NOT_ENOUGH_SPACE,
    /// @brief Invalid input sequence
    INVALID_INPUT,
  };

  /// @brief Converts any unicode encoding to UTF-8.
  /// The pointers are taken by reference:
  /// On INVALID_INPUT, 'from' will point to the start of the character
  /// sequence that is invalid.
  /// On NOT_ENOUGH_SPACE, 'from' will point to the
  /// @tparam Ty The source type
  /// @param from The source start
  /// @param from_size The source count (not byte size!)
  /// @param result The result where to write
  /// @param result_size The result buffer byte count
  /// @return NO_ERROR or the encountered error.
  template<meta::CharType Ty>
  constexpr ConvError to_utf8(
      const Ty*& from, size_t from_size, char8_t*& result,
      size_t result_size) noexcept;

  /// @brief Converts a code point in host endianness to a UTF16BE sequence
  /// @param from The code point to convert
  /// @param result Pointer to write to
  /// @warning 'result' must have at least 2 16-bit integers of capacity
  /// @return Pointer to after the last written character
  constexpr char16_t* unsafe_utf32to16(char32_t from, char16_t* result) noexcept;

  /// @brief Converts a UTF16 surrogate pair to code point
  /// @param high The high surrogate
  /// @param low The low surrogate
  /// @return The code point
  constexpr char32_t surrogate_to_cp(char16_t high, char16_t low) noexcept
  {
    return (high << 10) + low - 0x35fdc00;
  }

  /// @brief Converts a UTF16 sequence to a code point
  /// @param from The start of the sequence
  /// @param result The result in which to write the code point
  /// @return On error, returns 'from' and sets result to REPLACEMENT CHARACTER,
  ///         else the next start of sequence.
  template<typename T>
    requires(meta::is_any_of<T, Char16BE, Char16LE>)
  constexpr const T* unsafe_utf16to32(const T* from, char32_t& result) noexcept;

  /// @brief Converts a UTF8 sequence to a code point
  /// @param from The start of the sequence
  /// @param result The result in which to write the code point
  /// @return On error, returns 'from' and sets result to REPLACEMENT CHARACTER,
  ///         else the next start of sequence.
  constexpr const Char8* unsafe_utf8to32(
      const Char8* from, char32_t& result) noexcept;

  /// @brief Indexes into a sequence of characters, returning the n-th code point.
  /// @tparam underlying_type The character encoding type
  /// @param _ptr The start of the sequence
  /// @param index The code point index
  /// @return The n-th code point
  /// @warning The sequence must be valid unicode
  template<meta::CharType underlying_type>
  constexpr char32_t index_front(const underlying_type* _ptr, size_t index) noexcept;

  /// @brief Indexes the back of a unicode sequence.
  /// This function will fire an assert on invalid sequences
  /// @tparam underlying_type The encoding char type
  /// @param _ptr The pointer (must be passed the end of the last character)
  /// @param _index The index (0 for last, 1 for penultimate)
  /// @return The decoded code point
  template<meta::CharType underlying_type>
  constexpr char32_t index_back(const underlying_type* _ptr, size_t _index) noexcept;

  template<typename underlying_type>
    requires(
        meta::CharType<std::remove_cv_t<underlying_type>>
        || meta::CppCharType<std::remove_cv_t<underlying_type>>)
  constexpr underlying_type* iterator_index_front(
      underlying_type* _ptr, size_t _index) noexcept;

  template<typename underlying_type>
    requires(
        meta::CharType<std::remove_cv_t<underlying_type>>
        || meta::CppCharType<std::remove_cv_t<underlying_type>>)
  constexpr underlying_type* iterator_index_back(
      underlying_type* _ptr, size_t _index) noexcept;

  /// @brief Returns the size in bytes of a NUL-terminated string
  /// @tparam T The char type
  /// @param start The string whose size in bytes to determine
  /// @return The size in bytes (not including NUL-terminator)
  template<typename T>
    requires(meta::CppCharType<T> || meta::CharType<T>)
  constexpr size_t unitlen(const T* start) noexcept;

  /// @brief Count the number of code points of a string of size 'count'.
  /// @tparam T The encoding char type
  /// @param start The start of the
  /// @param units The unit count
  /// @return The number of code points
  template<typename T>
    requires(meta::CppCharType<T> || meta::CharType<T>)
  constexpr size_t countlen(const T* start, size_t units) noexcept;

  /// @brief Returns the number of code points of a NUL-terminated string
  /// @tparam T The char type
  /// @param start The string whose length to determine
  /// @return The size in code points (not including NUL-terminator)
  template<typename T>
    requires(meta::CppCharType<T> || meta::CharType<T>)
  constexpr size_t strlen(const T* start) noexcept;

  /// @brief Represents the result of 'len'.
  /// For UTF32 and ASCII both fields are the same
  struct LenInfo
  {
    /// @brief The number of code point encoded
    size_t strlen;
    /// @brief The number of storage unit used
    size_t unitlen;
  };

  /// @brief Returns the strlen and unitlen of a NUL terminated string.
  /// @tparam T The char type
  /// @param start The start of the NUL terminated string
  /// @return LenInfo representing the result
  template<typename T>
    requires(meta::CppCharType<T> || meta::CharType<T>)
  constexpr LenInfo len(const T* start) noexcept;

  namespace details
  {
    /// @brief Optimized strlen for UTF8
    /// @param ptr The NUL-terminated string whose code point count to return
    /// @return Return the number of code point (not including NUL-terminator)
    COLTCPP_EXPORT LenInfo len8(const char8_t* ptr) noexcept;
    /// @brief Optimized strlen for UTF16LE
    /// @param ptr The NUL-terminated string whose code point count to return
    /// @return Return the number of code point (not including NUL-terminator)
    COLTCPP_EXPORT LenInfo len16LE(const char16_t* ptr) noexcept;
    /// @brief Optimized strlen for UTF16BE
    /// @param ptr The NUL-terminated string whose code point count to return
    /// @return Return the number of code point (not including NUL-terminator)
    COLTCPP_EXPORT LenInfo len16BE(const char16_t* ptr) noexcept;
    /// @brief Optimized strlen for native UTF16
    /// @param ptr The NUL-terminated string whose code point count to return
    /// @return Return the number of code point (not including NUL-terminator)
    COLTCPP_EXPORT LenInfo len16(const char16_t* ptr) noexcept;
    /// @brief Optimized unitlen for UTF16.
    /// This works for both endianness as zero are represented the same
    /// way on both endianness.
    /// The implementation uses SIMD instructions.
    /// @param ptr The NUL-terminated string whose unit count to return
    /// @return Return the count of char16_t forming the string
    COLTCPP_EXPORT size_t unitlen16(const char16_t* ptr) noexcept;
    /// @brief Optimized unitlen for UTF32.
    /// This works for both endianness as zero are represented the same
    /// way on both endianness.
    /// The implementation uses SIMD instructions.
    /// @param ptr The NUL-terminated string whose unit count to return
    /// @return Return the count of char32_t forming the string
    COLTCPP_EXPORT size_t unitlen32(const char32_t* ptr) noexcept;
  } // namespace details

  /// @brief Iterator over Unicode encoded strings
  /// @tparam ENCODING The encoding
  template<StringEncoding ENCODING>
  class CodePointIterator
  {
    using ptr_t = meta::encoding_to_char_t<ENCODING>;

    const ptr_t* ptr;

  public:
    constexpr CodePointIterator(const ptr_t* ptr) noexcept
        : ptr(ptr)
    {
    }

    MAKE_DEFAULT_COPY_AND_MOVE_FOR(CodePointIterator);

    constexpr const ptr_t* current() const noexcept { return ptr; }

    constexpr CodePointIterator& operator++() noexcept
    {
      if constexpr (meta::is_any_of<ptr_t, Char32BE, Char32LE, char>)
        ++ptr;
      if constexpr (meta::is_any_of<ptr_t, Char16BE, Char16LE>)
        ptr += ptr->sequence_length();
      if constexpr (meta::is_any_of<ptr_t, Char8>)
      {
        auto value = ptr->sequence_length();
        assert_true("Invalid UTF8!", value.is_value());
        ptr += *value;
      }
      return *this;
    }

    constexpr CodePointIterator operator++(int)
    {
      auto copy = *this;
      ++(*this);
      return copy;
    }

    constexpr CodePointIterator& operator--()
    {
      if constexpr (meta::is_any_of<ptr_t, Char32BE, Char32LE, char>)
        --ptr;
      if constexpr (meta::is_any_of<ptr_t, Char16BE, Char16LE>)
        ptr -= 1 + ptr->is_trail_surrogate();
      if constexpr (meta::is_any_of<ptr_t, Char8>)
      {
        --ptr;
        while (ptr->is_trail())
          --ptr;
      }
      return *this;
    }

    constexpr CodePointIterator operator--(int)
    {
      auto copy = *this;
      --(*this);
      return copy;
    }

    constexpr char32_t operator*() noexcept
    {
      if constexpr (meta::is_any_of<ptr_t, Char32BE, Char32LE, char>)
        return static_cast<char32_t>(*ptr);
      if constexpr (meta::is_any_of<ptr_t, Char16BE, Char16LE>)
      {
        char32_t result;
        auto check = unsafe_utf16to32(ptr, result);
        assert_true("Invalid UTF16!", check != ptr);
        return result;
      }
      if constexpr (meta::is_any_of<ptr_t, Char8>)
      {
        char32_t result;
        auto check = unsafe_utf8to32(ptr, result);
        assert_true("Invalid UTF8!", check != ptr);
        return result;
      }
    }

    friend constexpr auto operator<=>(
        const CodePointIterator&, const CodePointIterator&) noexcept = default;
  };

  /************************
  | IMPLEMENTATIONS vvv   |
  ************************/

  constexpr char16_t* unsafe_utf32to16(char32_t from, char16_t* result) noexcept
  {
    if (const u32 cp = from; cp < (u32)0x10000)
      *(result++) = static_cast<char16_t>(cp);
    else
    {
      *(result++) = (char16_t)(u16)(LEAD_OFFSET + (cp >> 10));
      *(result++) = (char16_t)(u16)(TRAIL_SURROGATE_MIN + (cp & 0x3FF));
    }
    return result;
  }

  constexpr const Char8* unsafe_utf8to32(
      const Char8* from, char32_t& result) noexcept
  {
    auto first = *from;
    auto value = first.sequence_length();
    if (value.is_none())
    {
      result = U'\ufffd';
      return from;
    }
    auto ret = static_cast<char32_t>(first);
    switch_no_default(*value)
    {
    case 1:
      break;
    case 2:
      ++from;
      ret = ((ret << 6) & 0x7ff) + ((*from) & 0x3f);
      break;
    case 3:
      ++from;
      ret = ((ret << 12) & 0xffff) + ((*from << 6) & 0xfff);
      ++from;
      ret += (*from) & 0x3f;
      break;
    case 4:
      ++from;
      ret = ((ret << 18) & 0x1fffff) + ((*from << 12) & 0x3ffff);
      ++from;
      ret += (*from << 6) & 0xfff;
      ++from;
      ret += (*from) & 0x3f;
      break;
    }
    result = ret;
    return from + 1;
  }

  template<meta::CharType Ty>
  constexpr ConvError to_utf8(
      const Ty*& from, size_t from_size, char8_t*& result,
      size_t result_size) noexcept
  {
    if constexpr (std::same_as<Ty, char>)
    {
      const auto min = clt::min(from_size, result_size);
      if (std::is_constant_evaluated())
      {
        const auto max_from = from + min;
        while (from != max_from)
          *result++ = *from++;
      }
      else
      {
        std::memcpy(result, from, min);
        result += min;
        from += min;
      }
      return min < from_size * sizeof(Ty) ? ConvError::NOT_ENOUGH_SPACE
                                          : ConvError::NO_ERROR;
    }
    if constexpr (meta::is_any_of<Ty, Char32BE, Char32LE>)
    {
      const auto max_from   = from + from_size;
      const auto max_result = result + result_size;

      while (from != max_from)
      {
        const u32 as_host = from->as_host();
        if (as_host > CODE_POINT_MAX) [[unlikely]]
          return ConvError::INVALID_INPUT;

        if (const u32 cp = as_host; cp < 0x80) [[likely]]
        {
          if (result >= max_result)
            return ConvError::NOT_ENOUGH_SPACE;
          *(result++) = static_cast<char8_t>(cp);
        }
        else if (cp < 0x800) // 2 bytes
        {
          if (result + 1 >= max_result)
            return ConvError::NOT_ENOUGH_SPACE;
          *result++ = static_cast<char8_t>((cp >> 6) | 0xc0);
          *result++ = static_cast<char8_t>((cp & 0x3f) | 0x80);
        }
        else if (cp < 0x10000) // 3 bytes
        {
          if (result + 2 >= max_result)
            return ConvError::NOT_ENOUGH_SPACE;
          *result++ = static_cast<char8_t>((cp >> 12) | 0xe0);
          *result++ = static_cast<char8_t>(((cp >> 6) & 0x3f) | 0x80);
          *result++ = static_cast<char8_t>((cp & 0x3f) | 0x80);
        }
        else // 4 bytes
        {
          if (result + 3 >= max_result)
            return ConvError::NOT_ENOUGH_SPACE;
          *result++ = static_cast<char8_t>((cp >> 18) | 0xf0);
          *result++ = static_cast<char8_t>(((cp >> 12) & 0x3f) | 0x80);
          *result++ = static_cast<char8_t>(((cp >> 6) & 0x3f) | 0x80);
          *result++ = static_cast<char8_t>((cp & 0x3f) | 0x80);
        }
        from++;
      }
      return ConvError::NO_ERROR;
    }
    // TODO: add UTF16 support
  }

  template<typename T>
    requires(meta::is_any_of<T, Char16BE, Char16LE>)
  constexpr const T* unsafe_utf16to32(const T* from, char32_t& result) noexcept
  {
    auto first = *from;
    auto value = first.sequence_length();
    if (first.is_lead_surrogate()) [[unlikely]]
    {
      if (auto second = from[1]; second.is_trail_surrogate()) [[likely]]
      {
        result = surrogate_to_cp(first, second);
        return from + 2;
      }
      // lead surrogate must be followed by trail
      result = U'\ufffd';
      return from;
    }
    ++from;
    result = static_cast<char32_t>(first);
    return from;
  }

  template<meta::CharType underlying_type>
  constexpr char32_t index_front(const underlying_type* _ptr, size_t index) noexcept
  {
    auto ptr = iterator_index_front(_ptr, index);
    if constexpr (meta::is_any_of<underlying_type, Char32BE, Char32LE, char>)
      return static_cast<char32_t>(*ptr);
    if constexpr (meta::is_any_of<underlying_type, Char8>)
    {
      char32_t result;
      auto check = unsafe_utf8to32(ptr, result);
      assert_true("Invalid UTF8!", ptr != check);
      return result;
    }
    if constexpr (meta::is_any_of<underlying_type, Char16BE, Char16LE>)
    {
      char32_t result;
      auto check = unsafe_utf16to32(ptr, result);
      assert_true("Invalid UTF16!", ptr != check);
      return result;
    }
  }

  template<meta::CharType underlying_type>
  constexpr char32_t index_back(const underlying_type* _ptr, size_t _index) noexcept
  {
    auto ptr = iterator_index_back(_ptr, _index);
    if constexpr (meta::is_any_of<underlying_type, Char32BE, Char32LE, char>)
      return static_cast<char32_t>(*ptr);
    if constexpr (meta::is_any_of<underlying_type, Char8>)
    {
      char32_t result;
      auto check = unsafe_utf8to32(ptr, result);
      assert_true("Invalid UTF8!", ptr != check);
      return result;
    }
    if constexpr (meta::is_any_of<underlying_type, Char16BE, Char16LE>)
    {
      char32_t result;
      auto check = unsafe_utf16to32(ptr, result);
      assert_true("Invalid UTF16!", ptr != check);
      return result;
    }
  }

  template<typename underlying_type>
    requires(
        meta::CharType<std::remove_cv_t<underlying_type>>
        || meta::CppCharType<std::remove_cv_t<underlying_type>>)
  constexpr underlying_type* iterator_index_front(
      underlying_type* _ptr, size_t index) noexcept
  {
    using ty = std::remove_cv_t<underlying_type>;
    if constexpr (meta::is_any_of<ty, Char32BE, Char32LE>)
      return _ptr + index;
    else if constexpr (meta::is_any_of<ty, char>)
      return _ptr + index;
    else
    {
      auto ptr = _ptr;
      while (index != 0)
      {
        auto len = uni::sequence_length(*ptr);
        --index;
        ptr += len;
      }
      return ptr;
    }
  }

  template<typename underlying_type>
    requires(
        meta::CharType<std::remove_cv_t<underlying_type>>
        || meta::CppCharType<std::remove_cv_t<underlying_type>>)
  constexpr underlying_type* iterator_index_back(
      underlying_type* _ptr, size_t _index) noexcept
  {
    using ty = std::remove_cv_t<underlying_type>;
    // TODO: handle possible overflow when negating index
    if constexpr (meta::is_any_of<ty, char, Char32BE, Char32LE>)
      return _ptr + -static_cast<i64>(_index);
    if constexpr (meta::is_any_of<ty, Char8, char8_t>)
    {
      auto ptr = _ptr;
      while (_index != 0)
      {
        while (uni::is_trail(*ptr))
          --ptr;
        --ptr;
        --_index;
      }
      while (uni::is_trail(*ptr))
        --ptr;
      return ptr;
    }
    if constexpr (meta::is_any_of<ty, Char16BE, Char16LE, char16_t>)
    {
      auto ptr = _ptr;
      while (_index != 0)
      {
        ptr -= 1 + uni::is_trail_surrogate(*ptr);
        --_index;
      }
      return ptr;
    }
  }

  template<typename T>
    requires(meta::CppCharType<T> || meta::CharType<T>)
  constexpr size_t unitlen(const T* start) noexcept
  {
    assert_true("Expected non-null pointer!", start != nullptr);
    if (std::is_constant_evaluated())
    {
      const T* end = start;
      while (*end != T{})
        ++end;
      return end - start;
    }
    else
    {
      if constexpr (std::same_as<char, T>)
        return std::strlen(start);
      if constexpr (meta::is_any_of<T, char8_t, Char8>)
        return std::strlen(reinterpret_cast<const char*>(start));
      if constexpr (meta::is_any_of<T, char16_t, Char16LE, Char16BE>)
        return details::unitlen16(reinterpret_cast<const char16_t*>(start));
      if constexpr (meta::is_any_of<T, char32_t, Char32LE, Char32BE>)
        return details::unitlen32(reinterpret_cast<const char32_t*>(start));
    }
  }

  template<typename T>
    requires(meta::CppCharType<T> || meta::CharType<T>)
  constexpr std::pair<size_t, size_t> count_and_middle(
      const T* start, size_t unit_len) noexcept
  {
    if constexpr (meta::is_any_of<T, char, char32_t, Char32BE, Char32LE>)
      return {unit_len, unit_len / 2};

    auto second = start + unit_len / 2;
    // Correct pointer
    if constexpr (meta::is_any_of<T, char8_t, Char8>)
    {
      while (is_trail(*second))
        ++second;
    }
    else
    {
      if (is_trail_surrogate(*second))
        ++second;
    }
    auto lhs                = uni::countlen(start, second - start);
    auto rhs                = uni::countlen(second, (start + unit_len) - second);
    const auto count_result = lhs + rhs;
    const auto half_count   = count_result / 2;
    if (lhs < half_count)
      second = iterator_index_front(second, half_count - lhs);
    else if (rhs < half_count)
      second = iterator_index_back(second, half_count - rhs);
    if (count_result % 2 == 0 && count_result != 0)
      second = iterator_index_back(second, 1);

    return {count_result, second - start};
  }

  template<typename T>
    requires(meta::CppCharType<T> || meta::CharType<T>)
  constexpr size_t countlen(const T* start, size_t unit_len) noexcept
  {
    assert_true("Expected non-null pointer!", start != nullptr);
    if constexpr (meta::is_any_of<T, char, char32_t, Char32BE, Char32LE>)
      return unit_len;
    if (std::is_constant_evaluated())
    {
      size_t result = 0;
      size_t index  = 0;
      // ^ this variable exist to avoid overflow with unit_len if decrementing.
      // Such overflow could happen if the last char16 was a lead surrogate
      while (index < unit_len)
      {
        size_t len = sequence_length(*start);
        index += len;
        start += len;
        ++result;
      }
      return result;
    }
    else if constexpr (std::same_as<T, Char16BE>)
      return simdutf::count_utf16be(ptr_to<const char16_t*>(start), unit_len);
    else if constexpr (std::same_as<T, Char16LE>)
      return simdutf::count_utf16le(ptr_to<const char16_t*>(start), unit_len);
    else if constexpr (std::same_as<T, char16_t>)
      return simdutf::count_utf16(ptr_to<const char16_t*>(start), unit_len);
    else if constexpr (std::same_as<T, char8_t>)
      return simdutf::count_utf8(reinterpret_cast<const char*>(start), unit_len);
    else if constexpr (std::same_as<T, Char8>)
      return simdutf::count_utf8(ptr_to<const char*>(start), unit_len);
  }

  template<typename T>
    requires(meta::CppCharType<T> || meta::CharType<T>)
  constexpr size_t strlen(const T* start) noexcept
  {
    return len(start).strlen;
  }

  template<typename T>
    requires(meta::CppCharType<T> || meta::CharType<T>)
  constexpr LenInfo len(const T* start) noexcept
  {
    assert_true("Expected non-null pointer!", start != nullptr);
    if constexpr (meta::is_any_of<T, char, char32_t, Char32BE, Char32LE>)
    {
      auto cache = uni::unitlen(start);
      return {cache, cache};
    }

    if constexpr (meta::is_any_of<T, char8_t, Char8>)
    {
      if (std::is_constant_evaluated())
      {
        const auto copy = start;
        size_t len      = 0;
        const T* end    = start;
        char8_t current;
        while ((current = static_cast<char8_t>(*end)) != u8'\0')
        {
          end += sequence_length(current);
          ++len;
        }
        return {len, static_cast<size_t>(start - copy)};
      }
      else
        return details::len8(reinterpret_cast<const char8_t*>(start));
    }
    if constexpr (meta::is_any_of<T, char16_t, Char16BE, Char16LE>)
    {
      if (std::is_constant_evaluated())
      {
        const auto copy = start;
        size_t len      = 0;
        const T* end    = start;
        char16_t current;
        while ((current = static_cast<char16_t>(*end)) != T{})
        {
          end += sequence_length(current);
          ++len;
        }
        return {len, static_cast<size_t>(start - copy)};
      }
      else
      {
        if constexpr (std::same_as<T, char16_t>)
          return details::len16(start);
        if constexpr (std::same_as<T, Char16BE>)
          return details::len16BE(reinterpret_cast<const char16_t*>(start));
        if constexpr (std::same_as<T, Char16LE>)
          return details::len16LE(reinterpret_cast<const char16_t*>(start));
      }
    }
  }
} // namespace clt::uni

namespace clt
{
  namespace meta
  {
    /// @brief Returns "" with the right char type
    /// @tparam T The char type
    /// @return "" or u8"" or u"" or U""
    template<CharType T>
    consteval auto empty_string_literal() noexcept
    {
      // We can cheat a bit as 0 is represented the same on different endianness.
      if constexpr (std::same_as<T, char>)
        return "";
      if constexpr (std::same_as<T, Char8>)
        return ""_UTF8.data();
      if constexpr (std::same_as<T, Char16LE>)
        return ""_UTF16LE.data();
      if constexpr (std::same_as<T, Char16BE>)
        return ""_UTF16BE.data();
      if constexpr (std::same_as<T, Char32LE>)
        return ""_UTF32LE.data();
      if constexpr (std::same_as<T, Char32BE>)
        return ""_UTF32LE.data();
    }
  } // namespace meta

  template<meta::CharType T, size_t SIZE>
  constexpr uni::CodePointIterator<meta::char_to_encoding_v<T>> clt::UnicodeLiteral<
      T, SIZE>::begin() const noexcept
  {
    return std::array<T, SIZE>::data();
  }

  template<meta::CharType T, size_t SIZE>
  constexpr uni::CodePointIterator<meta::char_to_encoding_v<T>> clt::UnicodeLiteral<
      T, SIZE>::end() const noexcept
  {
    return std::array<T, SIZE>::data() + std::array<T, SIZE>::size();
  }
} // namespace clt

template<clt::meta::is_any_of<clt::Char32BE, clt::Char32LE> Ty>
struct fmt::formatter<Ty>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const Ty op, FormatContext& ctx) const
  {
    using namespace clt::uni;

    // 4 char max for utf-8 + NUL terminator
    char8_t array8[clt::Char8::max_sequence + 1] = {0};
    auto from                                    = &op;
    auto result                                  = &array8[0];
    if (to_utf8(from, 1, result, 5) == ConvError::NO_ERROR)
      return fmt::format_to(ctx.out(), "{}", reinterpret_cast<const char*>(array8));
    FMT_THROW(std::runtime_error("invalid unicode"));
    clt::unreachable("Cannot be reached!");
  }
};

#endif // !HG_DSA_UNICODE
