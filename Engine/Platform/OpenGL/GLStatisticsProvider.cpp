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

#include "GLStatisticsProvider.h"

namespace Sorex
{
  GLStatisticsProvider::GLStatisticsProvider()
    : mDrawCalls("Draw Calls")
    , mFramesPerSecond("FPS")
    , mFpsAccumulator(0.0f, 0)
    , mDrawCallsAccumulator(0u)
  {}

  void GLStatisticsProvider::GetAllStatistics(
    TVector<const Statistics::Value*>& outValues) const
  {
    outValues.push_back(&mDrawCalls);
    outValues.push_back(&mFramesPerSecond);
  }

  void GLStatisticsProvider::GetStatisticsByGroup(
    EStatisticsGroup                   group,
    TVector<const Statistics::Value*>& outValues) const
  {
    if (group == EStatisticsGroup::Graphics)
      GetAllStatistics(outValues);
  }

  void GLStatisticsProvider::ResetStatistics()
  {
    mDrawCalls.Reset();
    mFramesPerSecond.Reset();
    mFpsAccumulator       = { 0.0f, 0 };
    mDrawCallsAccumulator = 0u;
  }

  void GLStatisticsProvider::OnBeginFrame(const float deltaTime)
  {
    mDrawCallsAccumulator = 0u;
    mFpsAccumulator.first += deltaTime;
    mFpsAccumulator.second += 1;
  }

  void GLStatisticsProvider::OnFinishFrame()
  {
    mDrawCalls = mDrawCallsAccumulator;

    constexpr float kSecond = 1000.f;
    const scalar_t  fps =
      scalar_t(mFpsAccumulator.second) * kSecond / mFpsAccumulator.first;
    mFramesPerSecond = static_cast<uint32>(fps);

    if (mFpsAccumulator.first >= kSecond)
      mFpsAccumulator = { 0.0f, 0 };
  }
}  // namespace Sorex
