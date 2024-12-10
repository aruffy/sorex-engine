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

#include <Sorex/CoreMinimal.h>

#include "GraphicsTypes.h"

namespace Sorex::Graphics
{
  class Renderer
  {
    SRX_RTTI_BASE(Graphics::Renderer);

public:
    virtual ~Renderer() = default;

    /**
     * @brief Initialize the renderer.
     *
     * @return status of initialization.
     */
    virtual Status Initialize() SRX_NOEXCEPT = 0;

    /**
     * @brief Prepare the renderer to drawing content.
     *
     * This function must be called before using the shader (see below).
     * The renderer do preparation and can change settings and options of a
     * render device and other `shared` elements. After using other shader need
     * to activate this shader and than use it. It is not nessecerry call this
     * function before every shader draw call.
     *
     * @return status of activation;
     */
    virtual Status Activate() SRX_NOEXCEPT = 0;

    /**
     * @brief Flush all data to a render device.
     *
     * If the renderer do batching the real drawing will be after this call.
     */
    virtual void Flush() SRX_NOEXCEPT = 0;

    /**
     * @brief Reset renderer and delete all data.
     */
    virtual void Reset() SRX_NOEXCEPT = 0;

    /**
     * @brief Check if render has nothing to render.
     *
     * @return True if nothing to render.
     */
    virtual bool IsEmpty() const SRX_NOEXCEPT = 0;
  };

  /**
   * PrimitiveRenderer - Draw primitive shapes.
   */
  class PrimitiveRenderer: public Renderer
  {
    SRX_RTTI(Graphics::PrimitiveRenderer, Graphics::Renderer);

public:
    virtual ~PrimitiveRenderer() override = default;

    /**
     * @brief Draw a line with the colors.
     *
     * If color is NULL or colorNumber is zero must use any color.
     * if colorNumber more that one use two color for the ending of lines;
     *
     * @param begin - first point of the line
     * @param end - second point of the line
     * @param colors - first and second colors for the point respectively
     * @param colorNumber - number of colors.
     */
    virtual void DrawLine(Point        begin,
                          Point        end,
                          const Color* color,
                          size_t       colorNumber) SRX_NOEXCEPT = 0;

    /**
     * @brief Draw rectangle from lines (not filled) with the colors;
     *
     * If color is NULL or colorNumber is zero must use any color.
     * If colorNumber less than number of edges, use last color for the
     * remaining edges;
     *
     * @param rectangle - rectangle borders to draw.
     * @param colors - colors for every corner.
     * @param colorNumber - number of color.
     */
    virtual void DrawBorder(const Rectangle& rectangle,
                            const Color*     color,
                            size_t           colorNumber) SRX_NOEXCEPT = 0;

    /**
     * @brief Draw filled rectangle. Every corner has color from array;
     *
     * If color is NULL or colorNumber is zero must use any color.
     * If colorNumber less than number of edges, use last color for the
     * remaining edges;
     *
     * @param rectangle - borders of the drawing rectangle.
     * @param colors - colors for every corner.
     */
    virtual void DrawRectangle(const Rectangle& rectangle,
                               const Color*     color,
                               size_t           colorNumber) SRX_NOEXCEPT = 0;

    /**
     * @brief Draws a circle given the center, radius and number of segments.
     *
     * @param center The circle center point.
     * @param radius The circle rotate of radius.
     * @param segments The number of segments.
     * @param color Set the circle color.
     */
    virtual void DrawCircle(Point    center,
                            scalar_t radius,
                            int32    segments,
                            Color    color) SRX_NOEXCEPT = 0;
  };

  class Texture2D;
  class TextureRenderer: public Renderer
  {
    SRX_RTTI(Graphics::TextureRenderer, Graphics::Renderer);

public:
    virtual ~TextureRenderer() override = default;

    virtual void DrawTexture(const Texture2D* texture,
                             const Point&     position,
                             Color            color) SRX_NOEXCEPT = 0;

    virtual void DrawTexture(const Texture2D* texture,
                             const Point&     position,
                             const Vec2&      scale,
                             EAnchorPoint     anchor,
                             scalar_t         rotation,
                             Color            color) SRX_NOEXCEPT = 0;

    virtual void DrawTexture(const Texture2D* texture,
                             const Rect&      texRect,
                             const Point&     position,
                             Color            color) SRX_NOEXCEPT = 0;
  };
}  // namespace

using SxRenderer = Sorex::Graphics::Renderer;
