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

#include "SxMaths.h"
#include "SxPoint.h"

#if defined(SOREX_COMPILER_MSVC)
#  pragma warning( \
    disable : 26495)  // @note: disable uninitialized variable warning: data
#endif

namespace Sorex::Math
{
  template<typename T>
  struct SRX_API TVector2
  {
    static_assert(std::is_floating_point_v<T>, "invalid vector value type");
    using value_type = T;

    union
    {
      struct
      {
        value_type x, y;
      };

      TArray<value_type, 2> data = { T{ 0 }, T{ 0 } };
    };

    TVector2() = default;
    SRX_INLINE TVector2(const value_type x, const value_type y) SRX_NOEXCEPT;

    TVector2(const TVector2& other)            = default;
    TVector2& operator=(const TVector2& other) = default;

    SRX_INLINE static TVector2 Zero() { return TVector2(); }
    SRX_INLINE static TVector2 One() { return TVector2(1.f, 1.f); }
    SRX_INLINE static TVector2 AxisX() { return TVector2(1.0, 0.0); }
    SRX_INLINE static TVector2 AxisY() { return TVector2(0.0, 1.0); }

    SRX_INLINE static value_type Distance(const TVector2& v1,
                                          const TVector2& v2) SRX_NOEXCEPT;

    // Calculates a dot product of two vectors.
    static value_type Dot(const TVector2& v1, const TVector2& v2) SRX_NOEXCEPT;

    // Returns a Z-component of the cross product of two vectors.
    static value_type Cross(const TVector2& v1,
                            const TVector2& v2) SRX_NOEXCEPT;

    // Returns the length of the vector.
    SRX_INLINE value_type Length() const SRX_NOEXCEPT;

    // Normalizes the vector and returns its length.
    value_type      Normalize() SRX_NOEXCEPT;
    static TVector2 Normalize(const TVector2& vec) SRX_NOEXCEPT;

    // Reflects a vector to the plane with the given normal.
    void            Reflect(const TVector2& norm) SRX_NOEXCEPT;
    static TVector2 Reflect(const TVector2& v,
                            const TVector2& norm) SRX_NOEXCEPT;

    // Peforms 2D rotation of the vector. Angle in radians.
    void            Rotate(const T angle) SRX_NOEXCEPT;
    static TVector2 Rotate(const TVector2& v, const T angle) SRX_NOEXCEPT;

    // Peforms 2D rotation of the vector considering point as origin. Angle in
    // radians.
    TVector2 RotateAroundPoint(const TVector2& point,
                               const T         angle) SRX_NOEXCEPT;

    SRX_INLINE bool IsZero() const { return (x == 0 && y == 0); }

    SRX_INLINE bool operator==(const TVector2& v) const SRX_NOEXCEPT
    {
      return (x == v.x && y == v.y);
    }

    SRX_INLINE bool operator!=(const TVector2& other) const SRX_NOEXCEPT
    {
      return !(*this == other);
    }

    SRX_INLINE TVector2 operator+() const SRX_NOEXCEPT { return *this; }
    SRX_INLINE TVector2 operator-() const { return TVector2<T>(-x, -y); }

    TVector2& operator+=(const TFloatingPoint<T>& p) SRX_NOEXCEPT;
    TVector2& operator-=(const TFloatingPoint<T>& p) SRX_NOEXCEPT;

    TVector2& operator*=(const value_type factor) SRX_NOEXCEPT;
    TVector2& operator/=(const value_type divisor) SRX_NOEXCEPT;
  };

  template<typename T>
  SRX_INLINE TVector2<T>::TVector2(const value_type aX,
                                   const value_type aY) SRX_NOEXCEPT
    : x(aX)
    , y(aY)
  {}

  template<typename T>
  SRX_INLINE T TVector2<T>::Distance(const TVector2& v1,
                                     const TVector2& v2) SRX_NOEXCEPT
  {
    const T dX = v1.x - v2.x;
    const T dY = v1.y - v2.y;

    if constexpr (std::is_same_v<T, float>)
      return ::sqrtf(dX * dX + dY * dY);

    return sqrt(dX * dX + dY * dY);
  }

  template<typename T>
  SRX_INLINE T TVector2<T>::Dot(const TVector2& v1,
                                const TVector2& v2) SRX_NOEXCEPT
  {
    return v1.x * v2.x + v1.y * v2.y;
  }

  template<typename T>
  SRX_INLINE T TVector2<T>::Cross(const TVector2& v1,
                                  const TVector2& v2) SRX_NOEXCEPT
  {
    return v1.x * v2.y - v1.y * v2.x;
  }

  template<typename T>
  SRX_INLINE T TVector2<T>::Length() const SRX_NOEXCEPT
  {
    if constexpr (std::is_same_v<T, float>)
      return ::sqrtf(x * x + y * y);

    return sqrt(x * x + y * y);
  }

  template<typename T>
  SRX_INLINE T TVector2<T>::Normalize() SRX_NOEXCEPT
  {
    const T len = Length();
    if (len)
    {
      const T ilen = T{ 1 } / len;
      x *= ilen;
      y *= ilen;
    }

    return len;
  }

  template<typename T>
  SRX_INLINE TVector2<T> TVector2<T>::Normalize(const TVector2& vec)
    SRX_NOEXCEPT
  {
    const T ilen = T{ 1 } / vec.Length();
    return TVector2(vec.x * ilen, vec.y * ilen);
  }

  template<typename T>
  SRX_INLINE void TVector2<T>::Reflect(const TVector2& norm) SRX_NOEXCEPT
  {
    const T d = T{ 2 } * (x * norm.x + y * norm.y);

    x = d * norm.x - x;
    y = d * norm.y - y;
  }

  template<typename T>
  SRX_INLINE TVector2<T> TVector2<T>::Reflect(const TVector2& v,
                                              const TVector2& norm) SRX_NOEXCEPT
  {
    const T d = T{ 2 } * Dot(v, norm);
    return TVector2<T>(d * norm.x - v.x, d * norm.y - v.y);
  }

  template<typename T>
  SRX_INLINE void TVector2<T>::Rotate(const T angle) SRX_NOEXCEPT
  {
    T s, c;
    if constexpr (std::is_same_v<T, float>)
    {
      s = ::sinf(angle);
      c = ::cosf(angle);
    }
    else
    {
      s = ::sin(angle);
      c = ::cos(angle);
    }

    const T pX = x;
    const T pY = y;

    x = pX * c - pY * s;
    y = pX * s + pY * c;
  }

  template<typename T>
  SRX_INLINE TVector2<T> TVector2<T>::Rotate(const TVector2& v,
                                             const T         angle) SRX_NOEXCEPT
  {
    T s, c;
    if constexpr (std::is_same_v<T, float>)
    {
      s = ::sinf(angle);
      c = ::cosf(angle);
    }
    else
    {
      s = ::sin(angle);
      c = ::cos(angle);
    }

    return TVector2<T>(v.x * c - v.y * s, v.x * s + v.y * c);
  }

  template<typename T>
  SRX_INLINE TVector2<T> TVector2<T>::RotateAroundPoint(const TVector2& point,
                                                        const T         angle)
    SRX_NOEXCEPT
  {
    const TVector2<T> tp(x - point.x, y - point.y);
    T                 s, c;

    if constexpr (std::is_same_v<T, float>)
    {
      s = ::sinf(angle);
      c = ::cosf(angle);
    }
    else
    {
      s = ::sin(angle);
      c = ::cos(angle);
    }

    return (TVector2(c * tp.x - s * tp.y, s * tp.x + c * tp.y) + point);
  }

  template<typename T>
  SRX_INLINE TVector2<T>& TVector2<T>::operator+=(const TFloatingPoint<T>& p)
    SRX_NOEXCEPT
  {
    x += p.x;
    y += p.y;
    return *this;
  }

  template<typename T>
  SRX_INLINE TVector2<T>& TVector2<T>::operator-=(const TFloatingPoint<T>& p)
    SRX_NOEXCEPT
  {
    x -= p.x;
    y -= p.y;
    return *this;
  }

  template<typename T>
  SRX_INLINE TVector2<T>& TVector2<T>::operator*=(const T factor) SRX_NOEXCEPT
  {
    x *= factor;
    y *= factor;
    return *this;
  }

  template<typename T>
  SRX_INLINE TVector2<T>& TVector2<T>::operator/=(const T divisor) SRX_NOEXCEPT
  {
    SRX_ASSERT(divisor != 0);

    const T factor = T{ 1 } / divisor;
    x *= factor;
    y *= factor;
    return *this;
  }

  template<typename T>
  SRX_INLINE TVector2<T> operator+(const TVector2<T>& lhs,
                                   const TVector2<T>& rhs) SRX_NOEXCEPT
  {
    return TVector2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
  }

  template<typename T>
  SRX_INLINE TVector2<T> operator-(const TVector2<T>& lhs,
                                   const TVector2<T>& rhs) SRX_NOEXCEPT
  {
    return TVector2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
  }

  template<typename T>
  SRX_INLINE TVector2<T> operator*(const TVector2<T>& lhs,
                                   const TVector2<T>& rhs) SRX_NOEXCEPT
  {
    return TVector2(lhs.x * rhs.x, lhs.y * rhs.y);
  }

  template<typename T>
  SRX_INLINE TVector2<T> operator*(const TVector2<T>& v,
                                   const T            factor) SRX_NOEXCEPT
  {
    return TVector2(v.x * factor, v.y * factor);
  }

  template<typename T>
  SRX_INLINE TVector2<T> operator*(const T            factor,
                                   const TVector2<T>& v) SRX_NOEXCEPT
  {
    return v * factor;
  }

  template<typename T>
  SRX_INLINE TVector2<T> operator/(const TVector2<T>& lhs,
                                   const TVector2<T>& rhs) SRX_NOEXCEPT
  {
    return TVector2<T>(lhs.x / rhs.x, lhs.y / rhs.y);
  }

  template<typename T>
  SRX_INLINE TVector2<T> operator/(const TVector2<T>& v,
                                   const T            divisor) SRX_NOEXCEPT
  {
    SRX_ASSERT(divisor != 0);
    return v * (T{ 1 } / divisor);
  }
}

namespace Sorex
{
  typedef Math::TVector2<scalar_t> Vector2;
  typedef Vector2                  Vec2;
}

using SxVec2 = Sorex::Vec2;

#if defined(SOREX_COMPILER_MSVC)
#  pragma warning(default : 26495)
#endif
