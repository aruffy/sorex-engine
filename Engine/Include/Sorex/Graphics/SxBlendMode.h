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

namespace Sorex::Graphics
{
  // Cr = Cs * Fs + Cd * Fd;
  // Cs - the source color vector;
  // Cd - the destination color vector. (in buffer)
  // Fs - the source factor alpha factor;
  // Cs - the destenation alpha factor;
  class BlendMode
  {
public:
    enum class EFactor : uint8
    {
      Zero = 1u,
      One,
      Src_Color,
      One_Minus_Src_Color,
      Dst_Color,
      One_Minus_Dst_Color,
      Src_Alpha,
      One_Minus_Src_Alpha,
      Dst_Alpha,
      One_Minus_Dst_Alpha,
      Const_Color,
      One_Minus_Const_Color,
      Const_Alpha,
      One_Minus_Const_Alpha
    };

    enum class EOperation : uint8
    {
      Add = 1u,
      Subtract,
      Reverse_Subtract,
      Min,
      Max
    };


public:
    static const BlendMode None;
    static const BlendMode Alpha;
    static const BlendMode Additive;
    static const BlendMode Multiply;

    BlendMode();
    explicit BlendMode(uint32 value);

    BlendMode(EFactor sfactor, EFactor dfactor, EOperation op);
    BlendMode(EFactor    srcFactor,
              EFactor    dstFactor,
              EOperation op,
              EFactor    srcAlphaFactor,
              EFactor    dstAlphaFactor,
              EOperation opAlpha);

    BlendMode(const BlendMode& other) SRX_NOEXCEPT;
    BlendMode& operator=(const BlendMode& other) SRX_NOEXCEPT;

    bool IsEnable() const { return mValue; }
    bool IsSeparate() const { return bSeparate; }

    EOperation GetColorOperation() const
    {
      return static_cast<EOperation>(operations >> 4);
    }

    EOperation GetAlphaOperation() const
    {
      return static_cast<EOperation>(operations & 0xf);
    }

    EFactor GetSrcFactor() const { return static_cast<EFactor>(cfactors >> 4); }

    EFactor GetDstFactor() const
    {
      return static_cast<EFactor>(cfactors & 0xf);
    }

    EFactor GetSrcAlphaFactor() const
    {
      return static_cast<EFactor>(afactors >> 4);
    }

    EFactor GetDstAlphaFactor() const
    {
      return static_cast<EFactor>(afactors & 0xf);
    }

    inline uint32 GetValue() const { return mValue; }

private:
    static SRX_INLINE uint8 Combine(EFactor sfactor,
                                    EFactor dfactor) SRX_NOEXCEPT;
    static SRX_INLINE uint8 Combine(EOperation op,
                                    EOperation alphaOp) SRX_NOEXCEPT;

private:
    union
    {
      struct
      {
        uint8 afactors;
        uint8 cfactors;
        uint8 operations;
        uint8 bSeparate;
      };

      uint32 mValue;
    };
  };

  SRX_INLINE uint8 BlendMode::Combine(EFactor sfactor,
                                      EFactor dfactor) SRX_NOEXCEPT
  {
    return ((((uint8)sfactor) << 4) | (((uint8)dfactor) & 0xf));
  }

  SRX_INLINE uint8 BlendMode::Combine(EOperation op,
                                      EOperation alphaOp) SRX_NOEXCEPT
  {
    return ((((uint8)op) << 4) | (((uint8)alphaOp) & 0xf));
  }

  SRX_INLINE bool operator==(const BlendMode& lhs,
                             const BlendMode& rhs) SRX_NOEXCEPT
  {
    return lhs.GetValue() == rhs.GetValue();
  }

  SRX_INLINE bool operator!=(const BlendMode& lhs,
                             const BlendMode& rhs) SRX_NOEXCEPT
  {
    return !(lhs == rhs);
  }
}
