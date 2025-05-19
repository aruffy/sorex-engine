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

#include <Sorex/Graphics/SxBlendMode.h>

namespace Sorex::Graphics
{
  const BlendMode BlendMode::None;
  const BlendMode BlendMode::Alpha(EFactor::Src_Alpha,
                                   EFactor::One_Minus_Src_Alpha,
                                   EOperation::Add);
  const BlendMode BlendMode::Additive(EFactor::Src_Alpha,
                                      EFactor::One,
                                      EOperation::Add,
                                      EFactor::Zero,
                                      EFactor::One,
                                      EOperation::Add);
  const BlendMode BlendMode::Multiply(EFactor::Dst_Color,
                                      EFactor::Zero,
                                      EOperation::Add,
                                      EFactor::One,
                                      EFactor::One,
                                      EOperation::Max);

  BlendMode::BlendMode()
    : mValue(0u)
  {}

  BlendMode::BlendMode(EFactor sfactor, EFactor dfactor, EOperation op)
    : afactors(Combine(sfactor, dfactor))
    , cfactors(0u)
    , operations(static_cast<uint8>(op))
    , bSeparate(0u)
  {}

  BlendMode::BlendMode(EFactor    srcFactor,
                       EFactor    dstFactor,
                       EOperation op,
                       EFactor    srcAlphaFactor,
                       EFactor    dstAlphaFactor,
                       EOperation opAlpha)
    : afactors(Combine(srcAlphaFactor, dstAlphaFactor))
    , cfactors(Combine(srcFactor, dstFactor))
    , operations(Combine(op, opAlpha))
    , bSeparate(1u)
  {}

  BlendMode::BlendMode(uint32 value)
    : mValue(value)
  {}

  BlendMode::BlendMode(const BlendMode& other) noexcept
    : mValue(other.mValue)
  {}

  BlendMode& BlendMode::operator=(const BlendMode& other) noexcept
  {
    mValue = other.mValue;
    return *this;
  }

}  // namespace
