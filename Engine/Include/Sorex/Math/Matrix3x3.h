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

#if defined(SOREX_COMPILER_MSVC)
#  pragma warning(push)
#  pragma warning( \
    disable : 4201)  // nonstandard extension used: nameless struct/union
#endif

#include "Vector2.h"
#include "Vector3.h"
#include "Point.h"

namespace Sorex::Math
{
  template<typename T>
  struct TMatrix3x3
  {
    static_assert(std::is_floating_point_v<T>, "Invalid matrix value type");
    using value_type = T;

    SRX_INLINE TMatrix3x3() SRX_NOEXCEPT;
    SRX_INLINE explicit TMatrix3x3(value_type value) SRX_NOEXCEPT;
    SRX_INLINE explicit TMatrix3x3(const value_type* ptr) SRX_NOEXCEPT;

    SRX_INLINE value_type*       begin() noexcept { return &_00; }
    SRX_INLINE const value_type* cbegin() const noexcept { return &_00; }

    SRX_INLINE value_type*       end() noexcept { return (&_22) + 1; }
    SRX_INLINE const value_type* cend() const noexcept { return (&_22) + 1; }

    TMatrix3x3(const TMatrix3x3& other) SRX_NOEXCEPT;
    TMatrix3x3& operator=(const TMatrix3x3& other) SRX_NOEXCEPT;

    /**
     * @brief Create transformation matrix with the TRS order (Translation,
     * Rotation, Scaling).
     *
     * @param translation
     * @param rotation
     * @param scale
     *
     * @return trasformation matrix
     */
    static TMatrix3x3 Create(const TVector2<T>& translation,
                             const T            rotation,
                             const TVector2<T>& scale) SRX_NOEXCEPT;
    static TMatrix3x3 Translation(const value_type tx,
                                  const value_type ty) SRX_NOEXCEPT;
    static TMatrix3x3 Rotation(value_type radians) SRX_NOEXCEPT;
    static TMatrix3x3 Scaling(const value_type sx,
                              const value_type sy) SRX_NOEXCEPT;

    static TMatrix3x3 Translate(const TMatrix3x3&  m,
                                const TVector2<T>& v) SRX_NOEXCEPT;
    void              Translate(const T x, const T y) SRX_NOEXCEPT;
    SRX_INLINE void   Translate(const TVector2<T>& v) SRX_NOEXCEPT
    {
      Translate(v.x, v.y);
    }

    static TMatrix3x3 Scale(const TMatrix3x3&  m,
                            const TVector2<T>& v) SRX_NOEXCEPT;
    SRX_INLINE void   Scale(const value_type sx,
                            const value_type sy) SRX_NOEXCEPT;
    SRX_INLINE void   Scale(const TVector2<T>& v) SRX_NOEXCEPT
    {
      Scale(v.x, v.y);
    }

    static TMatrix3x3 Rotate(const TMatrix3x3& m,
                             const value_type  radians) SRX_NOEXCEPT;
    void              Rotate(const value_type radians) SRX_NOEXCEPT;

    SRX_INLINE const value_type* GetValuePtr() const { return cbegin(); }

    SRX_INLINE static const TMatrix3x3& Identity() SRX_NOEXCEPT;
    SRX_INLINE void                     SetIdentity() SRX_NOEXCEPT;

    SRX_INLINE void Fill(const value_type value) SRX_NOEXCEPT
    {
      std::fill_n(begin(), 3 * 3, value);
    }

    SRX_INLINE value_type Get(const size_t i, const size_t j) const SRX_NOEXCEPT
    {
      return _data[i][j];
    }

    SRX_INLINE void Set(const size_t     i,
                        const size_t     j,
                        const value_type val) const SRX_NOEXCEPT
    {
      return _data[i][j] = val;
    }

    SRX_INLINE TVector3<T> GetRow(size_t m) const SRX_NOEXCEPT;
    SRX_INLINE void        SetRow(size_t m, const TVector3<T>& v) SRX_NOEXCEPT;

    SRX_INLINE TVector3<T> GetColumn(size_t n) const SRX_NOEXCEPT;
    SRX_INLINE void SetColumn(size_t m, const TVector3<T>& v) SRX_NOEXCEPT;

    static TMatrix3x3 Add(const TMatrix3x3& m1,
                          const TMatrix3x3& m2) SRX_NOEXCEPT;
    void              Add(const TMatrix3x3& mat) SRX_NOEXCEPT;

    static TMatrix3x3 Subtract(const TMatrix3x3& m1,
                               const TMatrix3x3& m2) SRX_NOEXCEPT;
    void              Subtract(const TMatrix3x3& m) SRX_NOEXCEPT;

    static TMatrix3x3 Multiply(const TMatrix3x3& m1,
                               const TMatrix3x3& m2) SRX_NOEXCEPT;
    void              Multiply(const TMatrix3x3& m) SRX_NOEXCEPT;

    value_type Determinant() const SRX_NOEXCEPT;

    static TMatrix3x3 Invert(const TMatrix3x3& m) SRX_NOEXCEPT;
    void              Invert() SRX_NOEXCEPT;

    static TVector2<T>       Transform(const TMatrix3x3&  m,
                                       const TVector2<T>& v) SRX_NOEXCEPT;
    static TVector3<T>       Transform(const TMatrix3x3&  m,
                                       const TVector3<T>& v) SRX_NOEXCEPT;
    static TFloatingPoint<T> Transform(const TMatrix3x3&        m,
                                       const TFloatingPoint<T>& point)
      SRX_NOEXCEPT;

    static TMatrix3x3 Transpose(const TMatrix3x3& m) SRX_NOEXCEPT;
    void              Transpose() SRX_NOEXCEPT;

    SRX_INLINE TVector3<T> operator[](const size_t i) const SRX_NOEXCEPT
    {
      return TVector3<T>(_data[i][0], _data[i][1], _data[i][2]);
    }

    SRX_INLINE TMatrix3x3& operator+=(const TMatrix3x3& m) SRX_NOEXCEPT
    {
      Add(m);
      return *this;
    }

    SRX_INLINE TMatrix3x3& operator-=(const TMatrix3x3& m) SRX_NOEXCEPT
    {
      Subtract(m);
      return *this;
    }

    SRX_INLINE TMatrix3x3& operator*=(const TMatrix3x3& m) SRX_NOEXCEPT
    {
      Multiply(m);
      return *this;
    }

private:
    union
    {
      struct
      {
        value_type _00, _01, _02;
        value_type _10, _11, _12;
        value_type _20, _21, _22;
      };

      value_type _data[3][3];
    };

    SRX_INLINE TMatrix3x3(const value_type a00,
                          const value_type a01,
                          const value_type a02,
                          const value_type a10,
                          const value_type a11,
                          const value_type a12,
                          const value_type a20,
                          const value_type a21,
                          const value_type a22) SRX_NOEXCEPT;
  };

  template<typename T>
  SRX_INLINE TMatrix3x3<T>::TMatrix3x3() SRX_NOEXCEPT
  {
    Fill(T{ 0 });
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T>::TMatrix3x3(value_type value) SRX_NOEXCEPT
    : TMatrix3x3()
  {
    _00 = _11 = _22 = value;
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T>::TMatrix3x3(const value_type a00,
                                       const value_type a01,
                                       const value_type a02,
                                       const value_type a10,
                                       const value_type a11,
                                       const value_type a12,
                                       const value_type a20,
                                       const value_type a21,
                                       const value_type a22) SRX_NOEXCEPT
    : _00(a00)
    , _01(a01)
    , _02(a02)
    , _10(a10)
    , _11(a11)
    , _12(a12)
    , _20(a20)
    , _21(a21)
    , _22(a22)
  {}

  template<typename T>
  SRX_INLINE TMatrix3x3<T>::TMatrix3x3(const value_type* ptr) SRX_NOEXCEPT

  {
    std::copy_n(ptr, 3 * 3, begin());
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T>::TMatrix3x3(const TMatrix3x3& other) SRX_NOEXCEPT
  {
    std::copy_n(other.cbegin(), 3 * 3, begin());
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T>& TMatrix3x3<T>::operator=(const TMatrix3x3& other)
    SRX_NOEXCEPT
  {
    std::copy_n(other.cbegin(), 3 * 3, begin());
    return *this;
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T> TMatrix3x3<T>::Translation(const T tx,
                                                      const T ty) SRX_NOEXCEPT
  {
    constexpr T kZero = T{ 0 };
    constexpr T kOne  = T{ 1 };
    return TMatrix3x3<T>(kOne, kZero, kZero, kZero, kOne, kZero, tx, ty, kOne);
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T> TMatrix3x3<T>::Rotation(const T angle) SRX_NOEXCEPT
  {
    constexpr T kZero = T{ 0 };

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

    return TMatrix3x3(c, s, kZero, -s, c, kZero, kZero, kZero, T{ 1 });
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T> TMatrix3x3<T>::Scaling(const T sx,
                                                  const T sy) SRX_NOEXCEPT
  {
    constexpr T kZero = T{ 0 };
    constexpr T kOne  = T{ 1 };

    return TMatrix3x3<T>(sx,
                         kZero,
                         kZero,
                         kZero,
                         sy,
                         kZero,
                         kZero,
                         kZero,
                         kOne);
  }

  template<typename T>
  TMatrix3x3<T> TMatrix3x3<T>::Translate(const TMatrix3x3&  m,
                                         const TVector2<T>& v) SRX_NOEXCEPT
  {
    TMatrix3x3 result(m);
    result.Translate(v);
    return result;
  }

  template<typename T>
  void TMatrix3x3<T>::Translate(const T x, const T y) SRX_NOEXCEPT
  {
    const Vec3 t =
      this->operator[](0) * x + this->operator[](1) * y + this->operator[](2);
    SetRow(2, t);
  }

  template<typename T>
  TMatrix3x3<T> TMatrix3x3<T>::Scale(const TMatrix3x3&  m,
                                     const TVector2<T>& v) SRX_NOEXCEPT
  {
    TMatrix3x3 result(m);
    result.Scale(v);
    return result;
  }

  template<typename T>
  SRX_INLINE void TMatrix3x3<T>::Scale(const T sx, const T sy) SRX_NOEXCEPT
  {
    _00 *= sx;
    _01 *= sx;
    _02 *= sx;

    _10 *= sy;
    _11 *= sy;
    _12 *= sy;
  }

  template<typename T>
  TMatrix3x3<T> TMatrix3x3<T>::Rotate(const TMatrix3x3& m,
                                      const value_type  radians) SRX_NOEXCEPT
  {
    TMatrix3x3 result(m);
    result.Rotate(radians);
    return result;
  }

  template<typename T>
  void TMatrix3x3<T>::Rotate(const value_type radians) SRX_NOEXCEPT
  {
    T s, c;
    if constexpr (std::is_same_v<T, float>)
    {
      s = ::sinf(radians);
      c = ::cosf(radians);
    }
    else
    {
      s = ::sin(radians);
      c = ::cos(radians);
    }

    const Vec3 r1 = GetRow(0);
    const Vec3 r2 = GetRow(1);

    SetRow(0, r1 * c + r2 * s);
    SetRow(1, r1 * (-s) + r2 * c);
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T> TMatrix3x3<T>::Create(const TVector2<T>& translation,
                                                 const T            rotation,
                                                 const TVector2<T>& scale)
    SRX_NOEXCEPT
  {
    constexpr T kZero = T{ 0 };

    T sine, cosine;
    if constexpr (std::is_same_v<T, float>)
    {
      sine   = ::sinf(rotation);
      cosine = ::cosf(rotation);
    }
    else
    {
      sine   = ::sin(rotation);
      cosine = ::cos(rotation);
    }

    return TMatrix3x3<T>(cosine * scale.x,
                         sine * scale.x,
                         kZero,
                         -sine * scale.y,
                         cosine * scale.y,
                         kZero,
                         translation.x,
                         translation.y,
                         T{ 1.0 });
  }

  template<typename T>
  SRX_INLINE const TMatrix3x3<T>& TMatrix3x3<T>::Identity() SRX_NOEXCEPT
  {
    const static TMatrix3x3 mat3(T{ 1 });
    return mat3;
  }

  template<typename T>
  SRX_INLINE void TMatrix3x3<T>::SetIdentity() SRX_NOEXCEPT
  {
    *this = Identity();
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T> TMatrix3x3<T>::Add(const TMatrix3x3& m1,
                                              const TMatrix3x3& m2) SRX_NOEXCEPT
  {
    return TMatrix3x3(m1._00 + m2._00,
                      m1._01 + m2._01,
                      m1._02 + m2._02,
                      m1._10 + m2._10,
                      m1._11 + m2._11,
                      m1._12 + m2._12,
                      m1._20 + m2._20,
                      m1._21 + m2._21,
                      m1._22 + m2._22);
  }

  template<typename T>
  SRX_INLINE void TMatrix3x3<T>::Add(const TMatrix3x3& m) SRX_NOEXCEPT
  {
    _00 += m._00;
    _01 += m._01;
    _02 += m._02;
    _10 += m._10;
    _11 += m._11;
    _12 += m._12;
    _20 += m._20;
    _21 += m._21;
    _22 += m._22;
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T> TMatrix3x3<T>::Subtract(const TMatrix3x3<T>& m1,
                                                   const TMatrix3x3<T>& m2)
    SRX_NOEXCEPT
  {
    return TMatrix3x3(m1._00 - m2._00,
                      m1._01 - m2._01,
                      m1._02 - m2._02,
                      m1._10 - m2._10,
                      m1._11 - m2._11,
                      m1._12 - m2._12,
                      m1._20 - m2._20,
                      m1._21 - m2._21,
                      m1._22 - m2._22);
  }

  template<typename T>
  SRX_INLINE void TMatrix3x3<T>::Subtract(const TMatrix3x3& m) SRX_NOEXCEPT
  {
    _00 -= m._00;
    _01 -= m._01;
    _02 -= m._02;
    _10 -= m._10;
    _11 -= m._11;
    _12 -= m._12;
    _20 -= m._20;
    _21 -= m._21;
    _22 -= m._22;
  }

  template<typename T>
  SRX_INLINE T TMatrix3x3<T>::Determinant() const SRX_NOEXCEPT
  {
    return _00 * (_11 * _22 - _12 * _21) - _10 * (_01 * _22 - _02 * _21)
           + _20 * (_01 * _12 - _11 * _02);
  }

  template<typename T>
  SRX_INLINE TVector3<T> TMatrix3x3<T>::GetRow(const size_t i) const
    SRX_NOEXCEPT
  {
    SRX_ASSERT(i < 3ULL);
    return TVector3<T>(_data[i][0], _data[i][1], _data[i][2]);
  }

  template<typename T>
  SRX_INLINE void TMatrix3x3<T>::SetRow(const size_t       i,
                                        const TVector3<T>& v) SRX_NOEXCEPT
  {
    SRX_ASSERT(i < 3ULL);
    _data[i][0] = v.x;
    _data[i][1] = v.y;
    _data[i][2] = v.z;
  }

  template<typename T>
  SRX_INLINE TVector3<T> TMatrix3x3<T>::GetColumn(const size_t i) const
    SRX_NOEXCEPT
  {
    SRX_ASSERT(i < 3ULL);
    return TVector3<T>(_data[0][i], _data[1][i], _data[2][i]);
  }

  template<typename T>
  SRX_INLINE void TMatrix3x3<T>::SetColumn(const size_t       i,
                                           const TVector3<T>& v) SRX_NOEXCEPT
  {
    SRX_ASSERT(i < 3ULL);
    _data[0][i] = v.x;
    _data[1][i] = v.y;
    _data[2][i] = v.z;
  }

  template<typename T>
  SRX_INLINE void TMatrix3x3<T>::Invert() SRX_NOEXCEPT
  {
    *this = Invert(*this);
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T> TMatrix3x3<T>::Invert(const TMatrix3x3& m)
    SRX_NOEXCEPT
  {
    const T idet = T{ 1 } / m.Determinant();

    return TMatrix3x3((m._11 * m._22 - m._12 * m._21) * idet,
                      (m._02 * m._21 - m._01 * m._22) * idet,
                      (m._01 * m._12 - m._02 * m._11) * idet,
                      (m._12 * m._20 - m._10 * m._22) * idet,
                      (m._00 * m._22 - m._02 * m._20) * idet,
                      (m._02 * m._10 - m._00 * m._12) * idet,
                      (m._10 * m._21 - m._11 * m._20) * idet,
                      (m._01 * m._20 - m._00 * m._21) * idet,
                      (m._00 * m._11 - m._01 * m._10) * idet);
  }

  template<typename T>
  SRX_INLINE void TMatrix3x3<T>::Transpose() SRX_NOEXCEPT
  {
    *this = Transpose(*this);
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T> TMatrix3x3<T>::Transpose(const TMatrix3x3& m)
    SRX_NOEXCEPT
  {
    return TMatrix3x3(m._00,
                      m._10,
                      m._20,
                      m._01,
                      m._11,
                      m._21,
                      m._02,
                      m._12,
                      m._22);
  }

  template<typename T>
  SRX_INLINE void TMatrix3x3<T>::Multiply(const TMatrix3x3& m) SRX_NOEXCEPT
  {
    *this = Multiply(*this, m);
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T> TMatrix3x3<T>::Multiply(const TMatrix3x3& m1,
                                                   const TMatrix3x3& m2)
    SRX_NOEXCEPT
  {
    return TMatrix3x3(m1._00 * m2._00 + m1._10 * m2._01 + m1._20 * m2._02,
                      m1._01 * m2._00 + m1._11 * m2._01 + m1._21 * m2._02,
                      m1._02 * m2._00 + m1._12 * m2._01 + m1._22 * m2._02,

                      m1._00 * m2._10 + m1._10 * m2._11 + m1._20 * m2._12,
                      m1._01 * m2._10 + m1._11 * m2._11 + m1._21 * m2._12,
                      m1._02 * m2._10 + m1._12 * m2._11 + m1._22 * m2._12,

                      m1._00 * m2._20 + m1._10 * m2._21 + m1._20 * m2._22,
                      m1._01 * m2._20 + m1._11 * m2._21 + m1._21 * m2._22,
                      m1._02 * m2._20 + m1._12 * m2._21 + m1._22 * m2._22);
  }

  template<typename T>
  SRX_INLINE TVector3<T> TMatrix3x3<T>::Transform(const TMatrix3x3<T>& m,
                                                  const TVector3<T>&   v)
    SRX_NOEXCEPT
  {
    return TVector3<T>(v.x * m._00 + v.y * m._10 + v.z * m._20,
                       v.x * m._01 + v.y * m._11 + v.z * m._21,
                       v.x * m._02 + v.y * m._12 + v.z * m._22);
  }

  template<typename T>
  SRX_INLINE TFloatingPoint<T> TMatrix3x3<T>::Transform(
    const TMatrix3x3&        m,
    const TFloatingPoint<T>& pt) SRX_NOEXCEPT
  {
    return TFloatingPoint<T>(pt.x * m._00 + pt.y * m._10 + m._20,
                             pt.x * m._01 + pt.y * m._11 + m._21);
  }

  template<typename T>
  SRX_INLINE TVector2<T> TMatrix3x3<T>::Transform(const TMatrix3x3&  m,
                                                  const TVector2<T>& v)
    SRX_NOEXCEPT
  {
    return TVector2<T>(v.x * m._00 + v.y * m._10 + m._20,
                       v.x * m._01 + v.y * m._11 + m._21);
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T> operator+(const TMatrix3x3<T>& lhs,
                                     const TMatrix3x3<T>& rhs) SRX_NOEXCEPT
  {
    return TMatrix3x3<T>::Add(lhs, rhs);
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T> operator-(const TMatrix3x3<T>& lhs,
                                     const TMatrix3x3<T>& rhs) SRX_NOEXCEPT
  {
    return TMatrix3x3<T>::Subtract(lhs, rhs);
  }

  template<typename T>
  SRX_INLINE TMatrix3x3<T> operator*(const TMatrix3x3<T>& lhs,
                                     const TMatrix3x3<T>& rhs) SRX_NOEXCEPT
  {
    return TMatrix3x3<T>::Multiply(lhs, rhs);
  }

  template<typename T>
  SRX_INLINE TVector2<T> operator*(const TVector2<T>&   v,
                                   const TMatrix3x3<T>& m) SRX_NOEXCEPT
  {
    return TMatrix3x3<T>::Transform(m, v);
  }

  template<typename T>
  SRX_INLINE TVector3<T> operator*(const TVector3<T>&   v,
                                   const TMatrix3x3<T>& m) SRX_NOEXCEPT
  {
    return TMatrix3x3<T>::Transform(m, v);
  }
}

namespace Sorex
{
  using Mat3 = Math::TMatrix3x3<scalar_t>;
}

using SxMat3 = Sorex::Mat3;

#if defined(SOREX_COMPILER_MSVC)
#  pragma warning(pop)
#endif
