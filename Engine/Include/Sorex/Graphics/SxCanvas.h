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

#include "SxRenderDevice.h"
#include "SxRenderer.h"

namespace Sorex
{
  class Canvas
  {
public:
    explicit Canvas(Graphics::RenderDevice& device);

    Status Initialize() SRX_NOEXCEPT { return SRX_OK; }
    /*
        inline void DrawLine(const Point& begin,
                             const Point& end,
                             const Color& color = Color::White);
        inline void DrawLine(const Point&            begin,
                             const Point&            end,
                             const TArray<Color, 2>& colors);
        void        DrawCircle(const Point& center,
                               float        radius,
                               int32        segments,
                               Color        color);

        inline void DrawRectangle(const Rectangle&        rectangle,
                                  const TArray<Color, 4>& colors,
                                  bool                    bFilled = false);
        inline void DrawRectangle(const Rectangle& rectangle,
                                  const Color&     color   = Color::White,
                                  bool             bFilled = false);

        void DrawSprite(const Graphics::Sprite& sprite);
        void DrawTexture(const Graphics::Texture2D* texture,
                         const Point&               location,
                         Color                      color);
        void DrawTexture(const Graphics::Texture2D* texture,
                         const Point&               location,
                         float                      rotation = 0.f,
                         const Vector2              scale    = Vec2::One(),
                         const Color&               color    = Color::White);

        void DrawText(const Graphics::Font& font,
                      StringView            text,
                      const Point&          pos,
                      Color                 color = Color::White,
                      float                 scale = 1.f);
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
        inline void Flush() { ActivateRenderer(nullptr); }

        // State Contol
        inline void PushState() { _stateStack.push(_state); }
        void        PopState();

        void Rotate(float rotation);
        void Rotate(float rotation, const Point& anchor);  // @todo:
       EAnchorPoint ?

        void        Translate(float x, float y);
        inline void Translate(const Vector2& v) { Translate(v.x, v.y); }

        void        Scale(float sx, float sy);
        inline void Scale(const Vector2& v) { Scale(v.x, v.y); }

        void SetBlendMode(Graphics::BlendMode mode);
     */
private:
    bool ActivateRenderer(Graphics::Renderer* renderer);

    /*
      void DrawLine(const Point& begin,
                      const Point& end,
                      const Color* color,
                      size_t       colorNumber);
        void DrawRectangle(const Rect&  rect,
                           const Color* color,
                           size_t       colorNumber,
                           bool         bFilled = false);
     */

private:
    Graphics::RenderDevice& mRenderDevice;
    Graphics::Renderer*     mRenderer = nullptr;

    // TUniquePointer<Graphics::PrimitiveRenderer> _primitiveRenderer;
    // TUniquePointer<Graphics::TextureRenderer>   _textureRenderer;
    // TUniquePointer<Graphics::TextRenderer>      _textRenderer;

    // Graphics::CanvasState         _state;
    // TStack<Graphics::CanvasState> _stateStack;
  };
}  // namespace
