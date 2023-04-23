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
  namespace details
  {
    template<meta::StringLiteral DESC>
    /// @brief Represents the description of an option
    struct Description
    {
      /// @brief Concept helper
      static constexpr bool is_desc = true;

      /// @brief The description value
      static constexpr StringView desc = DESC.value;
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
      static constexpr StringView desc = DESC.value;
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
      static constexpr StringView name = Name.value;
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

    template<typename T, typename... Ts>
    /// @brief Finds a Description type in the parameter pack, or returns Description<"">
    using find_description_t = meta::find_first_match_t<is_description, Description<"">, T, Ts...>;

    template<typename T, typename... Ts>
    /// @brief Finds a Description type in the parameter pack, or returns ValueDescription<"">
    using find_value_description_t = meta::find_first_match_t<is_value_description, ValueDescription<"">, T, Ts...>;

    template<typename T, typename... Ts>
    /// @brief Finds a Description type in the parameter pack, or returns Location<nullptr>
    using find_location_t = meta::find_first_match_t<is_location, Location<nullptr>, T, Ts...>;

    template<typename T, typename... Ts>
    /// @brief Finds a Description type in the parameter pack, or returns Alias<"">
    using find_alias_t = meta::find_first_match_t<is_alias, Alias<"">, T, Ts...>;
  }

  template<meta::StringLiteral T>
  /// @brief Adds a description for a Opt
  using desc = details::Description<T>;

  template<meta::StringLiteral T>
  /// @brief Adds a description for the value accepted by the Opt
  using value_desc = details::ValueDescription<T>;

  template<meta::StringLiteral T>
  /// @brief Adds an alias for an Opt
  using alias = details::Alias<T>;

  template<auto& REF> requires std::is_reference_v<decltype(REF)> && (!std::is_const_v<decltype(REF)>) && meta::Parsable<std::remove_reference_t<decltype(REF)>>
  /// @brief Adds the location in which to store the result for an Opt
  using location = details::Location<&REF>;  
  
  template<meta::StringLiteral Name, typename T, typename... Ts>
  /// @brief Represents an command line option
  struct Opt
  {
    /// @brief Concept helper
    static constexpr bool is_opt = true;
    
    /// @brief The name of the Opt (required, and not "")
    static constexpr StringView name = Name.value;
    /// @brief The description of the Opt (can be empty)
    static constexpr StringView desc = details::find_description_t<T, Ts...>::desc;
    /// @brief The description of the value (can be empty)
    static constexpr StringView value_desc = details::find_value_description_t<T, Ts...>::desc;
    /// @brief An alias for the Opt (can be empty)
    static constexpr StringView alias = details::find_alias_t<T, Ts...>::name;
    /// @brief The location were the result is written (never null)
    static constexpr auto location = details::find_location_t<T, Ts...>::ptr;

    static_assert(name != "", "Empty name is not allowed!");
    static_assert(location != nullptr, "cl::location<...> of the Opt must be specified!");
  };  

  namespace details
  {
    template<typename T>
    /// @brief True if Opt
    concept IsOpt = T::is_opt;

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
      return sizeof...(Args) + (!Args::alias.is_empty() + ...);
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
      return clt::max({ (Args::name.size() + (!Args::alias.is_empty()) * (Args::alias.size() + 3)) ..., 4ULL }) + 1;
    }

    template<typename... Args>
    /// @brief Computes the maximum count of chars that the value description will take.
    /// @param list The list of Opt
    /// @return Maximum count of chars
    consteval u64 max_desc_size(meta::type_list<Args...> list) noexcept
    {
      return clt::max({ Args::value_desc.size()... });
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
      //Write to the memory location
      *(opt::location) = std::move(result.data());
      //Destroy temporary storage used
      result.destruct();
      return ParseErrorCode::SUCCESS;
    }

    using parse_and_write_t = ParseErrorCode(*)(StringView) noexcept;

    template<typename... Args>
    consteval auto generate_map(meta::type_list<Args...> list) noexcept
    {
      using pair_t = std::pair<StringView, parse_and_write_t>;

      constexpr u64 map_size = opt_count(list);
      //unfiltered_array contains a pair of StringView mapping to
      //the respective callback.
      //The first half of the unfiltered_array contains Opt::name and
      //the respective callback.
      //As aliases are optional (and are represented as "" if not specified
      //by the used), we also expand them, but need another array that will
      //only hold pairs of alias that are not empty.
      std::array<pair_t, map_size * 2> unfiltered_array = {
        pair_t{ Args::name, &parse_and_write<Args>}...,
        pair_t{ Args::alias, &parse_and_write<Args>}...
      };
      //Final array that will hold non-empty Keys
      std::array<pair_t, opt_and_alias_count(list)> array;

      //Copy non-alias pair
      for (size_t i = 0; i < unfiltered_array.size() / 2; i++)
        array[i] = unfiltered_array[i];

      size_t index = 0;
      //Only copy alias pair if an alias exist (!= "")
      for (size_t i = unfiltered_array.size() / 2; i < unfiltered_array.size(); i++)
      {
        if (unfiltered_array[i].first == "")
          continue;
        array[index++] = unfiltered_array[i];
      }
      return array;
    }

    template<typename Arg>
    void print_help_for_arg(u64 max_size, u64 max_desc) noexcept
    {
      if constexpr (Arg::alias.is_empty())
        io::print("   -{: <{}}<{}>{: <{}}  - {}",
          Arg::name.data(), max_size, Arg::value_desc.data(),
          "", max_desc - Arg::value_desc.size(), Arg::desc);
      else
        io::print("   -{}, -{}{: <{}}<{}>{: <{}}  - {}",
          Arg::name.data(), Arg::alias.data(),
          "", max_size - Arg::name.size() - Arg::alias.size() - 3, Arg::value_desc.data(),
          "", max_desc - Arg::value_desc.size(), Arg::desc);
    }

    template<typename... Args>
    [[noreturn]]
    void print_help(meta::type_list<Args...> list, StringView description) noexcept
    {
      constexpr u64 max_size = max_name_size(list);
      constexpr u64 max_desc = max_desc_size(list);
      
      io::print("{}\nOPTIONS:", description);
      //Print commands in format -NAME <VALUE_DESC> - DESC aligning all options.
      (print_help_for_arg<Args>(max_size, max_desc), ...);
      //Print help command description
      io::print("   -{: <{}}{: <{}}  - {}", "help", max_size, "", max_desc + 2, "Display available options");
      std::exit(0);
    }

    void handle_non_positional(StringView arg, u64& i, u64 argc, char** argv, auto& CONST_MAP) noexcept
    {
      StringView to_parse = arg; to_parse.pop_front();
      if (auto opt = CONST_MAP.find(to_parse))
      {
        //not enough arguments...
        if (i == argc - 1)
        {
          io::print_error("'{}' expects an argument!", arg);
          std::exit(1);
        }
        //invoke callback...
        ParseErrorCode err = (**opt)(argv[++i]);
        if (err != ParseErrorCode::SUCCESS)
        {
          io::print_error("Invalid argument for '{}' option ({:h})!", arg, err);
          std::exit(1);
        }
      }
      else
      {
        io::print_error("'{}' is not an option!\nUse '-help' to enumerate possible options.", arg);
        std::exit(1);
      }
    }
  }

  template<meta::TypeList list>
  void parse_command_line_options(int argc, char** argv, StringView description = {}) noexcept
  {
    static constexpr meta::ConstexprMap CONST_MAP = details::generate_map(list{});

    for (u64 i = 1; i < static_cast<u64>(argc); i++)
    {
      StringView arg = argv[i];
      if (arg.front() == '-')
      {
        if (arg == "-help")
          details::print_help(list{}, description);
        else
          details::handle_non_positional(arg, i,
            static_cast<u64>(argc), argv, CONST_MAP);
      }
    }
  }
}

#endif //!HG_COLT_PARSE_ARGS