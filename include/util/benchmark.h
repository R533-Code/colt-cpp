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

// Generate unique ID using filename and line number,
// registers begin and end of scope using ID.
// We can then reconstruct time-line.
namespace clt::bench
{
  using time_point = std::chrono::time_point<std::chrono::steady_clock>;
  using duration = std::chrono::steady_clock::duration;
  namespace tm = std::chrono;

  /// @brief Timer class, whic
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
    lstring fn_name;
    /// @brief The file name
    lstring file_name;
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

  class GlobalInstrumentator
  {
    std::vector<ProfileResult> profiles {};
    mutable std::mutex mtx {};

    template<meta::StringLiteral lit>
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
    GlobalInstrumentator() noexcept = default;

    void save_profile(const ProfileSource& src, time_point tm, duration dur, std::thread::id thread_id = std::this_thread::get_id()) noexcept
    {
      auto lck = std::scoped_lock(mtx);
      profiles.emplace_back(src, tm, dur, std::hash<std::thread::id>{}(thread_id));
    }

    bool write_tracing(lstring path) const noexcept
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
    inline GlobalInstrumentator GlobalInstrument = {};
  }

  inline void save_tracing_to(const char* path) noexcept
  {
    details::GlobalInstrument.write_tracing(path);
  }
}

#ifndef COLT_NO_PROFILE
  #define COLT_PROFILE_SCOPE(name) clt::bench::Timer COLT_CONCAT(__TIMER_, __LINE__); static constexpr clt::bench::ProfileSource COLT_CONCAT(__TIMER_SRC_, __LINE__) = { name, COLT_FILENAME, COLT_LINE_NUM }; ON_EXIT { clt::bench::details::GlobalInstrument.save_profile(COLT_CONCAT(__TIMER_SRC_, __LINE__), COLT_CONCAT(__TIMER_, __LINE__).get_start_time(), COLT_CONCAT(__TIMER_, __LINE__).get_elapsed_time()); }
  #define COLT_PROFILE_FN() COLT_PROFILE_SCOPE(COLT_FUNCTION_NAME)
#else
  #define COLT_PROFILE_SCOPE(name) do {} while (0)
  #define COLT_PROFILE_FN() do {} while (0)
#endif

#endif //!HG_COLT_BENCHMARK