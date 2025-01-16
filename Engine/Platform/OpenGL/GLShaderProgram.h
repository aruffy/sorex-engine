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

#include "GLResourceToken.h"
#include "GLShader.h"
// #include "GLUniform.h"
// #include "GLTexture2D.h"

namespace Ruffy::Graphics
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
  class GLShaderProgram
  {
public:
    static TUniquePointer<GLShaderProgram> CreateFromSource(
      GLRenderDevice*                 glRenderDevice,
      const GLShaderSource::Instance& vert,
      const GLShaderSource::Instance& frag,
      Error*                          error);

    GLShaderProgram(const GLShaderProgram& other)      = delete;
    GLShaderProgram& operator=(GLShaderProgram& other) = delete;

    RFY_NODISCARD bool Initialize(Error* error) RFY_NOEXCEPT;

    inline const GLResourceReference* GetResourceToken() const
    {
      return _glResourceToken.get();
    }
    inline GLRenderDevice* GetRenderDevice();

    inline ERenderingMode GetRenderingMode() const { return _mode; }
    inline void SetRenderingMode(ERenderingMode value) { _mode = value; }

    const TVector<GLShaderPointer>& GetShaders() const { return _shaders; }
    GLShaderPointer                 GetShader(EShaderType shaderType);

    inline const TVector<GLUniform*>& GetUniforms() const { return _uniforms; }
    const GLUniform*                  GetShaderParam(const String& name) const;
    GLUniform*                        GetShaderParam(const String& name);

    bool SetTexture(uint32                index,
                    const Texture2D*      texture,
                    const TextureSampler* sampler,
                    Error*                error);

private:
    GLShaderProgram(GLRenderDevice* glRenderDevice,
                    GLShaderPointer vert,
                    GLShaderPointer frag);

    // cppcheck-suppress functionConst
    bool       SetTexCoordTransform(uint32         index,
                                    const Vector2& transform,
                                    Error*         error);
    GLUniform* FindUniform(const hash_t hash) const;

private:
    GLResourceToken _glResourceToken;

    ERenderingMode           _mode;
    TVector<GLShaderPointer> _shaders;
    TVector<GLUniform*>      _uniforms;
  };

  inline GLRenderDevice* GLShaderProgram::GetRenderDevice()
  {
    return _glResourceToken ? _glResourceToken->GetRenderDevice() : nullptr;
  }
}
