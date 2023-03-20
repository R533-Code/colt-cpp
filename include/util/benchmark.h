#ifndef HG_COLT_BENCHMARK
#define HG_COLT_BENCHMARK

#include <thread>
#include <cstdio>
#include <atomic>
#include <mutex>
#include <chrono>
#include <charconv>
//TODO: use better data structure
#include <vector>
#include <unordered_map>

#include "./typedefs.h"
#include "../meta/string_literal.h"
#include "./macro.h"
#include "./on_exit.h"

namespace clt::bench
{
  /// @brief Time point type from steady_clock
  using time_point = std::chrono::time_point<std::chrono::steady_clock>;
  /// @brief Duration type from steady_clock
  using duration = std::chrono::steady_clock::duration;
  namespace tm = std::chrono;

  /// @brief Timer class, records elapsed time since construction
  class Timer
  {
    /// @brief The start time
    time_point start_time = tm::steady_clock::now();

  public:    
    /// @brief Records current time as beginning of recording
    void restart() noexcept
    {
      start_time = tm::steady_clock::now();
    }

    /// @brief Returns the start time of the recording
    /// @return The start time of the recording
    time_point get_start_time() const noexcept { return start_time; }

    /// @brief Returns the elapsed time since constructor
    /// @return The elapsed time
    duration get_elapsed_time() const noexcept
    {
      return tm::steady_clock::now() - start_time;
    }
  };

  /// @brief Source code and runtime informations
  struct ProfileSource
  {
    /// @brief The function name
    const char* fn_name;
    /// @brief The file name
    const char* file_name;
    /// @brief The line number
    u32 line_number;    
  };

  /// @brief Saved benchmarking snapshot
  struct ProfileResult
  {
    /// @brief The source from which the profile come
    const ProfileSource& src;
    /// @brief The start time of the snapshot
    time_point start_time;
    /// @brief The duration of the snapshot
    duration duration;
    /// @brief The thread ID
    u64 thread_id;
  };

  /// @brief Class responsible of storing profiles results
  class Instrumentator
  {
    /// @brief Vector to store the ProfileResult
    std::vector<ProfileResult> profiles {};
    /// @brief Mutex to avoid data-races
    mutable std::mutex mtx {};

    template<meta::StringLiteral lit>
    /// @brief Writes a profile to a file in the "chrome://tracing" format
    /// @tparam lit Literal string to append before format string
    /// @param file The file to which to write
    /// @param start Buffer where to write the start time as a string
    /// @param durat Buffer where to write the duration as a string
    /// @param profile The profile to write
    void write_profile(FILE* file, char (*start)[24], char (*durat)[24], const ProfileResult& profile) const noexcept
    {
      *std::to_chars(*start, *start + 24,
        tm::time_point_cast<tm::milliseconds>(profile.start_time).time_since_epoch().count()).ptr = '\0';
      *std::to_chars(*durat, *durat + 24,
        tm::duration_cast<tm::milliseconds>(profile.duration).count()).ptr = '\0';
      
      std::fprintf(file,
        meta::join_v<lit,
        "{"
        R"("cat": "function", )"
        R"("dur": %s, )"
        R"("name": "%s", )"
        R"("ph": "X", )"
        R"("pid": 0, )"
        R"("tid": "%llu", )"
        R"("ts": %s })">,
        *durat, profile.src.fn_name,
        profile.thread_id, *start);
    }

  public:
    // Default constructor
    Instrumentator() = default;
    // No copy constructor
    Instrumentator(const Instrumentator&) = delete;
    // No move constructor
    Instrumentator(Instrumentator&&) = delete;

    /// @brief Saves a profile result
    /// @param src The source of the profile
    /// @param tm The start time
    /// @param dur The duration
    /// @param thread_id The thread ID
    void save_profile(const ProfileSource& src, time_point tm, duration dur, std::thread::id thread_id = std::this_thread::get_id()) noexcept
    {
      auto lck = std::scoped_lock(mtx);
      profiles.emplace_back(src, tm, dur, std::hash<std::thread::id>{}(thread_id));
    }

    /// @brief Writes saved profile in the "chrome://tracing" format to file at 'path'
    /// @param path The path name (preferably ending with .json)
    /// @return True if successful
    bool write_tracing(const char* path) const noexcept
    {
      auto lck = std::scoped_lock(mtx);
      auto file = fopen(path, "wt");
      if (!file)
        return false;
      
      std::fputs(R"({"otherData": {},"traceEvents":[)", file);
      char start[24];
      char durat[24];
      if (!profiles.empty())
        write_profile<"">(file, &start, &durat, profiles.front());
      for (size_t i = 1; i < profiles.size(); i++)
        write_profile<",">(file, &start, &durat, profiles[i]);

      std::fputs("]}", file);
      std::fclose(file);
      return true;
    }
  };

  namespace details
  {
    /// @brief Responsible of holding all profile results
    inline Instrumentator GlobalInstrument;
  }

  /// @brief Saves the profiles stored in the global Instrumentator to file at 'path' in "chrome://tracing" format
  /// @param path The path to which to write the profiles
  /// @return True if successful
  inline bool save_tracing_to(const char* path) noexcept
  {
    return details::GlobalInstrument.write_tracing(path);
  }
}

#ifndef COLT_NO_PROFILE
  /// @brief Profiles a scope, with name 'name'. NOT AN INLINE MACRO.
  #define COLT_PROFILE_SCOPE(name) clt::bench::Timer COLT_CONCAT(__TIMER_, __LINE__); static constexpr clt::bench::ProfileSource COLT_CONCAT(__TIMER_SRC_, __LINE__) = { name, COLT_FILENAME, COLT_LINE_NUM }; ON_EXIT { clt::bench::details::GlobalInstrument.save_profile(COLT_CONCAT(__TIMER_SRC_, __LINE__), COLT_CONCAT(__TIMER_, __LINE__).get_start_time(), COLT_CONCAT(__TIMER_, __LINE__).get_elapsed_time()); }
  /// @brief Profiles a function. NOT AN INLINE MACRO.
  #define COLT_PROFILE_FN() COLT_PROFILE_SCOPE(COLT_FUNCTION_NAME)
#else
  /// @brief Does nothing.
  #define COLT_PROFILE_SCOPE(name) do {} while (0)
  /// @brief Does nothing.
  #define COLT_PROFILE_FN() do {} while (0)
#endif

#endif //!HG_COLT_BENCHMARK