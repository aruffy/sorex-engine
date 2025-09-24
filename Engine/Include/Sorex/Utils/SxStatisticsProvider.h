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

#include <Sorex/SxCoreMinimal.h>

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

  class StatisticsProvider
  {
    SRX_RTTI_BASE(StatisticsProvider);

public:
    virtual ~StatisticsProvider() = default;

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

    virtual void OnBeginFrame(float deltaTime) {};
    virtual void OnFinishFrame() {};
  };

  enum class EGraphicsResource
  {
    Texture,
    Buffer,
    Shader,
    Other
  };

  class GraphicsStatisticsProvider: public StatisticsProvider
  {
    SRX_RTTI(GraphicsStatisticsProvider, StatisticsProvider);

public:
    virtual ~GraphicsStatisticsProvider() override = default;
  };
}  // namespace Sorex
