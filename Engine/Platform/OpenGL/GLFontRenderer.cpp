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

#include "GLFontRenderer.h"
#include "GLRenderDevice.h"

#include <Sorex/Graphics/SxFont.h>

namespace Sorex::Graphics
{
  GLFontRenderer::GLFontRenderer(GLRenderDevice& glRenderDevice,
                                 size_t          maxGlyphNumber)
    : mQuadBatch(glRenderDevice, maxGlyphNumber)
    , mTexture(nullptr)
  {}

  Status GLFontRenderer::Initialize() SRX_NOEXCEPT
  {
    SRX_CLSFUN_TRACE();

    GLRenderDevice* glDevice = mQuadBatch.GetRenderDevice();
    Status          status   = SRX_STATUS(EStatusCode::Invalid_State);
    if (glDevice)
    {
      mBitmapShaderProgram =
        GLShaderProgram::Create(*glDevice,
                                OpenGL::Shader::kTextureVertexShaderSource,
                                OpenGL::Shader::kFontBitmapFragmentShaderSource,
                                status);
    }

    if (!mBitmapShaderProgram)
      return status;

    /*     _sdfShaderProgram = GLShaderProgram::CreateFromSource(
          glDevice,
          GLShaderSource::kTextureVertexShaderSource,
          GLShaderSource::kSignedDistanceFieldFragmentShaderSource,
          error);
     */

    mTechnique.blendMode = BlendMode::Alpha;
    mTechnique.program   = mBitmapShaderProgram.get();

    return SRX_OK;
  }

  Status GLFontRenderer::Activate(const CanvasPencil* pencil) SRX_NOEXCEPT
  {
    Reset();

    mPencil              = pencil;
    mTechnique.blendMode = mPencil ? mPencil->blendMode : BlendMode::None;
    mTechnique.program   = nullptr;

    return SRX_OK;
  }

  void GLFontRenderer::Flush() SRX_NOEXCEPT
  {
    if (auto status = mQuadBatch.Flush(); !status.Ok())
      SRX_WARN("[GLTextureRenderer] Missed draw call: {}", status.ToString());
  }

  void GLFontRenderer::Reset() SRX_NOEXCEPT
  {
    // DisableOutline();

    mTexture = nullptr;
    mQuadBatch.Clear();
  }

  void GLFontRenderer::DrawText(const Font&  font,
                                StringView   text,
                                const Point& pos,
                                float        scale,
                                Color        color)
  {
    if (!ApplyFont(font, scale))
      return;

    // DisableOutline();

    Point position = pos;
    if (auto metrics = font.GetMetrics())
      position.y += metrics->leading;

    for (const char ch : text)
      if (const FontGlyph* glyph = font.GetGlyph(static_cast<glyph_t>(ch)))
        position.x = DrawGlyph(*glyph,
                               position,
                               color,
                               scale);  // cppcheck-suppress unreadVariable
  }

  void GLFontRenderer::DrawText(const Font&  font,
                                WStringView  wtext,
                                const Point& pos,
                                float        scale,
                                Color        color)
  {
    SRX_NOENTRY(
      "[GLFontRenderer] DrawText with WStringView is not implemented yet");
  }

  static inline bool IsEqual(const FontData::SDFMetrics& lhs,
                             const FontData::SDFMetrics& rhs)
  {
    return lhs.onedge == rhs.onedge
           && Math::IsEqual(lhs.pxlDistScale, rhs.pxlDistScale);
  }

  bool GLFontRenderer::ApplyFont(const Font& font,
                                 const float scale) SRX_NOEXCEPT
  {
    const bool bSdfFont = (font.GetType() == EFontType::Signed_Distance_Field);
    GLShaderProgram* shaderProgram =
      bSdfFont ? mSdfShaderProgram.get() : mBitmapShaderProgram.get();

    if (shaderProgram != mTechnique.program)
    {
      Flush();
      mTechnique.program       = shaderProgram;
      GLRenderDevice* glDevice = mQuadBatch.GetRenderDevice();
      if (!glDevice || glDevice->ApplyRenderTechnique(mTechnique) != SRX_OK)
      {
        SRX_NOENTRY("[GLFontRenderer] Failed to apply render technique");
        return false;
      }
    }

    const FontData::Metrics* metrics  = font.GetMetrics();
    constexpr size_t         kByteMax = std::numeric_limits<uint8>::max();

    if (bSdfFont)
    {
      const FontData::SDFMetrics* sdf =
        metrics && metrics->sdf.has_value() ? &metrics->sdf.value() : nullptr;
      SRX_NOENTRY("[GLFontRenderer] SDF font rendering is not implemented yet");
    }

    return SetFontTexture(font.GetTexture());
  }

  bool GLFontRenderer::SetFontTexture(const Texture2D* texture) SRX_NOEXCEPT
  {
    if (texture != mTexture)
    {
      Flush();
      mTexture = texture;

      if (mTexture)
      {
        if (const auto s =
              mTechnique.program->SetTexture(0, *mTexture, nullptr);
            s != SRX_OK)
        {
          SRX_NOEXCEPT(s.ToString());
          return false;
        }
      }
    }

    return mTexture != nullptr;
  }

  scalar_t GLFontRenderer::DrawGlyph(const FontGlyph& glyph,
                                     Point            position,
                                     const Color      color,
                                     const scalar_t   scale)
  {
    const scalar_t baseline = position.y;

    position.x += glyph.bearing.x * scale;
    position.y = baseline - glyph.bearing.y * scale;

    DrawQuad(glyph.rect, position, scale, color);

    return position.x + (glyph.advance * scale);
  }

  void GLFontRenderer::DrawQuad(const RectInt& rect,
                                const Point&   position,
                                const scalar_t scale,
                                Color          color)
  {
    Rect texRect(scalar_t(rect.x),
                 scalar_t(rect.y),
                 scalar_t(rect.width),
                 scalar_t(rect.height));

    SRX_CHECK(mTexture && mTexture->GetContentRect().Contains(texRect));
    SRX_CHECK(scale > scalar_t(0));

    TArray<Point, 4> texPoints;
    texRect.ToArray(texPoints);

    TArray<Point, 4> screenPoints;
    texRect.SetLocation(position);
    texRect.SetSize(texRect.width * scale, texRect.height * scale);
    texRect.ToArray(screenPoints);

    if (mPencil && mPencil->transform.has_value())
    {
      for (Point& sp : screenPoints)  // cppcheck-suppress useStlAlgorithm
        sp = Mat3::Transform(mPencil->transform.value(), sp);
    }

    mQuadBatch.Draw(texPoints, screenPoints, color);
  }

}  // namespace Sorex::Graphics