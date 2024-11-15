#include <Sorex/Thread/Thread.h>

namespace Sorex
{
  bool Thread::IsMainThread() SRX_NOEXCEPT
  {
    return std::this_thread::get_id() == GetMainThreadId();
  }

  void Thread::SetMainThread() SRX_NOEXCEPT
  {
    GetMainThreadId() = std::this_thread::get_id();
  }

  std::thread::id& Thread::GetMainThreadId()
  {
    static std::thread::id id;
    return id;
  }

  void Thread::Sleep(const int64 milliseconds)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
  }

  Thread::~Thread()
  {
    Join();
  }

  void Thread::Join()
  {
    if (mThreadObject.joinable())
      mThreadObject.join();
  }
}
