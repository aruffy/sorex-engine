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

#include <Sorex/Graphics/SxCanvas.h>

namespace Sorex
{
  Canvas::Canvas(Graphics::RenderDevice& device) SRX_NOEXCEPT
    : mRenderDevice(device)
  {}

  Status Canvas::Initialize() SRX_NOEXCEPT
  {
    mPrimitiveRenderer =
      mRenderDevice.CreateRenderer<Graphics::PrimitiveRenderer>();
    mTextureRenderer =
      mRenderDevice.CreateRenderer<Graphics::TextureRenderer>();

    if (!mPrimitiveRenderer || !mTextureRenderer)
      return SRX_STATUS_MSG(EStatusCode::Not_Supported,
                            "renderer creation failed");

    // FIXME:
    SRX_VERIFY(mPrimitiveRenderer->Initialize().Ok());
    SRX_VERIFY(mTextureRenderer->Initialize().Ok());

    return SRX_OK;
  }

  void Canvas::PopPencil()
  {
    if (mPencilStack.empty())
    {
      SRX_NOENTRY("canvas pencil stack empty");
      return;
    }

    Flush();

    mPencil = mPencilStack.top();
    mPencilStack.pop();
  }

  void Canvas::Rotate(scalar_t rotation)
  {
    if (rotation)
      mPencil.transform = Mat3::Rotation(Math::Radians(rotation))
                          * mPencil.transform.value_or(Mat3::Identity());
  }

  void Canvas::Rotate(scalar_t                     rotation,
                      const Graphics::EAnchorPoint anchorPoint)
  {
    if (rotation == 0.f)
      return;

    Mat3       t      = mPencil.transform.value_or(Mat3::Identity());
    const Vec2 anchor = Graphics::Utils::ToVec2(anchorPoint);

    t.Translate(anchor.x, anchor.y);
    t.Rotate(Math::Radians(rotation));
    t.Translate(-anchor.x, -anchor.y);

    mPencil.transform = t;
  }

  void Canvas::Translate(scalar_t x, scalar_t y)
  {
    if (mPencil.transform.has_value() == false)
      mPencil.transform = Mat3::Identity();

    mPencil.transform->Translate(x, y);
  }

  void Canvas::Scale(scalar_t sx, scalar_t sy)
  {
    if (mPencil.transform.has_value() == false)
      mPencil.transform = Mat3::Identity();

    mPencil.transform->Scale(sx, sy);
  }

  void Canvas::SetBlendMode(const Graphics::BlendMode mode) SRX_NOEXCEPT
  {
    if (mPencil.blendMode == mode)
      return;

    Flush();
    mPencil.blendMode = mode;
  }

  void Canvas::DrawLine(const Point& begin,
                        const Point& end,
                        const Color* color,
                        size_t       colorNumber) SRX_NOEXCEPT
  {
    if (ActivateRenderer(mPrimitiveRenderer.get()))
      mPrimitiveRenderer->DrawLine(begin, end, color, colorNumber);
  }

  void Canvas::DrawRectangle(const Rect&  rect,
                             const Color* colors,
                             size_t       colorNumber,
                             bool         bFilled) SRX_NOEXCEPT
  {
    if (!ActivateRenderer(mPrimitiveRenderer.get()))
      return;

    if (bFilled)
      mPrimitiveRenderer->DrawRectangle(rect, colors, colorNumber);
    else
      mPrimitiveRenderer->DrawBorder(rect, colors, colorNumber);
  }

  bool Canvas::ActivateRenderer(Graphics::Renderer* renderer) SRX_NOEXCEPT
  {
    if (mRenderer == renderer)
      return renderer != nullptr;

    if (mRenderer)
      mRenderer->Flush();

    mRenderer = renderer;
    if (!mRenderer)
      return false;

    auto status = mRenderer->Activate(&mPencil);
    if (!status.Ok())
    {
      SRX_WARN("[Canvas] Renderer activation failed: {}", status.ToString());
      return false;
    }

    return true;
  }

  void Canvas::DrawCircle(const Point& center,
                          float        radius,
                          int32        segments,
                          Color        color) SRX_NOEXCEPT
  {
    if (ActivateRenderer(mPrimitiveRenderer.get()))
      mPrimitiveRenderer->DrawCircle(center, radius, segments, color);
  }


  void Canvas::DrawTexture(const Graphics::Texture2D* texture,
                           const Point&               location,
                           Color                      color)
  {
    if (ActivateRenderer(mTextureRenderer.get()))
      mTextureRenderer->DrawTexture(texture, location, color);
  }

  void Canvas::DrawTexture(const Graphics::Texture2D* texture,
                           const Point&               location,
                           scalar_t                   rotation,
                           Vec2                       scale /* = Vec2::One() */,
                           Color color /* = Color::White */)
  {
    if (ActivateRenderer(mTextureRenderer.get()))
      mTextureRenderer->DrawTexture(texture,
                                    location,
                                    scale,
                                    Graphics::EAnchorPoint::Middle,
                                    rotation,
                                    color);
  }

  void Canvas::Clear() SRX_NOEXCEPT
  {
    if (mRenderer)
      mRenderer->Reset();

    mRenderDevice.Cleanup();
  }
}  // namespace
