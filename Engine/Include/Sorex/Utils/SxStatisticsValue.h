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
#include <Sorex/Utils/SxString.h>

namespace Sorex::Statistics
{
  class Value
  {
public:
    SRX_INLINE Value(const String& name) SRX_NOEXCEPT: mName(name) {}
    virtual ~Value() = default;

    Value(const Value& other)            = delete;
    Value& operator=(const Value& other) = delete;

    SRX_INLINE const String& GetName() const { return mName; }

    virtual String ToString() const = 0;

private:
    String mName;
  };

  /**
   * @brief Statistics counter for integral types.
   *
   * TCounter is a generic statistics counter designed for use cases where
   * values are updated frequently, such as per-frame or periodic statistics
   * (e.g., FPS counters). The class maintains two members:
   * - @c  mAccumulator: a mutable value that is incremented or decremented as
   * events occur.
   * - @c mValue: the stable value representing the actual statistic, updated
   * from @c mAccumulator via the Apply() or Reset() methods.
   *
   * This separation allows for continuous updates to the accumulator without
   * affecting the reported value, which is only refreshed at specific intervals
   * (such as at the end of a frame).
   *
   * @tparam TInt Integral type used for counting.
   *
   * Example usage:
   * @code
   * TCounter<int32> fpsCounter("FPS");
   * fpsCounter.Increase(); // Called every frame
   * fpsCounter.Reset();    // Called once per second to reset accumulator and
   *                        // update the visible FPS value
   *  std::cout << fpsCounter.ToString(); // Prints
   * @endcode
   */
  template<typename TInt>
  class TCounter: public Value
  {
    static_assert(std::is_integral_v<TInt>,
                  "[Statistics::Counter] Must be a integral type");

public:
    SRX_INLINE explicit TCounter(const String& name) SRX_NOEXCEPT
      : Value(name)
      , mAccumulator(TInt{ 0 })
      , mValue(TInt{ 0 })
    {}

    virtual ~TCounter() override = default;
    virtual String ToString() const override { return Utils::ToString(mValue); }

    void Apply() { mValue = mAccumulator; }
    void Reset(const TInt val = TInt{ 0 })
    {
      mValue       = mAccumulator;
      mAccumulator = val;
    }

    void Increase() { ++mAccumulator; }
    void Decrease()
    {
      if constexpr (std::is_unsigned<TInt>::value)
        mAccumulator = mAccumulator ? mAccumulator - 1 : 0u;
      else
        mAccumulator -= 1;
    }

    SRX_INLINE TInt GetAccumulator() const { return mAccumulator; }
    SRX_INLINE TInt GetValue() const { return mValue; }

    TCounter& operator=(const TInt value) noexcept
    {
      mAccumulator = value;
      return *this;
    }

    TCounter& operator++() noexcept
    {
      ++mAccumulator;
      return *this;
    }

    TCounter& operator--() noexcept
    {
      --mAccumulator;
      return *this;
    }

    TCounter& operator+=(const TInt arg) noexcept
    {
      mAccumulator += arg;
      return *this;
    }

    TCounter& operator-=(const TInt arg) noexcept
    {
      if constexpr (std::is_unsigned<TInt>::value)
        mAccumulator = (mAccumulator > arg) ? mAccumulator - arg : 0u;
      else
        mAccumulator -= arg;

      return *this;
    }

private:
    TInt mAccumulator;
    TInt mValue;
  };

  using Counter = TCounter<int32>;
}  // namespace Sorex::Statistics
