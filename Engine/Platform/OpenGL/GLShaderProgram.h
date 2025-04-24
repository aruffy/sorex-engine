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

#include <Sorex/Graphics/SxTexture2D.h>

#include "GLResourceToken.h"
#include "GLShader.h"
#include "GLUniform.h"

// #include "GLTexture2D.h"

namespace Sorex::Graphics
{
  // @note: used as array index
  enum class ERenderingMode : uint32
  {
    None = 0u,
    Points,
    Lines,
    Triangles,
    TriangleStrip
  };

  class GLRenderDevice;
  class GLShaderProgram final
  {
public:
    static TUniquePointer<GLShaderProgram> Create(
      GLRenderDevice&       glRenderDevice,
      const GLShaderSource& vert,
      const GLShaderSource& frag,
      Status&               status) SRX_NOEXCEPT;

    GLShaderProgram(GLRenderDevice* glRenderDevice,
                    GLShaderPtr     vert,
                    GLShaderPtr     frag) SRX_NOEXCEPT;

    GLShaderProgram(const GLShaderProgram& other)      = delete;
    GLShaderProgram& operator=(GLShaderProgram& other) = delete;

    Status Initialize() SRX_NOEXCEPT;

    const GLResourceReference* GetResourceToken() const { return mToken.get(); }
    SRX_INLINE GLRenderDevice* GetRenderDevice();

    ERenderingMode GetRenderingMode() const { return mMode; }
    void SetRenderingMode(const ERenderingMode value) { mMode = value; }

    TSpan<const GLShaderPtr> GetShaders() const { return mShaders; }
    GLShaderPtr              GetShader(EShaderType shaderType) SRX_NOEXCEPT;

    TSpan<const GLUniform> GetUniforms() const { return mUniforms; }
    // cppcheck-suppress functionConst
    TSpan<GLUniform> GetUniforms() { return mUniforms; }

    Status SetTexture(uint32 index, const Texture2D& texture);

private:
    GLUniform* FindUniform(const hash_t hash);
    Status     SetTexCoordTransform(uint32 index, const Vector2& transform);

private:
    GLResourceToken mToken;

    ERenderingMode       mMode;
    TVector<GLShaderPtr> mShaders;

    TVector<GLUniform> mUniforms;
  };

  SRX_INLINE GLRenderDevice* GLShaderProgram::GetRenderDevice()
  {
    return mToken ? mToken->GetRenderDevice() : nullptr;
  }
}  // namespace
