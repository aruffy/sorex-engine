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

#include "SxVector2.h"
#include "SxVector3.h"

#if defined(SOREX_COMPILER_MSVC)
#  pragma warning( \
    disable : 26495)  // @note: disable uninitialized variable warning: data
#endif

namespace Sorex::Math
{
  template<typename T>
  struct SRX_API TVector4
  {
    static_assert(std::is_floating_point_v<T>, "invalid vector value type");
    using value_type = T;

    union
    {
      struct
      {
        value_type x, y, z, w;
      };

      TArray<value_type, 4> data = { 0, 0, 0, 0 };
    };

    TVector4() = default;
    SRX_INLINE TVector4(const T x, const T y, const T z, const T w = T{ 1.0 })
      SRX_NOEXCEPT;

    SRX_INLINE explicit TVector4(const TVector2<T>& v) SRX_NOEXCEPT;
    SRX_INLINE explicit TVector4(const TVector3<T>& v) SRX_NOEXCEPT;

    SRX_INLINE TVector4(const TVector3<T>& v, const T w) SRX_NOEXCEPT;

    // Returns the dot product of two vectors.
    static value_type Dot(const TVector4& q1, const TVector4& q2) SRX_NOEXCEPT;

    // Returns the length of the vector.
    value_type Length() const SRX_NOEXCEPT;

    // Returns the square of the vector length.
    value_type LengthSq() const SRX_NOEXCEPT;

    // Performs vector normalization (produces a unit vector).
    SRX_INLINE value_type Normalize() SRX_NOEXCEPT;
    static TVector4       Normalize(const TVector4& q) SRX_NOEXCEPT;

    // Performs perspective division of the vector (making w = 1).
    void               PerspectiveDivide() SRX_NOEXCEPT;
    static TVector3<T> PerspectiveDivide(const TVector4& v) SRX_NOEXCEPT;

    SRX_INLINE TVector4 operator+() const SRX_NOEXCEPT { return *this; }
    SRX_INLINE TVector4 operator-() const SRX_NOEXCEPT
    {
      return TVector4(-x, -y, -z, -w);
    }

    SRX_INLINE TVector4& operator+=(const TVector4& v) SRX_NOEXCEPT;
    SRX_INLINE TVector4& operator-=(const TVector4& v) SRX_NOEXCEPT;
    SRX_INLINE TVector4& operator*=(const TVector4& v) SRX_NOEXCEPT;
    // cppcheck-suppress passedByValue
    SRX_INLINE TVector4& operator*=(const value_type factor) SRX_NOEXCEPT;
    // cppcheck-suppress passedByValue
    SRX_INLINE TVector4& operator/=(const value_type divisor) SRX_NOEXCEPT;
  };

  // ============================================================================
  //   I m p l e m e n t a t i o n
  // ============================================================================

  template<typename T>
  SRX_INLINE TVector4<T>::TVector4(const T ax,
                                   const T ay,
                                   const T az,
                                   const T aw) SRX_NOEXCEPT
    : x(ax)
    , y(ay)
    , z(az)
    , w(aw)
  {}

  template<typename T>
  SRX_INLINE TVector4<T>::TVector4(const TVector2<T>& v) SRX_NOEXCEPT
    : x(v.x)
    , y(v.y)
    , z(0.0)
    , w(1.0)
  {}

  template<typename T>
  SRX_INLINE TVector4<T>::TVector4(const TVector3<T>& v) SRX_NOEXCEPT
    : x(v.x)
    , y(v.y)
    , z(v.z)
    , w(1.0)
  {}

  template<typename T>
  SRX_INLINE TVector4<T>::TVector4(const TVector3<T>& v, T aw) SRX_NOEXCEPT
    : x(v.x)
    , y(v.y)
    , z(v.z)
    , w(aw)
  {}

  // ============================================================================
  // Dot

  template<typename T>
  SRX_INLINE T TVector4<T>::Dot(const TVector4& q1,
                                const TVector4& q2) SRX_NOEXCEPT
  {
    return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
  }

  // ============================================================================
  // Length

  template<typename T>
  SRX_INLINE T TVector4<T>::Length() const SRX_NOEXCEPT
  {
    if constexpr (std::is_same_v<T, float>)
      return ::sqrtf(x * x + y * y + z * z + w * w);

    return sqrt(x * x + y * y + z * z + w * w);
  }

  // ============================================================================
  // Length

  template<typename T>
  SRX_INLINE T TVector4<T>::LengthSq() const SRX_NOEXCEPT
  {
    return x * x + y * y + z * z + w * w;
  }

  // ============================================================================
  // Normalize

  template<typename T>
  SRX_INLINE T TVector4<T>::Normalize() SRX_NOEXCEPT
  {
    const T len  = Length();
    const T ilen = T{ 1 } / len;
    x *= ilen;
    y *= ilen;
    z *= ilen;
    w *= ilen;

    return len;
  }

  template<typename T>
  SRX_INLINE TVector4<T> TVector4<T>::Normalize(const TVector4& q) SRX_NOEXCEPT
  {
    const float ilen = 1.0f / q.Length();
    return TVector4(q.x * ilen, q.y * ilen, q.z * ilen, q.w * ilen);
  }

  template<typename T>
  SRX_INLINE void TVector4<T>::PerspectiveDivide() SRX_NOEXCEPT
  {
    constexpr T kOne = T{ 1 };
    if (w != kOne)
    {
      const T iw = kOne / w;
      x *= iw;
      y *= iw;
      z *= iw;
      w = kOne;
    }
  }

  template<typename T>
  SRX_INLINE TVector3<T> TVector4<T>::PerspectiveDivide(const TVector4& v)
    SRX_NOEXCEPT
  {
    const T invW = T{ 1.0 } / v.w;
    return Vector3(v.x * invW, v.y * invW, v.z * invW);
  }

  template<typename T>
  SRX_INLINE TVector4<T>& TVector4<T>::operator+=(const TVector4& v)
    SRX_NOEXCEPT
  {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
  }

  template<typename T>
  SRX_INLINE TVector4<T>& TVector4<T>::operator-=(const TVector4& v)
    SRX_NOEXCEPT
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
  }

  template<typename T>
  SRX_INLINE TVector4<T>& TVector4<T>::operator*=(const TVector4& v)
    SRX_NOEXCEPT
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
    return *this;
  }

  template<typename T>
  SRX_INLINE TVector4<T>& TVector4<T>::operator*=(T factor) SRX_NOEXCEPT
  {
    x *= factor;
    y *= factor;
    z *= factor;
    w *= factor;
    return *this;
  }

  template<typename T>
  SRX_INLINE TVector4<T>& TVector4<T>::operator/=(T divisor) SRX_NOEXCEPT
  {
    const T factor = T{ 1.0 } / divisor;
    x *= factor;
    y *= factor;
    z *= factor;
    w *= factor;
    return *this;
  }

  template<typename T>
  SRX_INLINE bool operator==(const TVector4<T>& lhs,
                             const TVector4<T>& rhs) SRX_NOEXCEPT
  {
    return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z)
           && (lhs.w == rhs.w);
  }

  template<typename T>
  SRX_INLINE bool operator!=(const TVector4<T>& lhs,
                             const TVector4<T>& rhs) SRX_NOEXCEPT
  {
    return !(lhs == rhs);
  }

  template<typename T>
  SRX_INLINE TVector4<T> operator+(const TVector4<T>& lhs,
                                   const TVector4<T>& rhs) SRX_NOEXCEPT
  {
    return TVector4<T>(lhs.x + rhs.x,
                       lhs.y + rhs.y,
                       lhs.z + rhs.z,
                       lhs.w + rhs.w);
  }

  template<typename T>
  SRX_INLINE TVector4<T> operator-(const TVector4<T>& lhs,
                                   const TVector4<T>& rhs) SRX_NOEXCEPT
  {
    return TVector4<T>(lhs.x - rhs.x,
                       lhs.y - rhs.y,
                       lhs.z - rhs.z,
                       lhs.w - rhs.w);
  }

  template<typename T>
  SRX_INLINE TVector4<T> operator*(const TVector4<T>& lhs,
                                   const TVector4<T>& rhs) SRX_NOEXCEPT
  {
    return TVector4<T>(lhs.x * rhs.x,
                       lhs.y * rhs.y,
                       lhs.z * rhs.z,
                       lhs.w * rhs.w);
  }

  template<typename T>
  SRX_INLINE TVector4<T> operator*(const TVector4<T>& v,
                                   const T            factor) SRX_NOEXCEPT
  {
    return TVector4<T>(v.x * factor, v.y * factor, v.z * factor, v.w * factor);
  }

  template<typename T>
  SRX_INLINE TVector4<T> operator*(const T            factor,
                                   const TVector4<T>& v) SRX_NOEXCEPT
  {
    return v * factor;
  }

  template<typename T>
  SRX_INLINE TVector4<T> operator/(const TVector4<T>& lhs,
                                   const TVector4<T>& rhs) SRX_NOEXCEPT
  {
    return TVector4<T>(lhs.x / rhs.x,
                       lhs.y / rhs.y,
                       lhs.z / rhs.z,
                       lhs.w / rhs.w);
  }

  template<typename T>
  SRX_INLINE TVector4<T> operator/(const TVector4<T>& v,
                                   const T            divisor) SRX_NOEXCEPT
  {
    return v * (T{ 1.0 } / divisor);
  }
}

namespace Sorex
{
  typedef Math::TVector4<float> Vector4;
  typedef Vector4               Vec4;
}

typedef Sorex::Vec4 SxVec4;

#if defined(SOREX_COMPILER_MSVC)
#  pragma warning(default : 26495)
#endif
