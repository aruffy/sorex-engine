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

#pragma once

#include <Sorex/SxCoreMinimal.h>
#include <Sorex/SxThread.h>
#include <Sorex/SxTask.h>

namespace Sorex
{
  class TaskWorker final: public LoopingThread
  {
public:
    explicit TaskWorker(StringView name, uint32 sleepTime = 125U);
    virtual ~TaskWorker() override;

    /**
     * @brief Push the loading request to the queue.
     *  Request will be prossecced by loading callback.
     *
     * @param request - asset loading request
     */
    void Push(TUniquePointer<Task> task);

    /**
     * @brief Try to get completed loading request.
     *
     * @return request pointer or null if no completed request available.
     */
    SRX_NODISCARD TUniquePointer<Task> Pop();

    bool HasCompletedTask() const
    {
      return SRX_ATOMIC_LOAD(mCompletedTaskNumber);
    }

protected:
    virtual ETaskAction ThreadFunction(uint32& sleep) override;

private:
    Task* PullTask();
    void  PushCompleted(Task* task);

    void Shutdown(Task* task);

    bool HandleTask();
    bool HandleDeferredTask();

private:
    Mutex  mMtx;
    uint32 mSleepTime;

    TPriorityQueue<Task*>       mQueue;
    TList<TUniquePointer<Task>> mStorage;

    TQueue<TUniquePointer<Task>> mCompleted;
    TQueue<Task*>                mDeferred;

    TAtomic<int> mCompletedTaskNumber;
  };
}  // namespace
