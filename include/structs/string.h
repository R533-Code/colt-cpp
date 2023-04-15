#ifndef HG_COLT_STRING
#define HG_COLT_STRING

#include "./vector.h"
#include "./string_view.h"
#include "./expect.h"
#include "../refl/enum.h"


DECLARE_ENUM_WITH_TYPE(u8, clt::io, IOError, FILE_EOF, FILE_ERROR, INVALID_FMT, OUT_OF_RANGE);

namespace clt
{
  template<auto ALLOCATOR, StringEncoding ENCODING>
    requires meta::AllocatorScope<ALLOCATOR>
  class BasicString{};

  template<auto ALLOCATOR>
    requires meta::AllocatorScope<ALLOCATOR>
  class BasicString<ALLOCATOR, StringEncoding::ASCII>
    : public Vector<char, ALLOCATOR>
  {
    using UnderlyingVector = Vector<char, ALLOCATOR>;

    /// @brief True if the allocator is global
    static constexpr bool is_global = meta::GlobalAllocator<ALLOCATOR>;
    /// @brief True if the allocator is local (we need a reference to it)
    static constexpr bool is_local = meta::LocalAllocator<ALLOCATOR>;

  public:
    template<typename AllocT> requires is_local
    constexpr BasicString(AllocT& alloc) noexcept
      : UnderlyingVector(alloc) {}

    constexpr BasicString() noexcept requires is_global = default;

    template<typename AllocT>
    constexpr BasicString(AllocT& alloc, StringView strv) noexcept requires is_local
      : UnderlyingVector(alloc, strv) {}

    constexpr BasicString(StringView strv) noexcept requires is_global
      : UnderlyingVector(strv) {}

    template<typename AllocT, size_t N> requires is_local
      constexpr BasicString(AllocT& alloc, const char(&x)[N]) noexcept
      : UnderlyingVector(alloc, StringView{ x, x + N }) {}

    template<size_t N> requires is_global
    constexpr BasicString(const char(&x)[N]) noexcept
      : UnderlyingVector(StringView{ x, x + N }) {}

    /// @brief Converts a String to a StringView
    /// @return Span over the whole Vector
    constexpr operator StringView() noexcept
    {
      return StringView{ this->begin(), this->end() };
    }

    static Expect<BasicString, io::IOError> getLine(FILE* from, u64 reserve = 64, bool strip_front = true) noexcept
    {
      BasicString str;
      auto gchar = std::fgetc(from);
      if (gchar == EOF)
      {
        if (feof(from))
          return { Error, IOError::FILE_EOF };
        else
          return { Error, IOError::FILE_ERROR };
      }
      str.reserve(reserve);
      if (strip_front && std::isspace(gchar))
      {
        //Consume spaces
        while ((gchar = std::fgetc(from)) != EOF)
          if (!std::isspace(gchar))
            break;
      }
      for (;;)
      {
        if (gchar != '\n' && gchar != EOF)
        {
          str.push_back(static_cast<char>(gchar));
          gchar = std::fgetc(from);
        }
        else
          break;
      }
      return str;
    }

    static Expect<BasicString, io::IOError> getLine(u64 reserve = 64, bool strip_front = true) noexcept
    {
      return getLine(stdin, reserve, strip_front);
    }
  };

  using String = BasicString<mem::GlobalAllocatorDescription, StringEncoding::ASCII>;
}

#endif //!HG_COLT_STRING