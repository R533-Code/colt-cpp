/*****************************************************************/ /**
 * @file   args_parsing.h
 * @brief  A command line argument parser.
 * The parser is generated at compile-time.
 * Opt represents an option (that starts with '-')
 * Pos represents a positional argument
 * OptPos represents an optional positional argument
 * 
 * callback<> will be called when the argument is hit.
 * location<> is the location where to write the result of parsing.
 * desc<> is the description of the option.
 * value_desc<> is the description of the value to receive by the option.
 * alias<> is an alias name for the option.
 * 
 * @code{.cpp}
 * using Args = meta::type_list<
 *   Opt<"test", callback<[](){ ... }>>,
 *   Opt<"test2", location<VAR_LOCATION>>
 * >;
 * 
 * int main(int argc, const char** argv)
 * {
 *   clt::cl::parse_command_line_options<Args>(argc, argv);
 * }
 * @endcode
 * @author RPC
 * @date   January 2024
 *********************************************************************/
#ifndef HG_COLT_PARSE_ARGS
#define HG_COLT_PARSE_ARGS

#include <string_view>

#include "colt/io/print.h"
#include "colt/io/parse.h"
#include "colt/typedefs.h"
#include "colt/meta/string_literal.h"
#include "colt/meta/traits.h"

namespace clt::cl
{
  namespace details
  {
    template<meta::StringLiteral DESC>
    /// @brief Represents the description of an option
    struct Description
    {
      /// @brief Concept helper
      static constexpr bool is_desc = true;

      /// @brief The description value
      static constexpr std::string_view desc = DESC.value;
    };

    template<typename T>
    /// @brief True if Description
    concept IsDescription = T::is_desc;

    template<typename T>
    /// @brief ::value is true if T IsDescription
    struct is_description
    {
      static constexpr bool value = IsDescription<T>;
    };

    template<meta::StringLiteral DESC>
    /// @brief Represents the description of the value accepted by an option
    struct ValueDescription
    {
      /// @brief Concept helper
      static constexpr bool is_value_desc = true;

      /// @brief The description value
      static constexpr std::string_view desc = DESC.value;
    };

    template<typename T>
    /// @brief True if ValueDescription
    concept IsValueDescription = T::is_value_desc;

    template<typename T>
    /// @brief ::value is true if T IsValueDescription
    struct is_value_description
    {
      static constexpr bool value = IsValueDescription<T>;
    };

    template<auto T>
    /// @brief Represents the location of an option
    struct Location
    {
      /// @brief Concept helper
      static constexpr bool is_location = true;

      /// @brief The location value (should be non-const pointer to Parsable)
      static constexpr auto ptr = T;
    };

    template<typename T>
    /// @brief True if Location
    concept IsLocation = T::is_location;

    template<typename T>
    /// @brief ::value is true if T IsLocation
    struct is_location
    {
      static constexpr bool value = IsLocation<T>;
    };

    template<meta::StringLiteral Name>
    /// @brief Represents an alias of an option
    struct Alias
    {
      /// @brief Concept helper
      static constexpr bool is_alias = true;

      /// @brief The alias value
      static constexpr std::string_view name = Name.value;
    };

    template<typename T>
    /// @brief True if Alias
    concept IsAlias = T::is_alias;

    template<typename T>
    /// @brief ::value is true if T IsAlias
    struct is_alias
    {
      static constexpr bool value = IsAlias<T>;
    };

    template<auto T>
    /// @brief Represents the callback of an option
    struct Callback
    {
      /// @brief Concept helper
      static constexpr bool is_callback = true;

      /// @brief The location value (should be non-const pointer to Parsable)
      static constexpr auto ptr = T;
    };

    template<typename T>
    /// @brief True if Callback
    concept IsCallback = T::is_callback;

    template<typename T>
    /// @brief ::value is true if T IsCallback
    struct is_callback
    {
      static constexpr bool value = IsCallback<T>;
    };

    template<typename T, typename... Ts>
    /// @brief Finds a Description type in the parameter pack, or returns Description<"">
    using find_description_t =
        meta::find_first_match_t<is_description, Description<"">, T, Ts...>;

    template<typename T, typename... Ts>
    /// @brief Finds a Description type in the parameter pack, or returns ValueDescription<"">
    using find_value_description_t = meta::find_first_match_t<
        is_value_description, ValueDescription<"">, T, Ts...>;

    template<typename T, typename... Ts>
    /// @brief Finds a Description type in the parameter pack, or returns Location<nullptr>
    using find_location_t =
        meta::find_first_match_t<is_location, Location<nullptr>, T, Ts...>;

    template<typename T, typename... Ts>
    /// @brief Finds a Description type in the parameter pack, or returns Alias<"">
    using find_alias_t = meta::find_first_match_t<is_alias, Alias<"">, T, Ts...>;

    template<typename T, typename... Ts>
    /// @brief Finds a Description type in the parameter pack, or returns an empty callback
    using find_callback_t =
        meta::find_first_match_t<is_callback, Callback<nullptr>, T, Ts...>;
  } // namespace details

  template<meta::StringLiteral T>
  /// @brief Adds a description for a Opt
  using desc = details::Description<T>;

  template<meta::StringLiteral T>
  /// @brief Adds a description for the value accepted by the Opt
  using value_desc = details::ValueDescription<T>;

  template<meta::StringLiteral T>
  /// @brief Adds an alias for an Opt
  using alias = details::Alias<T>;

  template<auto& REF>
    requires std::is_reference_v<decltype(REF)> && (!std::is_const_v<decltype(REF)>)
  /// @brief Adds the location in which to store the result for an Opt
  using location = details::Location<&REF>;

  template<void (*CALL)()>
  /// @brief Adds the location in which to store the result for an Opt
  using callback = details::Callback<CALL>;

  template<meta::StringLiteral GroupName, io::ANSIEffect Effect>
  struct Group
  {
    /// @brief Concept helper
    static constexpr bool is_group = true;

    /// @brief The name of the
    static constexpr std::string_view name = GroupName.value;
    /// @brief The color of the group
    static constexpr io::ANSIEffect effect = Effect;
  };

  template<meta::StringLiteral Name, typename T, typename... Ts>
  /// @brief Represents an command line option
  struct Opt
  {
    /// @brief Concept helper
    static constexpr bool is_opt = true;

    /// @brief The name of the Opt (required, and not "")
    static constexpr std::string_view name = Name.value;
    /// @brief The description of the Opt (can be empty)
    static constexpr std::string_view desc =
        details::find_description_t<T, Ts...>::desc;
    /// @brief The description of the value (can be empty)
    static constexpr std::string_view value_desc =
        details::find_value_description_t<T, Ts...>::desc;
    /// @brief An alias for the Opt (can be empty)
    static constexpr std::string_view alias = details::find_alias_t<T, Ts...>::name;
    /// @brief The location were the result is written (can be null if callback is not null)
    static constexpr auto location = details::find_location_t<T, Ts...>::ptr;
    /// @brief The callback to call upon detection (can be null if location is not null)
    static constexpr auto callback = details::find_callback_t<T, Ts...>::ptr;

    static_assert(name != "", "Empty name is not allowed!");
    static_assert(
        name.find('=') == std::string_view::npos, "'=' not allowed in Opt name!");
    static_assert(
        location != nullptr || callback != nullptr,
        "Either cl::location<...> or cl::callback<> of the Opt must be specified!");
  };

  template<meta::StringLiteral Name, typename T, typename... Ts>
  /// @brief Represents a positional argument
  struct Pos
  {
    /// @brief Concept helper
    static constexpr bool is_pos = true;

    /// @brief The name of the Opt (required, and not "")
    static constexpr std::string_view name = Name.value;
    /// @brief The description of the Opt (can be empty)
    static constexpr std::string_view desc =
        details::find_description_t<T, Ts...>::desc;
    /// @brief The location were the result is written (can be null if callback is not null)
    static constexpr auto location = details::find_location_t<T, Ts...>::ptr;
    /// @brief The callback to call upon detection (can be null if location is not null)
    static constexpr auto callback = details::find_callback_t<T, Ts...>::ptr;

    static_assert(name != "", "Empty name is not allowed!");
    static_assert(
        location != nullptr || callback != nullptr,
        "Either cl::location<...> or cl::callback<> of the Pos must be specified!");
  };

  template<meta::StringLiteral Name, typename T, typename... Ts>
  /// @brief Represents an optional positional argument
  struct OptPos
  {
    /// @brief Concept helper
    static constexpr bool is_optpos = true;

    /// @brief The name of the Opt (required, and not "")
    static constexpr std::string_view name = Name.value;
    /// @brief The description of the Opt (can be empty)
    static constexpr std::string_view desc =
        details::find_description_t<T, Ts...>::desc;
    /// @brief The location were the result is written (can be null if callback is not null)
    static constexpr auto location = details::find_location_t<T, Ts...>::ptr;
    /// @brief The callback to call upon detection (can be null if location is not null)
    static constexpr auto callback = details::find_callback_t<T, Ts...>::ptr;

    static_assert(name != "", "Empty name is not allowed!");
    static_assert(
        location != nullptr || callback != nullptr,
        "Either cl::location<...> or cl::callback<> of the OptPos must be "
        "specified!");
  };

  namespace details
  {
    template<typename T>
    /// @brief True if Opt
    concept IsOpt = T::is_opt;
    
    template<typename T>
    /// @brief True if Opt
    concept IsGroup = T::is_group;

    template<typename T>
    struct is_opt
    {
      static constexpr bool value = IsOpt<T>;
    };
    
    template<typename T>
    struct is_group
    {
      static constexpr bool value = IsGroup<T>;
    };

    template<typename T>
    struct is_opt_or_group
    {
      static constexpr bool value = IsGroup<T> || IsOpt<T>;
    };

    template<typename T>
    /// @brief True if Opt
    concept IsPos = T::is_pos;

    template<typename T>
    struct is_pos
    {
      static constexpr bool value = IsPos<T>;
    };

    template<typename T>
    /// @brief True if Opt
    concept IsOptPos = T::is_optpos;

    template<typename T>
    struct is_optpos
    {
      static constexpr bool value = IsOptPos<T>;
    };

    template<typename... Args>
    /// @brief Counts the number of Opt specified in 'list'
    /// @param list The list of Opt
    /// @return Count of Opt
    consteval u64 opt_count(meta::type_list<Args...> list) noexcept
    {
      //For now only return the size of the list.
      //When categories are implemented, we need to
      //recurse into each category, skipping positional arguments.
      return sizeof...(Args);
    }

    template<typename... Args>
    /// @brief Counts the number of Opt and their aliases specified in 'list'
    /// @param list The list of Opt
    /// @return Count of Opt + non-empty aliases
    consteval u64 opt_and_alias_count(meta::type_list<Args...> list) noexcept
    {
      //For now only return the size of the list.
      //When categories are implemented, we need to
      //recurse into each category, skipping positional arguments.
      return sizeof...(Args) + (!Args::alias.empty() + ...);
    }

    template<typename... Args>
    /// @brief Computes the maximum count of chars that the name and alias will take.
    /// Takes into account "help", and for aliases adds 3 to represent ", -".
    /// Adds one to the result to take into account the "-" for the name.
    /// @param list The list of Opt
    /// @return Maximum count of chars
    consteval u64 max_name_size(meta::type_list<Args...> list) noexcept
    {
      // 4ULL for "help"
      // + 3ULL is for ", -" between name and alias
      // + 1ULL is for "-"
      return clt::max(
                 {(static_cast<size_t>(Args::name.size())
                   + static_cast<size_t>(
                       (!Args::alias.empty()) * (Args::alias.size() + 3)))...,
                  static_cast<size_t>(4)})
             + 1;
    }

    template<typename... Args>
    /// @brief Computes the maximum count of chars that the value description will take.
    /// @param list The list of Opt
    /// @return Maximum count of chars
    consteval u64 max_desc_size(meta::type_list<Args...> list) noexcept
    {
      return clt::max({Args::value_desc.size()..., static_cast<size_t>(2)}) + 2ULL;
    }

    template<IsOpt opt>
    ParsingResult parse_opt(std::string_view strv) noexcept
    {
      using ResultType =
          std::remove_cvref_t<std::remove_pointer_t<decltype(opt::location)>>;

      if constexpr (opt::location != nullptr)
      {
        auto res = clt::parse<ResultType>(strv);
        if (res)
          *opt::location = std::move(*res);
        else
          return res.error();
      }

      //Run callback if it exists.
      //The callback is only run if parsing was successful.
      if constexpr (opt::callback != nullptr)
        (*opt::callback)();

      return clt::ParsingResult{};
    }

    template<typename opt>
    ParsingResult parse_pos(std::string_view strv) noexcept
    {
      using ResultType =
          std::remove_cvref_t<std::remove_pointer_t<decltype(opt::location)>>;

      if constexpr (opt::location != nullptr)
      {
        auto res = clt::parse<ResultType>(strv);
        if (res)
          *opt::location = std::move(*res);
        else
          return res.error();
      }

      //Run callback if it exists.
      //The callback is only run if parsing was successful.
      if constexpr (opt::callback != nullptr)
        (*opt::callback)();

      return clt::ParsingResult{};
    }

    using parse_and_write_t = ParsingResult (*)(std::string_view) noexcept;

    template<typename... Args>
    consteval auto generate_opt_table(meta::type_list<Args...> list) noexcept
    {
      using pair_t = std::pair<std::string_view, std::pair<bool, parse_and_write_t>>;

      constexpr u64 opt_size = opt_count(list);
      //unfiltered_array contains a pair of std::string_view mapping to
      //the respective callback.
      //The first half of the unfiltered_array contains Opt::name and
      //the respective callback.
      //As aliases are optional (and are represented as "" if not specified
      //by the used), we also expand them, but need another array that will
      //only hold pairs of alias that are not empty.
      std::array<pair_t, opt_size * 2> unfiltered_array = {
          pair_t{Args::name, {Args::location == nullptr, &parse_opt<Args>}}...,
          pair_t{Args::alias, {Args::location == nullptr, &parse_opt<Args>}}...};
      //Final array that will hold non-empty Keys
      std::array<pair_t, opt_and_alias_count(list)> array;

      //The first half of unfiltered_array contains valid pairs,
      //so copy them.
      //The second half contains the "alias" pairs.
      //Half the size of unfiltered_array is 'opt_size'.

      //Copy non-alias pair
      for (size_t i = 0; i < opt_size; i++)
        array[i] = unfiltered_array[i];

      size_t index = opt_size;
      //Only copy alias pair if an alias exist (!= "")
      for (size_t i = opt_size; i < opt_size * 2; i++)
      {
        if (unfiltered_array[i].first == "")
          continue;
        array[index++] = unfiltered_array[i];
      }
      return array;
    }

    template<typename... Args, typename... Args2>
    consteval auto generate_pos_table(
        meta::type_list<Args...>, meta::type_list<Args2...>) noexcept
    {
      return std::array<parse_and_write_t, sizeof...(Args) + sizeof...(Args2)>{
          &parse_pos<Args>..., &parse_pos<Args2>...};
    }

    template<typename Arg>
    void print_help_for_arg(u64 max_size, u64 max_desc) noexcept
    {
      if constexpr (details::IsGroup<Arg>)
      {
        print("\n {}{}:{}", Arg::effect, Arg::name, io::Reset);
      }
      else
      {
        if constexpr (Arg::alias.empty())
        {
          auto offset = Arg::name.find_first_not_of("-");
          print<"">(
              "   -{}{}{: <{}}{}", Arg::name.substr(0, offset), io::BrightCyanF,
              Arg::name.substr(offset), max_size - offset, io::Reset);
        }
        else
        {
          auto offset  = Arg::name.find_first_not_of("-");
          auto offset2 = Arg::alias.find_first_not_of("-");
          print<"">(
              "   -{}{}{}{}, -{}{}{}{}{: <{}}", Arg::name.substr(0, offset),
              io::BrightCyanF, Arg::name.substr(offset), io::Reset,
              Arg::alias.substr(0, offset2), io::BrightCyanF,
              Arg::alias.substr(offset2), io::Reset, "",
              max_size - Arg::name.size() - Arg::alias.size() - 3);
        }

        if constexpr (Arg::value_desc.empty())
          print<"">("{: <{}}", "", max_desc);
        else
          print<"">(
              "{}<{}>{}{: <{}}", io::BrightMagentaF, Arg::value_desc.data(),
              io::Reset, "", max_desc - Arg::value_desc.size() - 2);

        if constexpr (Arg::desc.empty())
          print("");
        else
          print("  - {}", Arg::desc);
      }      
    }

    template<typename Arg>
    void print_help_for_pos() noexcept
    {
      print<" ">("<{}>", Arg::name);
    }

    template<typename Arg>
    void print_help_for_optpos() noexcept
    {
      print<" ">("<{}>?", Arg::name);
    }

    template<typename... Args, typename... Args2, typename... Args3, typename... Args4>
    [[noreturn]] void print_help(
        meta::type_list<Args...> list, meta::type_list<Args2...> pos,
        meta::type_list<Args3...> optpos, meta::type_list<Args4...> optgroup, std::string_view name,
        std::string_view description) noexcept
    {
      constexpr u64 max_size = max_name_size(list);
      constexpr u64 max_desc = max_desc_size(list);

      if (name.empty())
        print<"">("USAGE: {}[OPTIONS] {}", io::BrightCyanF, io::BrightBlueF);
      else
        print<"">(
            "USAGE: {} {}[OPTIONS] {}", name, io::BrightCyanF, io::BrightBlueF);
      (print_help_for_pos<Args2>(), ...);
      print<"">("{}", io::GreenF);
      (print_help_for_optpos<Args3>(), ...);
      print("{}\n   {}\n\nOPTIONS:", io::Reset, description);
      //Print help command description
      print(
          "   -{}{: <{}}{}{: <{}}  - {}", io::BrightCyanF, "help", max_size,
          io::Reset, "", max_desc, "Display available options");
      //Print commands in format -NAME <VALUE_DESC> - DESC aligning all options.
      (print_help_for_arg<Args4>(max_size, max_desc), ...);
      std::exit(0);
    }

    void handle_non_positional(
        std::string_view arg, u64& i, u64 argc, const char** argv,
        auto& CONST_MAP) noexcept
    {
      arg.remove_prefix(1); // pop '-'
      std::string_view to_parse = arg;
      u64 equal_index           = to_parse.find('=');
      to_parse                  = to_parse.substr(0, equal_index);
      if (auto opt = CONST_MAP.find(to_parse))
      {
        if ((*opt).first == true)
        {
          if (equal_index != std::string_view::npos)
            print_warn("'{}' does not expect an argument ('{}')!", to_parse, arg.substr(equal_index));
          (*(*opt).second)({});
          return;
        }

        //not enough arguments...
        if (equal_index == std::string_view::npos && i == argc - 1)
        {
          print_error("'{}' expects an argument!", arg);
          std::exit(1);
        }
        //invoke callback...
        ParsingResult err;
        if (equal_index == std::string_view::npos)
          err = (*(*opt).second)(argv[++i]);
        else
          err = (*(*opt).second)(arg.substr(equal_index + 1));

        if (err != ParsingCode::GOOD)
        {
          print_error("Invalid argument for '{}' option ({:h})!", arg, err);
          std::exit(1);
        }
      }
      else
      {
        print_error(
            "'-{}' is not an option!\nUse '-help' to enumerate possible options.",
            arg);
        std::exit(1);
      }
    }

    void handle_positional(
        std::string_view arg, u64& pos_id, auto& POS_TABLE) noexcept
    {
      if (pos_id == POS_TABLE.size())
      {
        print_warn("Unused argument '{}'!", arg);
        return;
      }
      auto opt = POS_TABLE[pos_id++];
      //invoke callback...
      ParsingResult err = (*opt)(arg);
      if (err != ParsingCode::GOOD)
      {
        print_error("Invalid argument for '{}' option ({:h})!", arg, err);
        std::exit(1);
      }
    }
  } // namespace details

  template<meta::TypeList list>
  void parse_command_line_options(
      int argc, const char** argv, std::string_view name = {},
      std::string_view description = {}) noexcept
  {
    using OptList      = typename list::template remove_if_not<details::is_opt>;
    using OptGroupList = typename list::template remove_if_not<details::is_opt_or_group>;
    using PosList      = typename list::template remove_if_not<details::is_pos>;
    using OptPosList   = typename list::template remove_if_not<details::is_optpos>;

    //Positional argument table, contains pointers to the function to call
    //when a non-positional argument is detected.
    static constexpr meta::Map CONST_MAP = details::generate_opt_table(OptList{});
    //Positional argument table, contains pointers to the function to call
    //when a positional argument is detected.
    static constexpr auto POS_TABLE =
        details::generate_pos_table(PosList{}, OptPosList{});

    u64 pos_id          = 0;
    bool is_parsing_pos = false;
    for (u64 i = 1; i < static_cast<u64>(argc); i++)
    {
      std::string_view arg = argv[i];
      if (arg.empty() || arg.front() != '-' || is_parsing_pos)
        details::handle_positional(arg, pos_id, POS_TABLE);
      else
      {
        if (arg == "--")
        {
          is_parsing_pos = true;
          continue;
        }

        if (arg == "-help")
          details::print_help(
              OptList{}, PosList{}, OptPosList{}, OptGroupList{}, name,
              description);
        else
          details::handle_non_positional(
              arg, i, static_cast<u64>(argc), argv, CONST_MAP);
      }
    }
    if (pos_id < PosList::size)
    {
      print_error(
          "Not enough arguments provided! {} missing.", PosList::size - pos_id);
      std::exit(1);
    }
  }

  template<meta::TypeList list>
  void parse_command_line_options(
      Span<const char8_t*> args, std::string_view name = {},
      std::string_view description = {}) noexcept
  {
    return parse_command_line_options<list>(
        (int)args.size(), (const char**)args.data(), name, description);
  }
} // namespace clt::cl

#endif //!HG_COLT_PARSE_ARGS