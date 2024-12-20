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

#include <Sorex/Math/SxVector3.h>
#include <Sorex/Math/SxVector4.h>

namespace Sorex
{
  struct Color
  {
    static const Color Black;
    static const Color White;
    static const Color Gray;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Cyan;
    static const Color Purple;

    union
    {
      struct
      {
        uint8 r, g, b, a;
      };

      uint32 value;
    };


    SRX_INLINE Color() SRX_NOEXCEPT;
    Color(const Color& base, int alpha) SRX_NOEXCEPT;
    Color(const int red, const int green, const int blue, const int alpha = 255)
      SRX_NOEXCEPT;

    explicit Color(const Vector3& vec3, const scalar_t alpha = 1) SRX_NOEXCEPT;
    explicit Color(const Vector4& vec4) SRX_NOEXCEPT;

    SRX_INLINE Vec4 ToVector() const SRX_NOEXCEPT
    {
      return (Vec4(r, g, b, a) / scalar_t{ std::numeric_limits<uint8>::max() });
    }
  };

  SRX_INLINE Color operator+(const Color& lhs, const Color& rhs) SRX_NOEXCEPT
  {
    return Color(static_cast<int>(lhs.r) + rhs.r,
                 static_cast<int>(lhs.g) + rhs.g,
                 static_cast<int>(lhs.b) + rhs.b,
                 static_cast<int>(lhs.a) + rhs.a);
  }

  SRX_INLINE Color operator-(const Color& lhs, const Color& rhs) SRX_NOEXCEPT
  {
    return Color(static_cast<int>(lhs.r) - rhs.r,
                 static_cast<int>(lhs.g) - rhs.g,
                 static_cast<int>(lhs.b) - rhs.b,
                 static_cast<int>(lhs.a) - rhs.a);
  }

  SRX_INLINE Color operator*(const Color&   color,
                             const scalar_t factor) SRX_NOEXCEPT
  {
    return Color(static_cast<int>(color.r * factor),
                 static_cast<int>(color.g * factor),
                 static_cast<int>(color.b * factor),
                 static_cast<int>(color.a * factor));
  }

  SRX_INLINE Color operator*(const scalar_t factor,
                             const Color&   color) SRX_NOEXCEPT
  {
    return color * factor;
  }

  SRX_INLINE Color operator/(const Color& color,
                             const float  factor) SRX_NOEXCEPT
  {
    return color * (scalar_t{ 1 } / factor);
  }

  SRX_INLINE bool operator==(const Color& lhs, const Color& rhs) SRX_NOEXCEPT
  {
    return lhs.value == rhs.value;
  }

  SRX_INLINE bool operator!=(const Color& lhs, const Color& rhs) SRX_NOEXCEPT
  {
    return !(lhs == rhs);
  }

  SRX_INLINE Color::Color() SRX_NOEXCEPT
    : r(0u)
    , g(0u)
    , b(0u)
    , a(std::numeric_limits<uint8>::max())
  {}
}

using SxColor = Sorex::Color;
