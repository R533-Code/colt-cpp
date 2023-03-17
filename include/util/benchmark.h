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

// Generate unique ID using filename and line number,
// registers begin and end of scope using ID.
// We can then reconstruct time-line.
namespace clt::bench
{
  using time_point = std::chrono::time_point<std::chrono::steady_clock>;
  using duration = std::chrono::steady_clock::duration;

  /// @brief Timer class, whic
  class Timer
  {
    /// @brief The start time
    time_point start_time;

  public:
    /// @brief Constructs a Timer, and records current time
    constexpr Timer() noexcept
      : start_time(std::chrono::steady_clock::now()) {}
    
    /// @brief Records current time as beginning of recording
    constexpr void restart() noexcept
    {
      start_time = std::chrono::steady_clock::now();
    }

    /// @brief Returns the start time of the recording
    /// @return The start time of the recording
    constexpr auto get_start_time() const noexcept { return start_time; }

    /// @brief Returns the elapsed time since constructor
    /// @return The elapsed time
    constexpr duration get_elapsed_time() const noexcept
    {
      return std::chrono::steady_clock::now() - start_time;
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
    ProfileSource& src;
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
    std::mutex mtx {};

  public:
    GlobalInstrumentator() noexcept = default;

    void save_profile(ProfileSource& src, time_point tm, duration dur, std::thread::id thread_id = std::this_thread::get_id()) noexcept
    {
      auto lck = std::scoped_lock(mtx);
      profiles.emplace_back(src, tm, dur, std::hash<std::thread::id>{}(thread_id));
    }

    bool write_tracing(lstring path) noexcept
    {
      auto lck = std::scoped_lock(mtx);
      auto file = fopen(path, "wt");
      if (!file)
        return false;
      
      std::fputs("{\"otherData\": {},\"traceEvents\":[", file);
      char start[24];
      char durat[24];
      for (auto& profile : profiles)
      {
        *std::to_chars(start, start + 24, profile.start_time.time_since_epoch().count()).ptr = '\0';
        *std::to_chars(durat, durat + 24, profile.duration.count()).ptr = '\0';
        std::fprintf(file, R"({
          "cat": "function",
          "dur": %s,
          "name": "%s in %s",
          "ph": "X",
          "pid": 0,
          "tid": "%llu",
          "ts": %s
        })", durat, profile.src.fn_name, profile.src.file_name, profile.thread_id, start);
      }
      std::fputs("]}", file);
      std::fclose(file);
      return true;
    }
  };

  inline GlobalInstrumentator GlobalInstrument = {};
}

#endif //!HG_COLT_BENCHMARK