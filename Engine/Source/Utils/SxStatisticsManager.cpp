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

#include <Sorex/Utils/SxStatisticsManager.h>

namespace Sorex
{
  StatisticsManager& StatisticsManager::GetInstance()
  {
    static StatisticsManager instance;
    return instance;
  }

  void StatisticsManager::GetStatisticsByGroup(
    EStatisticsGroup                 group,
    TVector<const StatisticsValue*>& values) const
  {
    values.clear();
    mProviders.ForEach([group, &values](const StatisticsProvider& provider) {
      provider.GetStatisticsByGroup(group, values);
    });
  }

  void StatisticsManager::OnBeginFrame(float deltaTime)
  {
    mProviders.ForEach([deltaTime](StatisticsProvider& provider) {
      provider.OnBeginFrame(deltaTime);
    });

    // FIXME: Temprorary solution
    static float accumulator = 0.0f;
    accumulator += deltaTime;
    constexpr float kSecond = 1000.f;
    if (accumulator >= kSecond)
    {
      accumulator = 0.0f;
      mProviders.ForEach([](StatisticsProvider& provider) {
        TVector<const StatisticsValue*> values;
        provider.GetAllStatistics(values);
        for (const StatisticsValue* value : values)
        {
          SRX_INFO("Statistic {}: {}", value->GetName(), value->ToString());
        }
      });
    }
  }

  void StatisticsManager::OnFinishFrame()
  {
    mProviders.ForEach(
      [](StatisticsProvider& provider) { provider.OnFinishFrame(); });
  }
}  // namespace
