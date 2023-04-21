#ifndef HG_COLT_PARSE_ARGS
#define HG_COLT_PARSE_ARGS

#include "../io/print.h"
#include "../structs/string.h"
#include "../meta/string_literal.h"
#include "../meta/type_list.h"
#include "../str/distance.h"
#include "../str/parse.h"
#include "../refl/enum.h"

namespace clt::cl
{
  template<auto PTR, meta::StringLiteral Name, meta::StringLiteral Description, meta::StringLiteral ValueDesc>
    requires (std::is_pointer_v<decltype(PTR)>) && meta::Parsable<std::remove_cv_t<std::remove_pointer_t<decltype(PTR)>>>
  struct Opt
  {
    static constexpr bool is_opt = true;
    static constexpr StringView name = Name.value;
    static constexpr StringView desc = Description.value;
    static constexpr StringView value_desc = ValueDesc.value;
    static constexpr decltype(PTR) location = PTR;
  };

  template<meta::StringLiteral Name>
  struct Alias
  {
    static constexpr bool is_alias = true;
    static constexpr StringView name = Name.value;
  };

  namespace details
  {
    template<typename T>
    concept IsOpt = T::is_opt;

    template<typename T>
    concept IsAlias = T::is_alias;

    template<meta::TypeList list>
    consteval u64 recursively_get_opt_count() noexcept
    {
      //For now only return the size of the list.
      //When categories are implemented, we need to
      //recurse into each category, skipping positional arguments.
      return list::size;
    }

    template<IsOpt opt>
    ParseErrorCode parse_and_write(StringView strv) noexcept
    {
      using ResultType = std::remove_cvref_t<std::remove_pointer_t<decltype(opt::location)>>;
      //The variable in which to store the result
      uninit<ResultType> result;
      
      //Parse the line
      auto [ptr, err] = clt::str::parser<ResultType>{}(result, strv);
      if (err != ParseErrorCode::SUCCESS)
        return err;
      *(opt::location) = std::move(result.data());
      result.destruct();
      return ParseErrorCode::SUCCESS;
    }

    using parse_and_write_t = ParseErrorCode(*)(StringView) noexcept;

    template<typename... Args>
    consteval auto generate_array(meta::type_list<Args...>) noexcept
    {
      using pair_t = std::pair<StringView, parse_and_write_t>;

      constexpr u64 map_size = recursively_get_opt_count<meta::type_list<Args...>>();
      std::array<pair_t, map_size> array = {
        pair_t{ Args::name, &parse_and_write<Args>}...
      };
      return array;
    }

    void handle_non_positional(StringView arg, u64& i, u64 argc, char** argv, auto& CONST_MAP) noexcept
    {
      StringView to_parse = arg; to_parse.pop_front();
      if (auto opt = CONST_MAP.find(to_parse))
      {
        io::print_message("'{}' is an option!", arg);
        //not enough arguments...
        if (i == argc - 1)
          return io::print_fatal("'{}' expects an option!", arg);
        //invoke callback...
        ParseErrorCode err = (**opt)(argv[++i]);
        if (err != ParseErrorCode::SUCCESS)
        {
          io::print_error("Invalid argument for '{}' option ({:h})!", arg, err);
          std::exit(1);
        }
      }
      else
        io::print_error("'{}' is not an option!", arg);
    }
  }

  template<meta::TypeList list>
  void parse_command_line_options(int argc, char** argv) noexcept
  {
    static constexpr meta::ConstexprMap CONST_MAP = details::generate_array(list{});
    auto argn = static_cast<u64>(argc);
    for (u64 i = 1; i < argn; i++)
    {
      StringView arg = argv[i];
      if (arg.front() == '-')
      {
        if (arg == '-help')
          details::print_help<list>();
        details::handle_arg(arg, i, argc, argv, CONST_MAP);
      }
    }
  }
}

#endif //!HG_COLT_PARSE_ARGS