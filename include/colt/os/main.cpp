#ifdef COLT_DEFINE_MAIN

  #include <cstdio>
  #include <colt/dsa/string_view.h>
  #include <colt/macro/config.h>
  #include <colt/mem/composable_alloc.h>

/// @brief The main function to call
/// @param args Span over the UTF8 encoded arguments
/// @return The exit code
extern int colt_main(clt::Span<const char8_t*> args);

  #ifdef COLT_WINDOWS

    #include <fcntl.h>

// On Windows, we make use of 'wmain' to obtain
// the arguments as Unicode.
int wmain(int argc, const wchar_t** argv)
{
  using namespace clt;

  // Set support to wchar_t in Console
  _setmode(_fileno(stdout), _O_WTEXT);
  _setmode(_fileno(stdin), _O_WTEXT);

  // Allocator used for needed memory to convert 'argv' to UTF8
  mem::FallbackAllocator<mem::StackAllocator<1024>, mem::Mallocator> allocator;
  // The array of 'const char8_t**'
  auto pointers = allocator.alloc(sizeof(const char8_t*) * (argc + 1));

  // The number of bytes necessary for storing UTF8 converted 'argv'.
  size_t alloc_size = 0;
  for (int i = 0; i < argc; i++)
  {
    auto ptr  = reinterpret_cast<const char16_t*>(argv[i]);
    auto size = uni::unitlen(ptr) + 1;
    // Little trick: we store the unit length as it is needed after
    reinterpret_cast<uintptr_t*>(pointers.ptr())[i] = size;
    // Compute the necessary size
    alloc_size += simdutf::utf8_length_from_utf16(ptr, size);
  }
  // The array of 'char8_t'
  auto str         = allocator.alloc(alloc_size);
  auto current_str = (char8_t*)str.ptr();
  for (int i = 0; i < argc; i++)
  {
    auto ptr     = reinterpret_cast<const char16_t*>(argv[i]);
    auto written = simdutf::convert_utf16_to_utf8(
        ptr, reinterpret_cast<uintptr_t*>(pointers.ptr())[i],
        reinterpret_cast<char*>(current_str));
    if (written == 0)
    {
      std::printf("FATAL: Argument '%i' is not valid Unicode!", i);
      return -1;
    }
    reinterpret_cast<const char8_t**>(pointers.ptr())[i] = current_str;
    current_str += written;
  }
  // Set the last argument to NULL
  reinterpret_cast<const char8_t**>(pointers.ptr())[argc] = nullptr;

  try
  {
    return colt_main(
        clt::Span{reinterpret_cast<const char8_t**>(pointers.ptr()), argc});
  }
  catch (const std::exception& e)
  {
    std::printf("FATAL: Uncaught Exception: %s", e.what());
    return -1;
  }
}

  #else

int main(int argc, const char** argv)
{
  try
  {
    return colt_main(clt::Span{reinterpret_cast<const char8_t**>(argv), argc});
  }
  catch (const std::exception& e)
  {
    std::printf("FATAL: Uncaught Exception: %s", e.what());
    return -1;
  }
}

  #endif // COLT_WINDOWS

#endif // COLT_DEFINE_MAIN
