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

#include <Sorex/Color.h>

using namespace Sorex;

namespace
{
  constexpr int kByteMaxValue = std::numeric_limits<uint8>::max();

  SRX_INLINE uint8 IntToByte(const int value) SRX_NOEXCEPT
  {
    return static_cast<uint8>(Math::Clamp<int>(value, 0, kByteMaxValue));
  }

  SRX_INLINE uint8 FloatToByte(const scalar_t value) SRX_NOEXCEPT
  {
    return IntToByte(static_cast<int>(value * kByteMaxValue));
  }
}

namespace Sorex
{
  const Color Color::Black(0u, 0u, 0u);
  const Color Color::White(255u, 255u, 255u);
  const Color Color::Gray(128u, 128u, 128u);
  const Color Color::Red(255u, 0u, 0u);
  const Color Color::Green(0u, 255u, 0u);
  const Color Color::Blue(0u, 0u, 255u);
  const Color Color::Yellow(255u, 255u, 0u);
  const Color Color::Cyan(0u, 255u, 255u);
  const Color Color::Purple(255u, 0u, 255u);

  Color::Color(const Color& base, int alpha) SRX_NOEXCEPT
    : r(base.r)
    , g(base.g)
    , b(base.b)
    , a(IntToByte(alpha))
  {}

  Color::Color(const int red, const int green, const int blue, const int alpha)
    SRX_NOEXCEPT
    : r(IntToByte(red))
    , g(IntToByte(green))
    , b(IntToByte(blue))
    , a(IntToByte(alpha))
  {}

  Color::Color(const Vector3& vec3, const scalar_t alpha) SRX_NOEXCEPT
    : r(FloatToByte(vec3.x))
    , g(FloatToByte(vec3.y))
    , b(FloatToByte(vec3.z))
    , a(FloatToByte(alpha))
  {}

  Color::Color(const Vector4& vec4) SRX_NOEXCEPT
    : r(FloatToByte(vec4.x))
    , g(FloatToByte(vec4.y))
    , b(FloatToByte(vec4.z))
    , a(FloatToByte(vec4.w))
  {}
}
