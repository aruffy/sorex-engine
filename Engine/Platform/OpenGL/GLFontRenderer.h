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
#include <Sorex/Graphics/SxFont.h>

#include "GLQuadBatch.h"
#include "GLShaderProgram.h"
#include "GLRenderTechnique.h"

namespace Sorex::Graphics
{
  class GLRenderDevice;
  class GLFontRenderer final: public TextRenderer
  {
    SRX_RTTI(Graphics::GLFontRenderer, Graphics::TextRenderer);

public:
    GLFontRenderer(GLRenderDevice& glRenderDevice, size_t maxGlyphNumber);

    virtual Status Initialize() SRX_NOEXCEPT override;
    virtual Status Activate(const CanvasPencil* pencil) SRX_NOEXCEPT override;
    virtual void   Flush() SRX_NOEXCEPT override;
    virtual void   Reset() SRX_NOEXCEPT override;
    virtual bool   IsEmpty() const SRX_NOEXCEPT override
    {
      return mQuadBatch.IsEmpty();
    }

    /**
     * @copydoc TextRenderer::DrawText
     */
    virtual void DrawText(const Font&  font,
                          StringView   text,
                          const Point& pos,
                          scalar_t     scale = 1.f,
                          Color        color = Color::White) override;

    /**
     * @copydoc TextRenderer::DrawText
     */
    virtual void DrawText(const Font&  font,
                          WStringView  wtext,
                          const Point& pos,
                          scalar_t     scale = 1.f,
                          Color        color = Color::White) override;

private:
    bool ApplyFont(const Font& font, const float scale) SRX_NOEXCEPT;
    bool SetFontTexture(const Texture2D* texture) SRX_NOEXCEPT;

    scalar_t DrawGlyph(const FontGlyph& glyph,
                       Point            position,
                       const Color      color,
                       const scalar_t   scale);
    void     DrawQuad(const RectInt& texRect,
                      const Point&   position,
                      const scalar_t scale,
                      Color          color);

private:
    GLTexQuadBatch mQuadBatch;

    const Texture2D*    mTexture;
    const CanvasPencil* mPencil;

    GLRenderTechnique               mTechnique;
    TUniquePointer<GLShaderProgram> mBitmapShaderProgram;
    TUniquePointer<GLShaderProgram> mSdfShaderProgram;
  };

}  // namespace Sorex::Graphics