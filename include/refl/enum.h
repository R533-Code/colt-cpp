#ifndef HG_COLT_ENUM
#define HG_COLT_ENUM

#include <array>
#include <utility>

#include "./refl.h"
#include "../meta/map.h"
#include "../structs/string_view.h"
#include "../util/macro.h"
#include "../util/contracts.h"
#include "../util/typedefs.h"

namespace clt::iter
{  
  template<u64 begin, u64 end>
  class RangeIterator
  {
    u64 current = begin;

  public:
    constexpr RangeIterator() noexcept = default;

    constexpr RangeIterator(u64 current) noexcept
      : current(current) {}

    /// @brief Reads the current value of the range
    /// @return The current value of the range
    constexpr u64 operator*() const noexcept { return current; }
    
    /// @brief Advances the iterator to the next value in the range
    /// @return Self
    constexpr RangeIterator& operator++() noexcept
    {
      ++current; return *this;
    }

    constexpr RangeIterator operator++(int) noexcept 
    {
      RangeIterator tmp = *this; ++(*this); return tmp;
    }

    constexpr RangeIterator& operator--() noexcept
    {
      --current; return *this;
    }

    constexpr RangeIterator operator--(int) noexcept
    {
      RangeIterator tmp = *this; --(*this); return tmp;
    }

    friend constexpr bool operator==(const RangeIterator&, const RangeIterator&) = default;
  };
  template<typename To, typename Iter>
  class ConvertIterator
  {
    Iter iterator;

  public:
    constexpr ConvertIterator() noexcept = default;

    template<typename U, typename... Args> requires (!std::same_as<ConvertIterator, std::remove_cvref_t<U>>)
    constexpr ConvertIterator(U&& ref, Args&&... args) noexcept
      : iterator(std::forward<U>(ref), std::forward<Args>(args)...) {}

    constexpr To operator*() const noexcept { return static_cast<To>(*iterator); }

    constexpr ConvertIterator& operator++() noexcept
    {
      ++iterator; return *this;
    }

    constexpr ConvertIterator operator++(int) noexcept
    {
      ConvertIterator tmp = *this; ++(*this); return tmp;
    }

    constexpr ConvertIterator& operator--() noexcept
    {
      --iterator; return *this;
    }

    constexpr ConvertIterator operator--(int) noexcept
    {
      ConvertIterator tmp = *this; --(*this); return tmp;
    }

    friend constexpr bool operator==(const ConvertIterator&, const ConvertIterator&) = default;
  };

  template<typename Enum, u64 BEGIN, u64 END>
  struct EnumIter
  {
    constexpr ConvertIterator<Enum, RangeIterator<BEGIN, END>> begin() const noexcept { return {}; }
    constexpr ConvertIterator<Enum, RangeIterator<BEGIN, END>> end()   const noexcept { return END + 1; }
  };
}

#define COLT_DETAILS_EXPAND_ENUM(en)    , en
#define COLT_DETAILS_STRINGIZE_ENUM(en) , clt::StringView{ #en }
#define COLT_DETAILS_MAP_PAIR_ENUM(en)  , std::pair{ clt::StringView{ #en }, en }

/// @brief Declares an enumeration with reflection support
#define DECLARE_ENUM_WITH_TYPE(type, namespace_name, name, first, ...) \
  namespace namespace_name { \
  enum class name : type { \
    first \
    COLT_FOR_EACH(COLT_DETAILS_EXPAND_ENUM, __VA_ARGS__) \
  }; } \
  template<> \
  struct clt::refl<name> { \
    using enum_type = type; \
    static constexpr clt::StringView str() noexcept { return #name; } \
    static constexpr bool is_consecutive() noexcept { return true; } \
    static constexpr std::array name##_str = { clt::StringView{ #first} COLT_FOR_EACH(COLT_DETAILS_STRINGIZE_ENUM, __VA_ARGS__) }; \
    static constexpr size_t min() noexcept { return 0; }  \
    static constexpr size_t max() noexcept { return name##_str.size() - 1; }  \
    static constexpr size_t count() noexcept { return name##_str.size(); } \
    static constexpr clt::StringView str(name value) { \
      COLT_PRE(static_cast<type>(value) <= max()) \
        return name##_str[static_cast<type>(value)]; \
      COLT_POST() \
    } \
    static constexpr clt::Option<name> from(type value) {\
      if (value > max()) \
        return clt::None; \
      return static_cast<name>(value); \
    } \
  private: \
    using ArrayTable_t = std::array<std::pair<clt::StringView, name>, name##_str.size()>; \
    using Map_t = clt::meta::ConstexprMap<clt::StringView, name, name##_str.size()>; \
    static constexpr ArrayTable_t get_array() { \
        using enum name; \
        ArrayTable_t ret = { \
          std::pair{ clt::StringView{ #first}, first } \
          COLT_FOR_EACH(COLT_DETAILS_MAP_PAIR_ENUM, __VA_ARGS__) }; \
        return ret; \
      } \
    static const ArrayTable_t internal_map; \
    static const Map_t map; \
  public: \
    static constexpr clt::Option<name> from(clt::StringView str) {\
      return map.find(str); \
    } \
    static constexpr clt::iter::EnumIter<name, 0, name##_str.size() - 1> iter() noexcept { return {}; } \
  }; \
  constexpr refl<name>::ArrayTable_t refl<name>::internal_map = refl<name>::get_array(); \
  constexpr refl<name>::Map_t refl<name>::map =  {{ refl<name>::internal_map }}

#define DECLARE_ENUM(namespace_name, name, first, ...) \
  namespace namespace_name { \
  enum class name { \
    first \
    COLT_FOR_EACH(COLT_DETAILS_EXPAND_ENUM, __VA_ARGS__) \
  }; } \
  template<> \
  struct clt::refl<name> { \
    using enum_type = std::underlying_type_t<name>; \
    static constexpr clt::StringView str() noexcept { return #name; } \
    static constexpr bool is_consecutive() noexcept { return true; } \
    static constexpr std::array name##_str = { clt::StringView{ #first} COLT_FOR_EACH(COLT_DETAILS_STRINGIZE_ENUM, __VA_ARGS__) }; \
    static constexpr size_t min() noexcept { return 0; }  \
    static constexpr size_t max() noexcept { return name##_str.size() - 1; }  \
    static constexpr size_t count() noexcept { return name##_str.size(); } \
    static constexpr clt::StringView str(name value) { \
      COLT_PRE(static_cast<enum_type>(value) <= max()) \
        return name##_str[static_cast<enum_type>(value)]; \
      COLT_POST() \
    } \
    static constexpr clt::Option<name> from(enum_type value) {\
      if (value > max()) \
        return clt::None; \
      return static_cast<name>(value); \
    } \
  private: \
    using ArrayTable_t = std::array<std::pair<clt::StringView, name>, name##_str.size()>; \
    using Map_t = clt::meta::ConstexprMap<clt::StringView, name, name##_str.size()>; \
    static constexpr ArrayTable_t get_array() { \
        using enum name; \
        ArrayTable_t ret = { \
          std::pair{ clt::StringView{ #first}, first } \
          COLT_FOR_EACH(COLT_DETAILS_MAP_PAIR_ENUM, __VA_ARGS__) }; \
        return ret; \
      } \
    static const ArrayTable_t internal_map; \
    static const Map_t map; \
  public: \
    static constexpr clt::Option<name> from(clt::StringView str) {\
      return map.find(str); \
    } \
    static constexpr clt::iter::EnumIter<name, 0, name##_str.size() - 1> iter() noexcept { return {}; } \
  }; \
  constexpr refl<name>::ArrayTable_t refl<name>::internal_map = refl<name>::get_array(); \
  constexpr refl<name>::Map_t refl<name>::map =  {{ refl<name>::internal_map }}

template<typename T>
  requires std::is_enum_v<T> && clt::meta::Reflectable<T>
struct fmt::formatter<T>
{
  bool human_readable = false;

  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    auto end = ctx.end();
    if (it == end)
      return it;
    if (*it == 'h')
    {
      ++it;
      human_readable = true;
    }
    assert_true("Possible format for Enum are: {} or {:h}!", *it == '}');
    return it;
  }

  template<typename FormatContext>
  auto format(const T& exp, FormatContext& ctx)
  {
    if (human_readable)
      return fmt::format_to(ctx.out(), "{}", refl<T>::str(exp));
    return fmt::format_to(ctx.out(), "{}::{}", refl<T>::str(), refl<T>::str(exp));
  }
};

#endif //!HG_COLT_ENUM