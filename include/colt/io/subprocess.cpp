#include "subprocess.h"

namespace clt
{
  bool Subprocess::is_alive() const noexcept
  {
    return subprocess_alive(const_cast<subprocess_s*>(&process_)) != 0;
  }

  File Subprocess::get_stdin() const noexcept
  {
    auto value = File::from(subprocess_stdin(&process_), File::Write);
    assert_true("Failed to open stdin of subprocess", value.is_value());
    return *value;
  }

  File Subprocess::get_stdout() const noexcept
  {
    auto value = File::from(subprocess_stdout(&process_), File::Read);
    assert_true("Failed to open stdout of subprocess", value.is_value());
    return *value;
  }

  File Subprocess::get_stderr() const noexcept
  {
    auto value = File::from(subprocess_stderr(&process_), File::Read);
    assert_true("Failed to open stderr of subprocess", value.is_value());
    return *value;
  }

  Option<int> Subprocess::join() noexcept
  {
    assert_true("Subprocess was already joined/detached!", joinable());
    if (!joinable()) [[unlikely]]
      std::terminate();
    joined = 1;
    ON_SCOPE_EXIT
    {
      // free resources
      subprocess_destroy(&process_);
    };
    if (int ret; subprocess_join(&process_, &ret) == 0)
      return {InPlace, ret};
    return None;
  }

  ErrorFlag Subprocess::detach() noexcept
  {
    assert_true("Subprocess was already joined/detached!", joinable());
    if (!joinable()) [[unlikely]]
      std::terminate();
    joined = 2;
    if (subprocess_destroy(&process_) == 0)
      return ErrorFlag::success();
    return ErrorFlag::error();
  }

  ErrorFlag Subprocess::terminate() noexcept
  {
    joined = 3;
    ON_SCOPE_EXIT
    {
      // free resources
      subprocess_destroy(&process_);
    };
    if (subprocess_terminate(&process_) == 0)
      return ErrorFlag::success();
    return ErrorFlag::error();
  }

  Option<Subprocess> clt::Subprocess::open(
      View<const char*> command_line, SubprocessOption opt,
      View<const char*> env) noexcept
  {
    assert_true(
        "command_line must be terminated with a NULL!",
        !command_line.empty() && command_line.back() == nullptr);
    assert_true(
        "env must be terminated with a NULL!",
        !env.empty() && env.back() == nullptr);
    subprocess_s process_;
    if (subprocess_create_ex(command_line.data(), (int)opt, env.data(), &process_)
        != 0)
      return None;
    return Option<Subprocess>{InPlace, process_};
  }
} // namespace clt
