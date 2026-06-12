
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

#include <Sorex/Graphics/SxRenderer.h>

#include "GLQuadBatch.h"
#include "GLShaderProgram.h"
#include "GLRenderTechnique.h"

namespace Sorex::Graphics
{
  class GLRenderDevice;
  class GLTextureRenderer final: public TextureRenderer
  {
    SRX_RTTI(Graphics::GLTextureRenderer, Graphics::TextureRenderer);

public:
    GLTextureRenderer(GLRenderDevice& glRenderDevice, size_t maxQuadNumber);

    virtual Status Initialize() SRX_NOEXCEPT override;
    virtual Status Activate(const CanvasPencil* pencil) SRX_NOEXCEPT override;

    virtual void Flush() SRX_NOEXCEPT override;
    virtual void Reset() SRX_NOEXCEPT override;

    virtual bool IsEmpty() const SRX_NOEXCEPT override
    {
      return mQuadBatch.IsEmpty();
    }

    SRX_INLINE GLRenderDevice* GetRenderDevice()
    {
      return mQuadBatch.GetRenderDevice();
    }

    // API TextureRenderer
    virtual void DrawTexture(const Texture2D* texture,
                             const Point&     position,
                             Color            color) SRX_NOEXCEPT override;

    virtual void DrawTexture(const Texture2D* texture,
                             const Rect&      texRect,
                             const Point&     position,
                             Color            color) SRX_NOEXCEPT override;

    virtual void DrawTexture(const Texture2D* texture,
                             const Point&     position,
                             const Vector2&   scale,
                             EAnchorPoint     anchor,
                             scalar_t         rotation,
                             Color            color) SRX_NOEXCEPT override;

private:
    void DrawQuad(const Texture2D* texture, Color color);

private:
    GLTexQuadBatch                  mQuadBatch;
    TUniquePointer<GLShaderProgram> mShaderProgram;
    GLRenderTechnique               mRenderTechnique;

    const Texture2D*    mActiveTexture;
    const CanvasPencil* mPencil;

    TArray<Point, 4> mScreenPoints;
    TArray<Point, 4> mTexPoints;
  };
}
