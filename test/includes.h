#ifndef HG_TEST_INCLUDES
#define HG_TEST_INCLUDES

//#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_section_info.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_tostring.hpp>

#include <fmt/format.h>
#include <colt/uni/unicode.h>

namespace Catch
{
  template<typename T>
    requires fmt::is_formattable<T>::value
  struct is_range<T>
  {
    static const bool value = false;
  };

  template<typename T>
    requires fmt::is_formattable<T>::value
  struct StringMaker<T>
  {
    static std::string convert(T const& value) { return fmt::format("{}", value); }
  };

  template<>
  struct StringMaker<char32_t>
  {
    static std::string convert(const char32_t& value)
    {
      constexpr size_t SIZE = clt::Char8::max_sequence + 1;
      char8_t RESULT[SIZE]  = {0};
      char8_t* ptr          = RESULT;
      const clt::Char32* from = (const clt::Char32*)&value;
      clt::uni::to_utf8(from, 1, ptr, SIZE);
      return fmt::format("{}", (const char*)RESULT);
    }
  };
} // namespace Catch

#endif // !HG_TEST_INCLUDES
