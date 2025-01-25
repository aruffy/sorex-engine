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
#include <Sorex/Graphics/SxRenderer.h>
#include <Sorex/Graphics/SxVertex.h>

#include "GLVertexArray.h"
#include "GLShaderProgram.h"
#include "GLRenderTechnique.h"

namespace Sorex::Graphics
{
  class GLRenderDevice;
  class GLPrimitiveRenderer: public PrimitiveRenderer
  {
    using VertexType   = Vertex::V2F_C4B;
    using VertexArray  = GLVertexArray<VertexType>;
    using VertexBuffer = typename VertexArray::VertexBuffer;

public:
    explicit GLPrimitiveRenderer(GLRenderDevice* glRenderDevice,
                                 size_t          vtxCapacity = 1024u);

    // API Render
    virtual Status Initialize() SRX_NOEXCEPT override;
    virtual Status Activate() SRX_NOEXCEPT override;
    virtual void   Flush() SRX_NOEXCEPT override;
    virtual void   Reset() SRX_NOEXCEPT override;
    virtual bool   IsEmpty() const SRX_NOEXCEPT override
    {
      return mVtxArray.IsEmpty();
    }

    // API PrimitiveRenderer
    virtual void DrawLine(Point        begin,
                          Point        end,
                          const Color* color,
                          size_t       colorNumber) SRX_NOEXCEPT override;
    virtual void DrawBorder(const Rectangle& rectangle,
                            const Color*     color,
                            size_t           colorNumber) SRX_NOEXCEPT override;
    virtual void DrawRectangle(const Rectangle& rectangle,
                               const Color*     color,
                               size_t colorNumber) SRX_NOEXCEPT override;
    virtual void DrawCircle(Point center,
                            float radius,
                            int32 segments,
                            Color color) SRX_NOEXCEPT override;

private:
    Vertex::V2F_C4B* AllocateVertices(size_t vtxNumber) SRX_NOEXCEPT;
    void             SwitchRenderingMode(ERenderingMode mode);

    void DrawBorder(const Color* color, size_t colorNumber);
    void DrawRectangle(const Color* color, size_t colorNumber);

private:
    GLRenderDevice* mRenderDevice;
    // const CanvasState* mCanvasState;

    TArray<Point, 4>  mPoints;
    GLRenderTechnique mTechnique;

    VertexArray mVtxArray;
    size_t      mVtxCapacity;

    TUniquePointer<GLShaderProgram> mShaderProgram;
  };
}
