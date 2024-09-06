#include "../includes.h"
#include <colt/io/args_parsing.h>

bool CLT_TEST = false;
bool CLT_TEST2 = false;

using namespace clt;
using namespace clt::cl;
using Args = meta::type_list<
  Opt<"test", callback<[](){ CLT_TEST = true; }>>,
  Opt<"test2", location<CLT_TEST2>>
>;

TEST_CASE("args_parsing")
{
  std::array COMMANDS = {"", "-test", "-test2=true"};
  cl::parse_command_line_options<Args>(COMMANDS.size(), COMMANDS.data());

  REQUIRE(CLT_TEST == true);
  REQUIRE(CLT_TEST2 == true);
}