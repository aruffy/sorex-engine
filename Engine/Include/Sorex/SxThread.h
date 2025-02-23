/**************************************************************************/
/*                         This file is part of:                          */
/*                                SOREX                                   */
/*                 Simple OpenGL Rendering Engine eXtended                */
/**************************************************************************/
/* Copyright (c) 2022-2024 Aleksandr Ershov (Ruffy).                      */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include <compare>

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
