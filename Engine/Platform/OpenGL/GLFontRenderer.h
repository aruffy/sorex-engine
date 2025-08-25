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

#include <cwctype>

#include <Sorex/SxCoreMinimal.h>
#include <Sorex/Graphics/SxRenderer.h>
#include <Sorex/Graphics/SxFont.h>
#include <Sorex/Graphics/SxFontDecorator.h>

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
                          Color        color = Color::White) override
    {
      DrawTextInternal(font, text, pos, scale, color);
    }

    /**
     * @copydoc TextRenderer::DrawText
     */
    virtual void DrawText(const Font&  font,
                          WStringView  wtext,
                          const Point& pos,
                          scalar_t     scale = 1.f,
                          Color        color = Color::White) override
    {
      DrawTextInternal(font, wtext, pos, scale, color);
    }

    virtual void DrawText(const FontDecorator& decorator,
                          StringView           text,
                          const Point&         pos) override

    {
      DrawTextInternal(decorator, text, pos);
    }
    virtual void DrawText(const FontDecorator& decorator,
                          WStringView          wtext,
                          const Point&         pos) override
    {
      DrawTextInternal(decorator, wtext, pos);
    }


private:
    bool ApplyFont(const Font& font, const float scale = 1.f) SRX_NOEXCEPT;
    bool ApplyOutline(const FontOutline& outline,
                      const float        scale = 1.f) SRX_NOEXCEPT;
    void DisableOutline() SRX_NOEXCEPT;

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
    template<typename Char>
    void DrawTextInternal(const Font&           font,
                          BasicStringView<Char> text,
                          const Point&          pos,
                          scalar_t              scale,
                          Color                 color);

    template<typename Char>
    void DrawTextInternal(const FontDecorator&  decorator,
                          BasicStringView<Char> text,
                          const Point&          pos);

    template<typename Char>
      requires std::is_same_v<Char, char> || std::is_same_v<Char, wchar_t>
    static Char Transform(const Char ch, const EFontTransform transform)
    {
      if (transform == EFontTransform::None)
        return ch;

      if constexpr (std::is_same_v<Char, char>)
        return transform == EFontTransform::Upppercase ? std::toupper(ch)
                                                       : std::tolower(ch);
      else
        return transform == EFontTransform::Upppercase ? std::towupper(ch)
                                                       : std::towlower(ch);
    }

private:
    GLTexQuadBatch mQuadBatch;

    const Texture2D*    mTexture;
    const CanvasPencil* mPencil;

    GLRenderTechnique               mTechnique;
    TUniquePointer<GLShaderProgram> mBitmapShaderProgram;
    TUniquePointer<GLShaderProgram> mSdfShaderProgram;

    FontData::SDFMetrics mSdfMetrics;
    bool                 mIsOutline;
  };

  template<typename Char>
  void GLFontRenderer::DrawTextInternal(const Font&           font,
                                        BasicStringView<Char> text,
                                        const Point&          pos,
                                        scalar_t              scale,
                                        Color                 color)
  {
    static_assert(sizeof(Char) <= sizeof(glyph_t),
                  "[GLFontRenderer::DrawTextInternal] Invalid character type");

    if (!ApplyFont(font, scale))
      return;

    DisableOutline();

    Point position = pos;
    for (const Char ch : text)
      if (const FontGlyph* glyph = font.GetGlyph(static_cast<glyph_t>(ch)))
        position.x = DrawGlyph(*glyph,
                               position,
                               color,
                               scale);  // cppcheck-suppress unreadVariable
  }

  template<typename Char>
  void GLFontRenderer::DrawTextInternal(const FontDecorator&  decorator,
                                        BasicStringView<Char> text,
                                        const Point&          pos)
  {
    const Font* font  = decorator.GetFont();
    const float scale = decorator.GetScale();
    if (!font || !ApplyFont(*font, scale))
      return;

    ApplyOutline(decorator.GetOutline(), scale);

    Point position = pos;
    if (const auto fontMetrics = font->GetMetrics())
      position.y += fontMetrics->leading * scale;

    const Color          color     = decorator.GetColor();
    const int32          spacing   = decorator.GetLetterSpacing();
    const EFontTransform transform = decorator.GetTextTransform();
    for (const Char ch : text)
    {
      if (const FontGlyph* glyph =
            decorator.GetGlyph(static_cast<glyph_t>(Transform(ch, transform))))
        // cppcheck-suppress unreadVariable
        position.x = DrawGlyph(*glyph, position, color, scale) + spacing;
    }
  }
}  // namespace Sorex::Graphics
