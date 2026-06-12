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

#include <Sorex/Utils/SxStatisticsProvider.h>

#include "GLTypes.h"
#include "GLResourceToken.h"

namespace Sorex
{
  class GLStatisticsProvider: public StatisticsProvider
  {
    SRX_RTTI(GLStatisticsProvider, StatisticsProvider);

public:
    GLStatisticsProvider();

    // StatisticsProvider Interface
    virtual void GetAllStatistics(
      TVector<const Statistics::Value*>& outValues) const override;
    virtual void GetStatisticsByGroup(
      EStatisticsGroup                   group,
      TVector<const Statistics::Value*>& outValues) const override;

    virtual void ResetStatistics() override;
    virtual void OnBeginFrame(const float deltaTime) override;
    virtual void OnFinishFrame() override;

    // Methods
    SRX_INLINE void OnDrawCall() { mDrawCalls.Increase(); }

private:
    Statistics::TCounter<uint32> mDrawCalls;
    Statistics::TCounter<uint32> mFramesPerSecond;
    float                        mTimer;
  };
}  // namespace Sorex
