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

#include "SxCoreMinimal.h"

namespace Sorex
{
  enum class ETaskAction
  {
    Await,
    Continue,
    Cancel
  };

  enum class ETaskPriority
  {
    None = 0,
    Low,
    Medium,
    High,
    Critical
  };

  class Task
  {
    SRX_RTTI_BASE(Task);

public:
    Task() = default;
    SRX_INLINE explicit Task(ETaskPriority priority) SRX_NOEXCEPT
      : mPriority(priority)
    {}

    virtual ~Task() = default;

    /**
     * @brief Execute the main task function.
     *
     * If `Execute` return ETaskAction::Await, the task should be resumed later
     * by Resume() call. When Resume() return ETaskAction::Continue, the task
     * will be executed again.
     *
     * @return ETaskAction - the action to be performed after the task
     * execution.
     */
    virtual ETaskAction Execute() = 0;

    /**
     * @brief Try to resume the task.
     *
     * @return ETaskAction - next action describes if the caller should await,
     * can execute the task or should cancel it.
     */
    virtual ETaskAction Resume() = 0;

    /**
     * @brief Shutdown the task.
     *
     * This call is used if the task is cancelled. It should free all resources
     * that was allocated for the task.
     * After this call, the task might be deleted.
     *
     */
    virtual void Shutdown() = 0;

    /**
     * @brief Finalize the task.
     *
     * This call is used to finalize the task execution.
     * It might be helpful when the task is executed in a separate thread.
     * But some task action is required to be performed in the `main` thread.
     * As an example: the OpenGL call should be performed in the active OpenGL
     * context.
     *
     * @return Status - the final status of the task.
     */
    virtual Status Finalize() = 0;

    ETaskPriority GetPriority() const { return mPriority; }

    std::strong_ordering operator<=>(const Task& other) const
    {
      return mPriority == other.mPriority  ? std::strong_ordering::equal
             : mPriority > other.mPriority ? std::strong_ordering::greater
                                           : std::strong_ordering::less;
    }

private:
    ETaskPriority mPriority = ETaskPriority::Medium;
  };
}  // namespace

using SxTask = Sorex::Task;
