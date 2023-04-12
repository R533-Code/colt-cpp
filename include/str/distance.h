#ifndef HG_COLT_DISTANCE
#define HG_COLT_DISTANCE

namespace clt
{
  /// @brief constexpr version of 'strlen'
  /// @param str The string whose size to find
  /// @return The size of the string
  static constexpr size_t strlen(const char* str) noexcept
  {
    if (std::is_constant_evaluated())
    {
      size_t ret = 0;
      while (str[ret] != '\0')
        ++ret;
      return ret;
    }
    else
      return std::strlen(str);
  }
}

#endif //!HG_COLT_DISTANCE