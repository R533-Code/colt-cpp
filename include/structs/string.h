#ifndef HG_COLT_STRING
#define HG_COLT_STRING

#include "./vector.h"
#include "./string_view.h"
#include "../refl/enum.h"

DECLARE_ENUM_WITH_TYPE(u8, clt, StringEncoding, ASCII, UTF8, UTF32);

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
  };

  using String = BasicString<mem::GlobalAllocatorDescription, StringEncoding::ASCII>;
}

#endif //!HG_COLT_STRING