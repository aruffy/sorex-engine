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

#include "Vector2.h"

#if defined(SOREX_COMPILER_MSVC)
#  pragma warning( \
    disable : 26495)  // @note: disable uninitialized variable warning: data
#endif

namespace Sorex::Math
{
  template<typename T>
  struct SRX_API TVector3
  {
    static_assert(std::is_floating_point_v<T>, "invalid vector value type");
    using value_type = T;

    union
    {
      struct
      {
        value_type x, y, z;
      };

      TArray<value_type, 3> data = { T{ 0 }, T{ 0 }, T{ 0 } };
    };

    TVector3() = default;
    SRX_INLINE TVector3(const T x, const T y, const T z) SRX_NOEXCEPT;

    SRX_INLINE explicit TVector3(const TVector2<T>& v) SRX_NOEXCEPT;
    SRX_INLINE TVector3(const TVector2<T>& v, const value_type z) SRX_NOEXCEPT;

    SRX_INLINE static TVector3 Zero() SRX_NOEXCEPT
    {
      return TVector3(0.f, 0.f, 0.f);
    }
    SRX_INLINE static TVector3 AxisX() SRX_NOEXCEPT
    {
      return TVector3(1.0f, 0.0f, 0.0f);
    }
    SRX_INLINE static TVector3 AxisY() SRX_NOEXCEPT
    {
      return TVector3(0.0f, 1.0f, 0.0f);
    }
    SRX_INLINE static TVector3 AxisZ() SRX_NOEXCEPT
    {
      return TVector3(0.0f, 0.0f, 1.0f);
    }

    // Compares to vectors.
    static bool IsEqual(const TVector3& v1, const TVector3& v2) SRX_NOEXCEPT;
    bool        IsEqual(const TVector3& other) const SRX_NOEXCEPT;

    // Determines the cross product of two vectors.
    static TVector3 Cross(const TVector3& v1, const TVector3& v2) SRX_NOEXCEPT;

    // Clamps components of the vector with specified min and max values.
    static TVector3 Clamp(const TVector3& v,
                          const TVector3& min,
                          const TVector3& max) SRX_NOEXCEPT;
    void Clamp(const TVector3& min, const TVector3& max) SRX_NOEXCEPT;

    // Returns the distance between two 3D points.
    static value_type Distance(const TVector3& p1,
                               const TVector3& p2) SRX_NOEXCEPT;

    // Determines the dot product of two vectors.
    static value_type Dot(const TVector3& v1, const TVector3& v2) SRX_NOEXCEPT;

    // Returns the length of the vector.
    value_type Length() const SRX_NOEXCEPT;

    // Returns the square of the length of the vector.
    value_type LengthSq() const SRX_NOEXCEPT;

    // Performs a linear interpolation between two 3*D vectors
    static TVector3 Lerp(const TVector3& v1,
                         const TVector3& v2,
                         value_type      alpha) SRX_NOEXCEPT;

    // Normalizes the vector and returns its length.
    SRX_INLINE value_type Normalize() SRX_NOEXCEPT;
    static TVector3       Normalize(const TVector3& v) SRX_NOEXCEPT;
    TVector3              Normalized() const SRX_NOEXCEPT;

    // Builds a perpendicular vector.
    static TVector3 Perpendicular(const TVector3& v) SRX_NOEXCEPT;

    // Reflects a vector off the plane with the given normal.
    void            Reflect(const TVector3& norm) SRX_NOEXCEPT;
    static TVector3 Reflect(const TVector3& v,
                            const TVector3& norm) SRX_NOEXCEPT;

    // Swaps two vectors.
    void        Swap(TVector3& v) SRX_NOEXCEPT;
    static void Swap(TVector3& v1, TVector3& v2) SRX_NOEXCEPT;

    TVector3 operator+() const SRX_NOEXCEPT { return *this; }
    TVector3 operator-() const SRX_NOEXCEPT { return Vector3(-x, -y, -z); }

    TVector3& operator+=(const TVector3& v) SRX_NOEXCEPT;
    TVector3& operator-=(const TVector3& v) SRX_NOEXCEPT;
    TVector3& operator*=(const TVector3& v) SRX_NOEXCEPT;
    TVector3& operator*=(value_type factor) SRX_NOEXCEPT;
    TVector3& operator/=(value_type divisor) SRX_NOEXCEPT;
  };

  template<typename T>
  SRX_INLINE TVector3<T>::TVector3(const T ax,
                                   const T ay,
                                   const T az) SRX_NOEXCEPT
    : x(ax)
    , y(ay)
    , z(az)
  {}

  template<typename T>
  SRX_INLINE TVector3<T>::TVector3(const TVector2<T>& v) SRX_NOEXCEPT
    : x(v.x)
    , y(v.y)
    , z(T{ 0 })
  {}


  template<typename T>
  SRX_INLINE TVector3<T>::TVector3(const TVector2<T>& v,
                                   const T            az) SRX_NOEXCEPT
    : x(v.x)
    , y(v.y)
    , z(az)
  {}

  template<typename T>
  SRX_INLINE TVector3<T> TVector3<T>::Clamp(const TVector3& v,
                                            const TVector3& min,
                                            const TVector3& max) SRX_NOEXCEPT
  {
    return TVector3(Math::Clamp(v.x, min.x, max.x),
                    Math::Clamp(v.y, min.y, max.y),
                    Math::Clamp(v.z, min.z, max.z));
  }

  template<typename T>
  SRX_INLINE void TVector3<T>::Clamp(const TVector3& min,
                                     const TVector3& max) SRX_NOEXCEPT
  {
    x = Math::Clamp(x, min.x, max.x);
    y = Math::Clamp(y, min.y, max.y);
    z = Math::Clamp(z, min.z, max.z);
  }

  template<typename T>
  SRX_INLINE T TVector3<T>::Dot(const TVector3& v1,
                                const TVector3& v2) SRX_NOEXCEPT
  {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
  }

  template<typename T>
  SRX_INLINE TVector3<T> TVector3<T>::Cross(const TVector3& v1,
                                            const TVector3& v2) SRX_NOEXCEPT
  {
    return TVector3(v1.y * v2.z - v1.z * v2.y,
                    v1.z * v2.x - v1.x * v2.z,
                    v1.x * v2.y - v1.y * v2.x);
  }

  template<typename T>
  SRX_INLINE T TVector3<T>::Length() const SRX_NOEXCEPT
  {
    if constexpr (std::is_same_v<T, float>)
      return ::sqrtf(x * x + y * y + z * z);

    return ::sqrt(x * x + y * y + z * z);
  }

  template<typename T>
  SRX_INLINE T TVector3<T>::LengthSq() const SRX_NOEXCEPT
  {
    return x * x + y * y + z * z;
  }

  template<typename T>
  SRX_INLINE T TVector3<T>::Normalize() SRX_NOEXCEPT
  {
    T len  = Length();
    T ilen = T{ 1 } / len;

    x *= ilen;
    y *= ilen;
    z *= ilen;

    return len;
  }

  template<typename T>
  SRX_INLINE TVector3<T> TVector3<T>::Normalize(const TVector3& v) SRX_NOEXCEPT
  {
    const float ilen = T{ 1 } / v.Length();
    return TVector3(v.x * ilen, v.y * ilen, v.z * ilen);
  }

  template<typename T>
  SRX_INLINE TVector3<T> TVector3<T>::Normalized() const SRX_NOEXCEPT
  {
    return Normalize(*this);
  }

  template<typename T>
  SRX_INLINE T TVector3<T>::Distance(const TVector3& p1,
                                     const TVector3& p2) SRX_NOEXCEPT
  {
    return TVector3(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z).Length();
  }

  template<typename T>
  SRX_INLINE TVector3<T> TVector3<T>::Lerp(const TVector3& left,
                                           const TVector3& right,
                                           const T         alpha) SRX_NOEXCEPT
  {
    const T oneMinusAlpha = T{ 1 } - alpha;

    return TVector3<T>(left.x * oneMinusAlpha + right.x * alpha,
                       left.y * oneMinusAlpha + right.y * alpha,
                       left.z * oneMinusAlpha + right.z * alpha);
  }

  template<typename T>
  SRX_INLINE bool TVector3<T>::IsEqual(const TVector3& vec) const SRX_NOEXCEPT
  {
    return IsEqual(*this, vec);
  }

  template<typename T>
  SRX_INLINE bool TVector3<T>::IsEqual(const TVector3& lhs,
                                       const TVector3& rhs) SRX_NOEXCEPT
  {
    return Math::IsEqual(lhs.x, rhs.x) && Math::IsEqual(lhs.y, rhs.y)
           && Math::IsEqual(lhs.z, rhs.z);
  }

  template<typename T>
  SRX_INLINE TVector3<T> TVector3<T>::Perpendicular(const TVector3& v)
    SRX_NOEXCEPT
  {
    TVector3 temp;

    // Find the smallest component.
    if (v.x < v.y)
    {
      if (v.x < v.z)
        temp.x = 1.0f;
      else
        temp.z = 1.0f;
    }
    else
    {
      if (v.y < v.z)
        temp.y = 1.0f;
      else
        temp.z = 1.0f;
    }

    // Project the point onto the plane defined by this vector.
    const T d = Dot(v, temp);

    return Normalize(
      TVector3(temp.x - d * v.x, temp.y - d * v.y, temp.z - d * v.x));
  }

  template<typename T>
  SRX_INLINE void TVector3<T>::Reflect(const TVector3& norm) SRX_NOEXCEPT
  {
    const T d = T{ 2.0 } * (x * norm.x + y * norm.y + z * norm.z);

    x = d * norm.x - x;
    y = d * norm.y - y;
    z = d * norm.z - z;
  }

  template<typename T>
  SRX_INLINE TVector3<T> TVector3<T>::Reflect(const TVector3& v,
                                              const TVector3& normal)
    SRX_NOEXCEPT
  {
    const T d = T{ 2.0 } * Dot(v, normal);
    return TVector3(d * normal.x - v.x, d * normal.y - v.y, d * normal.z - v.z);
  }


  template<typename T>
  SRX_INLINE void TVector3<T>::Swap(TVector3& v) SRX_NOEXCEPT
  {
    TVector3 t = v;
    v          = *this;
    *this      = t;
  }

  template<typename T>
  SRX_INLINE void TVector3<T>::Swap(TVector3& v1, TVector3& v2) SRX_NOEXCEPT
  {
    TVector3 t = v1;
    v1         = v2;
    v2         = t;
  }

  template<typename T>
  SRX_INLINE TVector3<T>& TVector3<T>::operator+=(const TVector3& v)
    SRX_NOEXCEPT
  {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  template<typename T>
  SRX_INLINE TVector3<T>& TVector3<T>::operator-=(const TVector3& v)
    SRX_NOEXCEPT
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  template<typename T>
  SRX_INLINE TVector3<T>& TVector3<T>::operator*=(const TVector3& v)
    SRX_NOEXCEPT
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }

  template<typename T>
  SRX_INLINE TVector3<T>& TVector3<T>::operator*=(const T factor) SRX_NOEXCEPT
  {
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
  }

  template<typename T>
  SRX_INLINE TVector3<T>& TVector3<T>::operator/=(const T divisor) SRX_NOEXCEPT
  {
    SRX_ASSERT(divisor != T{ 0 });

    const float factor = 1.f / divisor;
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
  }

  template<typename T>
  SRX_INLINE bool operator==(const TVector3<T>& lhs,
                             const TVector3<T>& rhs) SRX_NOEXCEPT
  {
    return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
  }

  template<typename T>
  SRX_INLINE bool operator!=(const TVector3<T>& lhs,
                             const TVector3<T>& rhs) SRX_NOEXCEPT
  {
    return !(lhs == rhs);
  }

  template<typename T>
  SRX_INLINE TVector3<T> operator+(const TVector3<T>& lhs,
                                   const TVector3<T>& rhs) SRX_NOEXCEPT
  {
    return TVector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
  }

  template<typename T>
  SRX_INLINE TVector3<T> operator-(const TVector3<T>& lhs,
                                   const TVector3<T>& rhs) SRX_NOEXCEPT
  {
    return TVector3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
  }

  template<typename T>
  SRX_INLINE TVector3<T> operator*(const TVector3<T>& lhs,
                                   const TVector3<T>& rhs) SRX_NOEXCEPT
  {
    return TVector3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
  }

  template<typename T>
  SRX_INLINE TVector3<T> operator*(const TVector3<T>& v,
                                   const T            factor) SRX_NOEXCEPT
  {
    return TVector3<T>(v.x * factor, v.y * factor, v.z * factor);
  }

  template<typename T>
  SRX_INLINE TVector3<T> operator*(const T            factor,
                                   const TVector3<T>& v) SRX_NOEXCEPT
  {
    return v * factor;
  }

  template<typename T>
  SRX_INLINE TVector3<T> operator/(const TVector3<T>& lhs,
                                   const TVector3<T>& rhs) SRX_NOEXCEPT
  {
    return TVector3<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
  }

  template<typename T>
  SRX_INLINE TVector3<T> operator/(const TVector3<T>& v,
                                   const T            divisor) SRX_NOEXCEPT
  {
    SRX_ASSERT(divisor != T{ 0 });
    return v * (T{ 1 } / divisor);
  }
}

namespace Sorex
{
  typedef Math::TVector3<float> Vector3;
  typedef Vector3               Vec3;
}

typedef Sorex::Vec3 SxVec3;

#if defined(SOREX_COMPILER_MSVC)
#  pragma warning(default : 26495)
#endif
