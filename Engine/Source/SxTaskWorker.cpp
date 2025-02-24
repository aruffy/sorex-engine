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

#include "SxTaskWorker.h"
namespace Sorex
{
  TaskWorker::TaskWorker(StringView name, uint32 sleepTime)
    : LoopingThread(name)
    , mSleepTime(sleepTime)
    , mCompletedTaskNumber(0)
  {}

  TaskWorker::~TaskWorker()
  {
    Join();
  }

  void TaskWorker::Push(TUniquePointer<Task> task)
  {
    if (!task)
    {
      SRX_NOENTRY("invlaid task");
      return;
    }

    mMtx.lock();
    mQueue.push(task.get());  // cppcheck-suppress danglingLifetime
    mStorage.push_front(std::move(task));
    mMtx.unlock();
  }

  void TaskWorker::PushCompleted(Task* task)
  {
    if (task == nullptr)
    {
      SRX_NOENTRY("invalid completed task");
      return;
    }

    MutexLock _lock(mMtx);

    auto it = std::find_if(mStorage.begin(),
                           mStorage.end(),
                           [task](const TUniquePointer<Task>& other) {
                             return other.get() == task;
                           });

    // @NOTE: it means that the task pointer is also invalid
    SRX_ASSERT(it != mStorage.end());

    mCompleted.push(std::move(*it));
    mStorage.erase(it);

    ++mCompletedTaskNumber;
  }

  SRX_NODISCARD TUniquePointer<Task> TaskWorker::Pop()
  {
    MutexLock _lock(mMtx);

    if (mCompleted.empty())
      return nullptr;

    auto task = std::move(mCompleted.front());

    mCompleted.pop();
    --mCompletedTaskNumber;

    return task;
  }

  Task* TaskWorker::PullTask()
  {
    MutexLock _lock(mMtx);

    if (mQueue.empty())
      return nullptr;

    Task* task = mQueue.top();
    mQueue.pop();

    return task;
  }

  bool TaskWorker::HandleTask()
  {
    Task* task = PullTask();
    if (!task)
      return false;

    const ETaskAction action = task->Execute();
    switch (action)
    {
    case ETaskAction::Continue:
      PushCompleted(task);
      break;

    case ETaskAction::Await:
      mDeferred.push(task);
      break;

    case ETaskAction::Cancel:
      Shutdown(task);
      break;

    default:
      SRX_NOENTRY("unexpected laoding task action");
      break;
    }

    return true;
  }

  bool TaskWorker::HandleDeferredTask()
  {
    const size_t size = mDeferred.size();
    if (size <= 0)
      return false;

    Task* task = mDeferred.front();
    SRX_ASSERT(task);

    const ETaskAction action = task->Resume();
    if (action == ETaskAction::Await)
    {
      if (size > 1)
      {
        mDeferred.pop();
        mDeferred.push(task);
      }

      return true;
    }

    mDeferred.pop();
    if (action == ETaskAction::Continue)
    {
      mMtx.lock();
      mQueue.push(task);
      mMtx.unlock();
    }
    else  // Cancel
    {
      Shutdown(task);
    }

    return true;
  }

  void TaskWorker::Shutdown(Task* task)
  {
    if (task == nullptr)
      return;

    task->Shutdown();

    MutexLock lock(mMtx);

    std::erase_if(mStorage, [task](const TUniquePointer<Task>& other) {
      return other.get() == task;
    });
  }

  ETaskAction TaskWorker::ThreadFunction(uint32& sleep)
  {
    // @TODO: Conditional Variable

    sleep            = mSleepTime;
    const bool bBusy = HandleDeferredTask() || HandleTask();
    return bBusy ? ETaskAction::Continue : ETaskAction::Await;
  }
}  // namespace
