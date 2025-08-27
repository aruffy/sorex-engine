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

namespace Sorex
{
  enum class EStatisticsGroup
  {
    System,
    Engine,
    Graphics,
    // Audio
    // Application,
    // Network
  };

  class StatisticsValue
  {
public:
    SRX_INLINE StatisticsValue(const String& name) SRX_NOEXCEPT: mName(name) {}
    virtual ~StatisticsValue() = default;

    StatisticsValue(const StatisticsValue& other)            = delete;
    StatisticsValue& operator=(const StatisticsValue& other) = delete;

    SRX_INLINE const String& GetName() const { return mName; }

    virtual String ToString() = 0;

private:
    String mName;
  };

  class IStatisticsProvider
  {
    SRX_RTTI_BASE(IStatisticsProvider);

public:
    virtual ~IStatisticsProvider() = default;

    /**
     * @brief Retrieve all statistics values provided by this provider.
     *
     * @param outValues Vector to be filled with pointers to statistics values.
     */
    virtual void GetAllStatistics(
      TVector<const StatisticsValue*>& outValues) const = 0;

    /**
     * @brief Retrieve statistics values belonging to a specific group.
     *
     * @param group The statistics group to filter by.
     * @param outValues Vector to be filled with pointers to statistics values.
     */
    virtual void GetStatisticsByGroup(
      EStatisticsGroup                 group,
      TVector<const StatisticsValue*>& outValues) const = 0;

    /**
     * @brief Reset all statistics values to their initial state.
     *
     * This function is called when statistics need to be cleared or
     * reinitialized.
     */
    virtual void ResetStatistics() = 0;
  };

  class StatisticsManager final
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

    // cppcheck-suppress functionConst
    void Reset()
    {
      mProviders.ForEach(
        [](IStatisticsProvider& provider) { provider.ResetStatistics(); });
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

    // API ApplicationLoop::Listener
    /* virtual void OnBeginFrame(float deltaTime) override;
    virtual void OnFinishFrame() override;
    virtual void OnExit() override; */

private:
    TObjectContainer<IStatisticsProvider> mProviders;
  };
}  // namespace Sorex
