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

  template<typename TInt>
  class TCounter: public Value
  {
    static_assert(std::is_integral_v<TInt>,
                  "[Statistics::Counter] Must be a integral type");

public:
    TCounter(const String& name)
      : Value(name)
      , _value(TInt{ 0 })
    {}

    virtual ~TCounter() override = default;
    virtual String ToString() const override { return Utils::ToString(_value); }

    inline void Reset(const TInt val = TInt{ 0 }) { _value = val; }
    inline void Increase() { ++_value; }
    inline void Decrease()
    {
      if constexpr (std::is_unsigned<TInt>::value)
        _value = _value ? _value - 1 : 0u;
      else
        _value -= 1;
    }

    TCounter& operator=(const TInt value) noexcept
    {
      _value = value;
      return *this;
    }

    TCounter& operator++() noexcept
    {
      ++_value;
      return *this;
    }

    TCounter& operator--() noexcept
    {
      --_value;
      return *this;
    }

    TCounter& operator+=(const TInt arg) noexcept
    {
      _value += arg;
      return *this;
    }

    TCounter& operator-=(const TInt arg) noexcept
    {
      _value -= arg;
      return *this;
    }

private:
    TInt _value;
    bool _isRefreshable;
  };

  using Counter = TCounter<int32>;
}  // namespace Sorex::Statistics
