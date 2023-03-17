#ifndef HG_COLT_STRING_LITERAL
#define HG_COLT_STRING_LITERAL

namespace clt::meta
{
  template <size_t N>
  struct StringLiteral
  {
    constexpr StringLiteral(const char (&str)[N])
    {
      std::copy_n(str, N, value);
    }

    char value[N];
  };
}

#endif //!HG_COLT_STRING_LITERAL