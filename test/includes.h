#ifndef HG_TEST_INCLUDES
#define HG_TEST_INCLUDES

//#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_section_info.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_tostring.hpp>

#include <fmt/format.h>

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
    static std::string convert(T const& value)
    {
      return fmt::format("{}", value);
    }
  };
} // namespace Catch

#endif // !HG_TEST_INCLUDES
