/*****************************************************************/ /**
 * @file   test_dynamic_lib.cpp
 * @brief  Unit tests for `DynamicLib`
 * 
 * @author RPC
 * @date   August 2024
 *********************************************************************/
#include "../includes.h"
#include <colt/os/mmap_file.h>


TEST_CASE("ViewOfFile")
{
  using namespace clt;
  using namespace clt::os;
  
  const char* text = "This is a test of ViewOfFile!";
  View<u8> view    = {(const u8*)text, uni::strlen(text)};

  auto file = fopen("test.txt", "w");
  REQUIRE(file != nullptr);
  fputs(text, file);
  fclose(file);

  auto current = ViewOfFile::open("test.txt");
  REQUIRE(current.is_value());
  REQUIRE(std::memcmp(current->view()->data(), view.data(), view.size()) == 0);
}