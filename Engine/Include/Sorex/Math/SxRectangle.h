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

#include <Sorex/Math/SxPoint.h>
#include <Sorex/Math/SxSize.h>

namespace Sorex::Math
{
  template<typename T>
  struct TIntegerRectangle
  {
    static_assert(TIsIntegral_Value<T>,
                  "[TIntegerRectangle] Invalid template type");

    T x = 0, y = 0;
    T width = 0, height = 0;

    TIntegerRectangle() = default;
    TIntegerRectangle(T aX, T aY, T w, T h)
      : x(aX)
      , y(aY)
      , width(w)
      , height(h)
    {}

    TIntegerRectangle(const TIntegerPoint<T>& loc, const TIntegerSize<T>& size)
      : x(loc.x)
      , y(loc.y)
      , width(size.width)
      , height(size.height)
    {}

    inline TIntegerPoint<T> GetLocation() const
    {
      return TIntegerPoint<T>(x, y);
    }
    inline TIntegerSize<T> GetSize() const
    {
      return TIntegerSize<T>(width, height);
    }

    bool              Intersects(const TIntegerRectangle& rect) const noexcept;
    TIntegerRectangle Intersection(
      const TIntegerRectangle& rect) const noexcept;

    bool Contains(const TIntegerPoint<T>& p) const
    {
      return Contains(p.x, p.y);
    }
    bool Contains(const TIntegerRectangle& rect) const;
    bool Contains(T _x, T _y) const noexcept;

    void Expand(const TIntegerPoint<T>& point);
    void Squeeze(T _x, T _y);

    inline TIntegerPoint<T> TopLeft() const { return TIntegerPoint<T>(x, y); }
    inline TIntegerPoint<T> TopRight() const
    {
      return TIntegerPoint<T>(x + width, y);
    }
    inline TIntegerPoint<T> BottomLeft() const
    {
      return TIntegerPoint<T>(x, y + height);
    }
    inline TIntegerPoint<T> BottomRight() const
    {
      return TIntegerPoint<T>(x + width, y + height);
    }
    inline TIntegerPoint<T> Center() const
    {
      return TIntegerPoint<T>(x + width / T{ 2 }, y + height / T{ 2 });
    }

    bool operator==(const TIntegerRectangle& rc) const
    {
      return ((x == rc.x) && (y == rc.y) && (width == rc.width)
              && (height == rc.height));
    }
    bool operator!=(const TIntegerRectangle& rc) const
    {
      return !(*this == rc);
    }
  };

  template<typename T>
  inline TIntegerRectangle<T> operator*(const TIntegerRectangle<T>& rc, T f)
  {
    return { rc.x * f, rc.y * f, rc.width * f, rc.height * f };
  }

  template<typename T>
  inline TIntegerRectangle<T> operator*(T f, const TIntegerRectangle<T>& rc)
  {
    return operator*(rc, f);
  }

  template<typename T>
  bool TIntegerRectangle<T>::Intersects(
    const TIntegerRectangle<T>& rect) const noexcept
  {
    if (x + width <= rect.x)
      return false;

    if (x >= rect.x + rect.width)
      return false;

    if (y + height <= rect.y)
      return false;

    if (y >= rect.y + rect.height)
      return false;

    return true;
  }

  template<typename T>
  TIntegerRectangle<T> TIntegerRectangle<T>::Intersection(
    const TIntegerRectangle<T>& other) const noexcept
  {
    constexpr T kNull = T{ 0 };

    T x1 = std::max<T>(x, other.x);
    T x2 = std::min<T>(x + width, other.x + other.width);

    T y1 = std::max<T>(y, other.y);
    T y2 = std::min<T>(y + height, other.y + other.height);

    if (((x2 - x1) < kNull) || ((y2 - y1) < kNull))
      return TIntegerRectangle<T>();
    else
      return TIntegerRectangle<T>(x1, y1, x2 - x1, y2 - y1);
  }

  template<typename T>
  bool TIntegerRectangle<T>::Contains(T _x, T _y) const noexcept
  {
    return (_x >= x && _y >= y && _x < (x + width) && _y < (y + height));
  }

  template<typename T>
  bool TIntegerRectangle<T>::Contains(const TIntegerRectangle<T>& rect) const
  {
    return (x <= rect.x) && (rect.x + rect.width <= x + width) && (y <= rect.y)
           && (rect.y + rect.height <= y + height);
  }

  template<typename T>
  void TIntegerRectangle<T>::Expand(const TIntegerPoint<T>& p)
  {
    if (p.x < x)
      x = p.x;
    else if (p.x - x > width)
      width = p.x - x;

    if (p.y < y)
      y = p.y;
    else if (p.y - y > height)
      height = p.y - y;
  }

  template<typename T>
  void TIntegerRectangle<T>::Squeeze(T _x, T _y)
  {
    constexpr T kNull   = T{ 0 };
    constexpr T kDouble = T{ 2 };

    x += _x;
    width -= _x * kDouble;

    y += _y;
    height -= _y * kDouble;

    if (width < kNull)
      width = kNull;

    if (height < kNull)
      height = kNull;
  }

  // --------------- Floating Rectangle ---------------- //

  template<typename T>
  struct TFloatingRectangle
  {
    static_assert(std::is_floating_point_v<T>,
                  "[TFloatingRectangle] Invalid template type");

    T x = 0, y = 0;
    T width = 0, height = 0;

    TFloatingRectangle() = default;
    SRX_INLINE TFloatingRectangle(const T aX,
                                  const T aY,
                                  const T aW,
                                  const T aH) SRX_NOEXCEPT
      : x(aX)
      , y(aY)
      , width(aW)
      , height(aH)
    {}

    SRX_INLINE TFloatingRectangle(const TFloatingPoint<T>& loc,
                                  const TFloatingSize<T>&  size) SRX_NOEXCEPT
      : x(loc.x)
      , y(loc.y)
      , width(size.width)
      , height(size.height)
    {}

    SRX_INLINE TFloatingPoint<T> GetLocation() const SRX_NOEXCEPT
    {
      return TFloatingPoint<T>(x, y);
    }
    SRX_INLINE TFloatingSize<T> GetSize() const SRX_NOEXCEPT
    {
      return TFloatingSize<T>(width, height);
    }

    SRX_INLINE void SetLocation(const TFloatingPoint<T>& loc) SRX_NOEXCEPT
    {
      SetLocation(loc.x, loc.y);
    }

    SRX_INLINE void SetLocation(const T aX, const T aY) SRX_NOEXCEPT
    {
      x = aX;
      y = aY;
    }

    SRX_INLINE void SetSize(const TFloatingSize<T>& size) SRX_NOEXCEPT
    {
      SetSize(size.width, size.height);
    }

    SRX_INLINE void SetSize(const T w, const T h) SRX_NOEXCEPT
    {
      width  = w;
      height = h;
    }

    SRX_INLINE bool IsEqual(const TFloatingRectangle& other) const SRX_NOEXCEPT
    {
      return (Math::IsEqual(x, other.x) && Math::IsEqual(y, other.y)
              && Math::IsEqual(width, other.width)
              && Math::IsEqual(height, other.height));
    }

    bool Intersects(const TFloatingRectangle& rect) const SRX_NOEXCEPT;
    TFloatingRectangle Intersection(const TFloatingRectangle& rect) const
      SRX_NOEXCEPT;

    SRX_INLINE bool Contains(const TFloatingPoint<T>& p) const SRX_NOEXCEPT
    {
      return Contains(p.x, p.y);
    }

    bool Contains(const TFloatingRectangle& rect) const SRX_NOEXCEPT;
    bool Contains(const T aX, const T aY) const SRX_NOEXCEPT;

    void Expand(const TFloatingPoint<T>& point) SRX_NOEXCEPT;
    void Squeeze(const T aX, const T aY) SRX_NOEXCEPT;

    SRX_INLINE TFloatingPoint<T> TopLeft() const SRX_NOEXCEPT
    {
      return TFloatingPoint<T>(x, y);
    }
    SRX_INLINE TFloatingPoint<T> TopRight() const SRX_NOEXCEPT
    {
      return TFloatingPoint<T>(x + width, y);
    }
    SRX_INLINE TFloatingPoint<T> BottomLeft() const SRX_NOEXCEPT
    {
      return TFloatingPoint<T>(x, y + height);
    }
    SRX_INLINE TFloatingPoint<T> BottomRight() const SRX_NOEXCEPT
    {
      return TFloatingPoint<T>(x + width, y + height);
    }
    SRX_INLINE TFloatingPoint<T> Center() const SRX_NOEXCEPT
    {
      return TFloatingPoint<T>(x + width / T{ 2.f }, y + height / T{ 2.f });
    }

    SRX_INLINE void ToArray(TArray<TFloatingPoint<T>, 4>& points) const
      SRX_NOEXCEPT;

    SRX_INLINE bool operator==(const TFloatingRectangle& rc) const SRX_NOEXCEPT
    {
      return ((x == rc.x) && (y == rc.y) && (width == rc.width)
              && (height == rc.height));
    }
    SRX_INLINE bool operator!=(const TFloatingRectangle& rc) const SRX_NOEXCEPT
    {
      return !(*this == rc);
    }
  };

  template<typename T>
  SRX_INLINE TFloatingRectangle<T> operator*(const TFloatingRectangle<T>& rc,
                                             const T f) SRX_NOEXCEPT
  {
    return TFloatingRectangle<T>(rc.x * f,
                                 rc.y * f,
                                 rc.width * f,
                                 rc.height * f);
  }

  template<typename T>
  SRX_INLINE TFloatingRectangle<T> operator*(const T                      f,
                                             const TFloatingRectangle<T>& rc)
    SRX_NOEXCEPT
  {
    return operator*(rc, f);
  }

  template<typename T>
  bool TFloatingRectangle<T>::Intersects(
    const TFloatingRectangle<T>& rect) const SRX_NOEXCEPT
  {
    if (x + width <= rect.x)
      return false;

    if (x >= rect.x + rect.width)
      return false;

    if (y + height <= rect.y)
      return false;

    if (y >= rect.y + rect.height)
      return false;

    return true;
  }

  template<typename T>
  TFloatingRectangle<T> TFloatingRectangle<T>::Intersection(
    const TFloatingRectangle<T>& other) const SRX_NOEXCEPT
  {
    constexpr T kNull = T{ 0 };

    const T x1 = std::max(x, other.x);
    const T x2 = std::min(x + width, other.x + other.width);

    const T y1 = std::max(y, other.y);
    const T y2 = std::min(y + height, other.y + other.height);

    if (((x2 - x1) < kNull) || ((y2 - y1) < kNull))
      return TFloatingRectangle<T>();
    else
      return TFloatingRectangle<T>(x1, y1, x2 - x1, y2 - y1);
  }

  template<typename T>
  bool TFloatingRectangle<T>::Contains(const T _x, T _y) const SRX_NOEXCEPT
  {
    return (_x >= x && _y >= y && _x < (x + width) && _y < (y + height));
  }

  template<typename T>
  bool TFloatingRectangle<T>::Contains(const TFloatingRectangle<T>& rect) const
    SRX_NOEXCEPT
  {
    return (x <= rect.x) && (rect.x + rect.width <= x + width) && (y <= rect.y)
           && (rect.y + rect.height <= y + height);
  }

  template<typename T>
  void TFloatingRectangle<T>::Expand(const TFloatingPoint<T>& p) SRX_NOEXCEPT
  {
    if (p.x < x)
      x = p.x;
    else if (p.x - x > width)
      width = p.x - x;

    if (p.y < y)
      y = p.y;
    else if (p.y - y > height)
      height = p.y - y;
  }

  template<typename T>
  void TFloatingRectangle<T>::Squeeze(T _x, T _y) SRX_NOEXCEPT
  {
    constexpr T kNull   = T{ 0 };
    constexpr T kDouble = T{ 2 };

    x += _x;
    width -= _x * kDouble;

    y += _y;
    height -= _y * kDouble;

    if (width < kNull)
      width = kNull;

    if (height < kNull)
      height = kNull;
  }

  template<typename T>
  SRX_INLINE void TFloatingRectangle<T>::ToArray(
    TArray<TFloatingPoint<T>, 4>& points) const SRX_NOEXCEPT
  {
    points = { TopLeft(), BottomLeft(), BottomRight(), TopRight() };
  }
}

namespace Sorex
{
  typedef Math::TIntegerRectangle<int32>            RectInt;
  typedef Math::TFloatingRectangle<Sorex::scalar_t> Rectangle;
  typedef Rectangle                                 Rect;
}

using SxRect = Sorex::Rect;
