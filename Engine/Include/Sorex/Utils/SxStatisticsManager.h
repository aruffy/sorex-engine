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
#include <Sorex/Containers/SxObjectContainer.h>
#include <Sorex/SxDirector.h>

#include "SxStatisticsProvider.h"

namespace Sorex
{
  using StatisticsValue = Statistics::Value;
  class StatisticsManager final: public Director::IListener
  {
public:
    StatisticsManager(const StatisticsManager& other)            = delete;
    StatisticsManager& operator=(const StatisticsManager& other) = delete;

    static StatisticsManager& GetInstance();

    template<typename T, typename... Args>
    T* GetOrCreateStatisticsProvider(Args&&... args)
    {
      if (T* const provider = mProviders.Get<T>())
        return provider;

      return mProviders.Add<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    SRX_INLINE T* GetStatisticsProvider()
    {
      return mProviders.Get<T>();
    }

    // cppcheck-suppress functionConst
    void Reset()
    {
      mProviders.ForEach(
        [](StatisticsProvider& provider) { provider.ResetStatistics(); });
    }

    void GetStatisticsByGroup(EStatisticsGroup                 group,
                              TVector<const StatisticsValue*>& values) const;

    template<typename T>
    void GetStatisticsByProvider(TVector<const StatisticsValue*>& values) const
    {
      if (auto provider = mProviders.Get<T>())
        provider->GetAllStatistics(values);
    }

private:
    StatisticsManager() = default;

    // API Director::IListener
    virtual void OnBeginFrame(float deltaTime) override;
    virtual void OnFinishFrame() override;

private:
    TObjectContainer<StatisticsProvider> mProviders;
  };
}  // namespace Sorex
