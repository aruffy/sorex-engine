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

#include "GLTextureRenderer.h"
#include "GLRenderDevice.h"

namespace Sorex::Graphics
{
  GLTextureRenderer::GLTextureRenderer(GLRenderDevice& glRenderDevice,
                                       size_t          maxQuadNumber)
    : mQuadBatch(glRenderDevice, maxQuadNumber)
  {}

  Status GLTextureRenderer::Initialize() SRX_NOEXCEPT
  {
    /* _shaderProgram = GLShaderProgram::CreateFromSource(
      GetRenderDevice(),
      GLShaderSource::kTextureVertexShaderSource,
      GLShaderSource::kTextureFragmentShaderSource,
      error); */

    // _technique.blend   = BlendMode::Alpha;
    // _technique.program = _shaderProgram.get();

    // return (_shaderProgram != nullptr);
    return SRX_STATUS(EStatusCode::Not_Implemented);
  }

  Status GLTextureRenderer::Activate() SRX_NOEXCEPT
  {
    /* GLRenderDevice* glDevice = GetRenderDevice();
    if (!glDevice || !_shaderProgram)
    {
      RFY_MAKE_ERR(error, Error::Invalid_State, "invalid gl resource");
      return false;
    }

    Reset();

    _canvasState     = &state;
    _technique.blend = state.blendMode;

    // _glDevice->SetShaderParam(kUniformCamera, mat4x4, ) @todo

    return glDevice->ApplyRenderTechnique(_technique, error); */

    return SRX_STATUS(EStatusCode::Not_Implemented);
  }

  void GLTextureRenderer::Flush() SRX_NOEXCEPT
  {
    /* if (IsEmpty())
      return;

    if (!_quadBatch.Flush(&_error))
      RFY_WARN("[GLTextureRenderer] Missed draw call: {}",
               _error.GetMessage().c_str()); */
  }

  void GLTextureRenderer::Reset() SRX_NOEXCEPT
  {
    /* _texture = nullptr;
    _quadBatch.Clear(); */
  }

  void GLTextureRenderer::DrawTexture(const Texture2D* texture,
                                      const Point&     position,
                                      Color            color) SRX_NOEXCEPT
  {
    /* if (texture == nullptr)
      return;

    Rect rect = texture->GetContentRect();
    rect.ToArray(_texPoints);

    rect.SetLocation(position);
    rect.ToArray(_screenPoints);

    DrawQuad(texture, color); */
  }

  void GLTextureRenderer::DrawTexture(const Texture2D* texture,
                                      const Point&     position,
                                      const Vec2&      scale,
                                      EAnchorPoint     anchor,
                                      scalar_t         rotation,
                                      Color            color) SRX_NOEXCEPT
  {
    /* Rect rect = texture->GetContentRect();
    rect.ToArray(_texPoints);

    rect.SetLocation(0.f, 0.f);
    rect.ToArray(_screenPoints);

    const Point shift(anchor.x * rect.width, anchor.y * rect.height);
    const Mat3  m = Mat3::Create(Vec2(position.x + shift.x * scale.x,
                                     position.y + shift.y * scale.y),
                                Math::Radians(rotation),
                                scale)
                   * Mat3::Translation(-shift.x, -shift.y);

    for (Point& sp : _screenPoints)
      sp = Mat3::Transform(m, sp);

    DrawQuad(texture, color); */
  }

  void GLTextureRenderer::DrawTexture(const Texture2D* texture,
                                      const Rect&      texRect,
                                      const Point&     position,
                                      Color            color) SRX_NOEXCEPT
  {
    /* if (!texture || !texture->GetContentRect().Contains(texRect))
    {
      RFY_NOENTRY("invalid texture region");
      return;
    }

    texRect.ToArray(_texPoints);
    Rect(position, texRect.GetSize()).ToArray(_screenPoints);

    DrawQuad(texture, color); */
  }

  /*void GLTextureRenderer::DrawSprite(const Sprite* sprite)
  {
     const Graphics::Texture2D* texture =
      sprite ? sprite->GetTexture() : nullptr;
    if (texture == nullptr)
      return;

    const Point pos     = sprite->GetPosition();
    const Vec2& scale   = sprite->GetScale();
    const Rect& texRect = sprite->GetContentRect();
    texRect.ToArray(_texPoints);
    Rect(Point(0.f, 0.f), texRect.GetSize()).ToArray(_screenPoints);

    RFY_CHECK(texture->GetContentRect() == texRect
              || texture->GetContentRect().Contains(texRect));
    RFY_CHECK(scale != Vec2::Zero());

    Mat3 transform;
    if (const float rotation = sprite->GetRotation())
    {
      const Vec2 anchor = Ruffy::Utils::ToVector(sprite->GetAnchorPoint());
      const Vec2 offset(anchor.x * texRect.width, anchor.y * texRect.height);
      transform = Mat3::Create(Vec2(pos.x + offset.x * scale.x,
                                    pos.y + offset.y * scale.y),
                               Math::Radians(rotation),
                               scale)
                  * Mat3::Translation(-offset.x, -offset.y);
    }
    else
    {
      transform = Mat3::Identity();
      transform.Translate(pos.x, pos.y);
      transform.Scale(scale);
    }

    for (Point& sp : _screenPoints)
      sp = Mat3::Transform(transform, sp);

    DrawQuad(texture, sprite->GetColor());
  }  */

  void GLTextureRenderer::DrawQuad(const Texture2D* texture, Color color)
  {
    /*     if (_texture != texture)
        {
          Flush();
          _texture = texture;
          RFY_VERIFY_MSG(_shaderProgram->SetTexture(0, texture, nullptr,
       &_error), _error.GetMessage().c_str());
        }

        if (_canvasState && _canvasState->bUseTransform)
        {
          for (Point& sp : _screenPoints)  // cppcheck-suppress useStlAlgorithm
            sp = Matrix3x3::Transform(_canvasState->transform, sp);
        }

        _quadBatch.Draw(_texPoints, _screenPoints, color); */
  }
}
