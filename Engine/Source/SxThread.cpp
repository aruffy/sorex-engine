/**************************************************************************/
/*                         This file is part of:                          */
/*                                SOREX                                   */
/*                 Simple OpenGL Rendering Engine eXtended                */
/**************************************************************************/
/* Copyright (c) 2022 Aleksandr Ershov (Ruffy).                           */
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

#include <Sorex/SxThread.h>

#include <Sorex/Utils/SxString.h>

namespace
{
#ifndef SOREX_DEBUG_NONE
  uint64_t s_GetThreadId()
  {
    std::stringstream ssconv;
    ssconv << Sorex::Thread::GetId();
    uint64_t tid;
    SRX_VERIFY(Sorex::Utils::ToInteger(ssconv.str(), tid)
               == Sorex::EStatusCode::Ok);

    return tid;
  }
#endif
}  // namespace

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

  LoopingThread::LoopingThread(StringView name)
    : mName(name)
    , mIsRunning(false)
    , mIsQuitRequested(false)
  {}

  EStatusCode LoopingThread::Start()
  {
    if (SRX_ATOMIC_LOAD(mIsRunning))
      return EStatusCode::Busy;

    SRX_ATOMIC_STORE(mIsRunning, true);
    SRX_ATOMIC_STORE(mIsQuitRequested, false);

    Thread::Execute([this]() {

#ifndef SOREX_DEBUG_NONE
      const uint64_t tid = s_GetThreadId();
      SRX_INFO("Thread {} <{}> start", mName, tid);
#endif

      uint32 milliseconds = 0;
      while (!SRX_ATOMIC_LOAD(mIsQuitRequested))
      {
        const ETaskAction action = ThreadFunction(milliseconds);

        if (action == ETaskAction::Cancel)
          break;

        if (action == ETaskAction::Await && milliseconds > 10U)
          Thread::Sleep(milliseconds);
      }

      SRX_ATOMIC_STORE(mIsRunning, false);

#ifndef SOREX_DEBUG_NONE
      SRX_INFO("Thread <{}> stop", tid);
#endif
    });

    return EStatusCode::Ok;
  }

  void LoopingThread::Stop()
  {
    SRX_ATOMIC_STORE(mIsQuitRequested, true);
  }

}  // namespace
