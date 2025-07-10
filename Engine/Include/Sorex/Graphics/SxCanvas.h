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
#include <Sorex/Math/SxMatrix3x3.h>
#include <Sorex/Graphics/SxGraphicsTypes.h>

#include "SxRenderDevice.h"
#include "SxRenderer.h"
#include "SxCanvasPencil.h"

namespace Sorex
{
  class Canvas
  {
public:
    explicit Canvas(Graphics::RenderDevice& device) SRX_NOEXCEPT;

    Status          Initialize() SRX_NOEXCEPT;
    SRX_INLINE void DrawLine(const Point& begin,
                             const Point& end,
                             const Color& color = Color::White) SRX_NOEXCEPT;
    SRX_INLINE void DrawLine(const Point&            begin,
                             const Point&            end,
                             const TArray<Color, 2>& colors) SRX_NOEXCEPT;
    SRX_INLINE void DrawRect(const Rectangle&        rectangle,
                             const TArray<Color, 4>& colors,
                             bool bFilled = false) SRX_NOEXCEPT;
    SRX_INLINE void DrawRect(const Rectangle& rectangle,
                             const Color&     color   = Color::White,
                             bool             bFilled = false) SRX_NOEXCEPT;

    void DrawCircle(const Point& center,
                    float        radius,
                    int32        segments,
                    Color        color) SRX_NOEXCEPT;

    void DrawTexture(const Graphics::Texture2D* texture,
                     const Point&               location,
                     Color                      color = Color::White);

    void DrawTexture(const Graphics::Texture2D* texture,
                     const Point&               location,
                     scalar_t                   rotation,
                     Vec2                       scale = Vec2::One(),
                     Color                      color = Color::White);

    void DrawText(const Graphics::Font& font,
                  StringView            text,
                  const Point&          pos,
                  scalar_t              scale = 1.f,
                  Color                 color = Color::White);

    /*
            void DrawText(const Graphics::Font& font,
                          WStringView           text,
                          const Point&          pos,
                          Color                 color = Color::White,
            float                 scale = 1.f);

            void DrawText(const Graphics::FontDecorator& decorator,
                          StringView                     text,
                          const Point&                   pos);
            void DrawText(const Graphics::FontDecorator& decorator,
                          WStringView                    wtext,
                          const Point&                   pos);

            void        Clear();





         */

    void            Translate(scalar_t x, scalar_t y);
    SRX_INLINE void Translate(const Vec2& v) { Translate(v.x, v.y); }

    void            Scale(scalar_t sx, scalar_t sy);
    SRX_INLINE void Scale(const Vec2& v) { Scale(v.x, v.y); }

    void Rotate(scalar_t rotation);
    void Rotate(scalar_t rotation, const Graphics::EAnchorPoint anchor);

    void SetBlendMode(const Graphics::BlendMode mode) SRX_NOEXCEPT;
    void SetTextureSampler(const Graphics::TextureSampler& sampler)
      SRX_NOEXCEPT;

    SRX_INLINE void SetPencil(const SxPencil& pencil) { mPencil = pencil; }
    SRX_INLINE void PushPencil() { mPencilStack.push(mPencil); }
    void            PopPencil();

    void            Clear() SRX_NOEXCEPT;
    SRX_INLINE void Flush() { ActivateRenderer(nullptr); }

private:
    bool ActivateRenderer(Graphics::Renderer* renderer) SRX_NOEXCEPT;

    void DrawLine(const Point& begin,
                  const Point& end,
                  const Color* color,
                  size_t       colorNumber) SRX_NOEXCEPT;
    void DrawRectangle(const Rect&  rect,
                       const Color* color,
                       size_t       colorNumber,
                       bool         bFilled = false) SRX_NOEXCEPT;

private:
    Graphics::RenderDevice& mRenderDevice;
    Graphics::Renderer*     mRenderer = nullptr;

    TUniquePointer<Graphics::PrimitiveRenderer> mPrimitiveRenderer;
    TUniquePointer<Graphics::TextureRenderer>   mTextureRenderer;
    TUniquePointer<Graphics::TextRenderer>      mTextRenderer;

    Graphics::CanvasPencil         mPencil;
    TStack<Graphics::CanvasPencil> mPencilStack;
  };

  SRX_INLINE void Canvas::DrawLine(const Point& begin,
                                   const Point& end,
                                   const Color& color /* = Color::White */)
    SRX_NOEXCEPT
  {
    DrawLine(begin, end, &color, 1u);
  }

  SRX_INLINE void Canvas::DrawLine(const Point&            begin,
                                   const Point&            end,
                                   const TArray<Color, 2>& colors) SRX_NOEXCEPT
  {
    DrawLine(begin, end, colors.data(), colors.size());
  }

  SRX_INLINE void Canvas::DrawRect(const Rect&             rect,
                                   const TArray<Color, 4>& colors,
                                   bool bFilled /* = false */) SRX_NOEXCEPT
  {
    DrawRectangle(rect, colors.data(), colors.size(), bFilled);
  }

  SRX_INLINE void Canvas::DrawRect(const Rect&  rect,
                                   const Color& color /* = Color::White */,
                                   bool bFilled /* = false */) SRX_NOEXCEPT
  {
    DrawRectangle(rect, &color, 1, bFilled);
  }

}  // namespace
