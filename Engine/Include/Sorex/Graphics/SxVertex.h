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
  enum class EVertexAttrib
  {
    Position   = 0,
    Color      = 1,
    Color_0    = Color,
    Color_1    = 2,
    TexCoord   = 3,
    TexCoord_0 = TexCoord,
    TexCoord_1 = 4,
  };

  enum class EVertexAttribType
  {
    Float,
    Integer,
    UByte
  };

  struct VertexAttribute
  {
    EVertexAttrib     index;
    EVertexAttribType type;
    int32             number;
    int32             offset;
    bool              normalization;
  };

  class VertexLayout
  {
public:
    using Iterator = TVector<VertexAttribute>::const_iterator;
    using Pointer  = TVector<VertexAttribute>::const_pointer;

public:
    VertexLayout(TVector<VertexAttribute>&& vtxAttributes,
                 uint32                     layoutSize) SRX_NOEXCEPT;

    SRX_INLINE Iterator begin() const { return mAttribs.cbegin(); }
    SRX_INLINE Iterator end() const { return mAttribs.cend(); }

    SRX_INLINE Pointer GetAttribute(const size_t index) const;

    SRX_INLINE uint32 GetStride() const { return mStride; }
    SRX_INLINE bool   IsValid() const { return (mStride && !mAttribs.empty()); }

private:
    TVector<VertexAttribute> mAttribs;
    uint32                   mStride = 0;
  };

  namespace Vertex
  {
    struct V2F_C4B
    {
      float  position[2];
      uint32 color;

      static const VertexLayout& GetLayout();
    };

    struct V2F_C4B_TC2F
    {
      float  position[2];
      uint32 color;
      float  texCoord[2];

      static const VertexLayout& GetLayout();
    };
  }  // namespace

  SRX_INLINE VertexLayout::Pointer VertexLayout::GetAttribute(
    const size_t index) const
  {
    return (index < mAttribs.size())
             ? std::addressof<const VertexAttribute>(mAttribs[index])
             : nullptr;
  }
}  // namespace
