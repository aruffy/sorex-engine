#pragma once

#include <Sorex/SxTypes.h>
#include <Sorex/SxPlatform.h>

namespace Sorex
{
  class Thread
  {
public:
    /**
     * @brief Check that thread where this function was invoked is the main
     * application thread.
     *
     * @return True if invoked from main application thread.
     */
    static bool IsMainThread() SRX_NOEXCEPT;

    /**
     * @brief Set calling thread identifier as the main application thread.
     */
    static void SetMainThread() SRX_NOEXCEPT;

    /**
     * @brief Blocks the execution of the current thread for at least the
     * specified duration.
     *
     * @param milliseconds - duration of the execution blocking
     */
    static void Sleep(const int64 milliseconds);

    SRX_INLINE static std::thread::id GetId()
    {
      return std::this_thread::get_id();
    }

    Thread() = default;
    virtual ~Thread();

    Thread(const Thread& other)            = delete;
    Thread& operator=(const Thread& other) = delete;

    template<class Func, class... Args>
    void Execute(Func&& func, Args&&... args);

    void Join();

private:
    static std::thread::id& GetMainThreadId();

private:
    std::thread mThreadObject;
  };

  template<class Func, class... Args>
  void Thread::Execute(Func&& func, Args&&... args)
  {
    mThreadObject =
      std::thread{ std::forward<Func>(func), std::forward<Args>(args)... };
  }
}
