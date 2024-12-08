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

#include <Sorex/CoreMinimal.h>

namespace Sorex::Utils
{
  SRX_API SRX_INLINE bool IsLittleEndian() SRX_NOEXCEPT
  {
    const uint16      x = 0x0001;
    static const bool endian =
      static_cast<bool>(*(reinterpret_cast<const uint8*>(&x)));
    return endian;
  }

  template<std::integral T>
  SRX_API T SwapBits(T val) SRX_NOEXCEPT
  {
    T                result = T{ 0 };
    constexpr size_t bits   = 8 * sizeof(val);
    for (size_t i = 0; i < bits; ++i)
    {
      result = (result << 1) | (val & 1);
      val >>= 1;
    }

    return result;
  }

  template<std::integral T>
  SRX_API T SwapBytes(T val) SRX_NOEXCEPT
  {
    if constexpr (sizeof(T) == 1)
      return val;

    if constexpr (sizeof(T) == 2 && std::is_unsigned_v<T>)
      return static_cast<T>((val >> 8) | (val << 8));

    auto*            b    = reinterpret_cast<byte*>(&val);
    constexpr size_t half = sizeof(T) / 2;
    constexpr size_t last = sizeof(T) - 1;

    for (size_t i = 0; i < half; ++i)
      std::swap(b[i], b[last - i]);

    return val;
  }
}  // namespace
