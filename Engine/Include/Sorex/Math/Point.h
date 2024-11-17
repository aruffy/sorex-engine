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

#include <Sorex/Types.h>
#include <Sorex/Platform.h>

namespace Sorex::Concept
{
  template<typename From, typename To>
  concept SafeIntegralConversion =
    std::is_integral_v<From> && std::is_integral_v<To>
    && ((std::is_signed_v<From> && std::is_signed_v<To>)
        || (std::is_unsigned_v<From> && std::is_unsigned_v<To>))
    && (sizeof(From) <= sizeof(To))
    && (std::numeric_limits<From>::max() <= std::numeric_limits<To>::max());
}

namespace Sorex::Math
{
  template<typename T>
  struct SRX_API TIntegerPoint
  {
private:
    using ValueType = T;
    static_assert(std::is_integral_v<T>,
                  "[TIntegerPoint] Value type must be an integral type");

public:
    union
    {
      struct
      {
        T x, y;
      };

      T data[2] = { 0, 0 };
    };

    SRX_INLINE TIntegerPoint() SRX_NOEXCEPT
      : x(0)
      , y(0)
    {}

    template<typename U>
      requires Concept::SafeIntegralConversion<U, T>
    SRX_INLINE TIntegerPoint(const U aX, const U aY) SRX_NOEXCEPT
      : x(aX)
      , y(aY)
    {}

    TIntegerPoint(const TIntegerPoint& other) SRX_NOEXCEPT            = default;
    TIntegerPoint& operator=(const TIntegerPoint& other) SRX_NOEXCEPT = default;

    template<typename U>
      requires(!std::is_same_v<T, U>) and Concept::SafeIntegralConversion<U, T>
    SRX_INLINE TIntegerPoint(const TIntegerPoint<U>& other)
      SRX_NOEXCEPT  // cppcheck-suppress noExplicitConstructor
      : x(static_cast<T>(other.x))
      , y(static_cast<T>(other.y))
    {}

    template<typename U>
      requires(!std::is_same_v<T, U>) and Concept::SafeIntegralConversion<U, T>
    SRX_INLINE TIntegerPoint& operator=(const TIntegerPoint<U>& other)
      SRX_NOEXCEPT
    {
      x = static_cast<T>(other.x);
      y = static_cast<T>(other.y);
      return *this;
    }

    template<typename U>
      requires std::is_signed_v<T> and Concept::SafeIntegralConversion<U, T>
    SRX_INLINE scalar_t
    Distance(const TIntegerPoint<U>& other) const SRX_NOEXCEPT
    {
      const scalar_t dx = static_cast<T>(other.x) - x;
      const scalar_t dy = static_cast<T>(other.y) - y;
      return std::sqrt(dx * dx + dy * dy);
    }

    template<typename U>
      requires std::is_unsigned_v<T> and Concept::SafeIntegralConversion<U, T>
    SRX_INLINE scalar_t
    Distance(const TIntegerPoint<U>& other) const SRX_NOEXCEPT
    {
      const scalar_t dx = static_cast<scalar_t>(other.x) - x;
      const scalar_t dy = static_cast<scalar_t>(other.y) - y;
      return std::sqrt(dx * dx + dy * dy);
    }


    void Swap(TIntegerPoint& other) SRX_NOEXCEPT
    {
      ValueType t;
      t       = other.x;
      other.x = x;
      x       = t;

      t       = other.y;
      other.y = y;
      y       = t;
    }

    static void Swap(TIntegerPoint& a, TIntegerPoint& b) SRX_NOEXCEPT
    {
      ValueType t;
      t   = a.x;
      a.x = b.x;
      b.x = t;

      t   = a.y;
      a.y = b.y;
      b.y = t;
    }

    SRX_INLINE SRX_TYPENAME TIntegerPoint operator-() const SRX_NOEXCEPT
    {
      static_assert(std::is_signed_v<T>,
                    "invalid operator for unsigned integer");
      return { -x, -y };
    }

    SRX_INLINE TIntegerPoint& operator+=(const TIntegerPoint& other)
      SRX_NOEXCEPT
    {
      x += other.x;
      y += other.y;
      return *this;
    }

    template<typename U>
      requires Concept::SafeIntegralConversion<U, T>
    SRX_INLINE TIntegerPoint& operator+=(const TIntegerPoint<U>& other)
      SRX_NOEXCEPT
    {
      x += static_cast<T>(other.x);
      y += static_cast<T>(other.y);
      return *this;
    }

    SRX_INLINE TIntegerPoint& operator-=(const TIntegerPoint& other)
      SRX_NOEXCEPT
    {
      x -= other.x;
      y -= other.y;
      return *this;
    }

    template<typename U>
      requires Concept::SafeIntegralConversion<U, T>
    SRX_INLINE TIntegerPoint& operator-=(const TIntegerPoint<U>& other)
      SRX_NOEXCEPT
    {
      x -= static_cast<T>(other.x);
      y -= static_cast<T>(other.y);
      return *this;
    }

    SRX_INLINE bool operator==(const TIntegerPoint& other) const SRX_NOEXCEPT
    {
      return (x == other.x && y == other.y);
    }
    SRX_INLINE bool operator!=(const TIntegerPoint& other) const SRX_NOEXCEPT
    {
      return (x != other.x || y != other.y);
    }

    template<typename U>
      requires std::is_integral_v<U>
    SRX_INLINE bool operator==(const TIntegerPoint<U>& other) const SRX_NOEXCEPT
    {
      if constexpr (sizeof(T) < sizeof(U))
        return (static_cast<U>(x) == other.x && static_cast<U>(y) == other.y);

      return (x == static_cast<T>(other.x) && y == static_cast<T>(other.y));
    }

    template<typename U>
    SRX_INLINE bool operator!=(const TIntegerPoint<U>& other) const SRX_NOEXCEPT
    {
      return !(*this == other);
    }

    SRX_INLINE TIntegerPoint& operator*=(const T factor) SRX_NOEXCEPT
    {
      x *= factor;
      y *= factor;
      return *this;
    }

    SRX_INLINE TIntegerPoint& operator/=(const T divisor) SRX_NOEXCEPT
    {
      x /= divisor;
      y /= divisor;
      return *this;
    }
  };

  template<typename T>
  SRX_INLINE TIntegerPoint<T> operator+(const TIntegerPoint<T>& lhs,
                                        const TIntegerPoint<T>& rhs)
    SRX_NOEXCEPT
  {
    return TIntegerPoint<T>(lhs.x + rhs.x, lhs.y + rhs.y);
  }

  template<typename T>
  SRX_INLINE TIntegerPoint<T> operator-(const TIntegerPoint<T>& lhs,
                                        const TIntegerPoint<T>& rhs)
    SRX_NOEXCEPT
  {
    return TIntegerPoint<T>(lhs.x - rhs.x, lhs.y - rhs.y);
  }

  template<typename T>
  SRX_INLINE TIntegerPoint<T> operator*(const TIntegerPoint<T>& point,
                                        const T factor) SRX_NOEXCEPT
  {
    return TIntegerPoint<T>(point.x * factor, point.y * factor);
  }
}
