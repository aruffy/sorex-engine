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

#include "GLResourceToken.h"

namespace Sorex::Graphics
{
  enum class EShaderType
  {
    Vertex,
    Fragment
  };

  struct GLShaderSource
  {
    EShaderType type;
    StringView  text;
  };

  class GLRenderDevice;
  class GLShader
  {
public:
    SRX_INLINE GLShader(GLRenderDevice*       glRenderDevice,
                        const GLShaderSource& source) SRX_NOEXCEPT
      : GLShader(glRenderDevice, source.type, source.text)
    {}
    SRX_INLINE GLShader(GLRenderDevice* glRenderDevice,
                        EShaderType     type,
                        StringView      source) SRX_NOEXCEPT;

    const String& GetShaderSource() const { return mSource; }
    EShaderType   GetShaderType() const { return mType; }

    SRX_INLINE GLRenderDevice* GetRenderDevice();
    GLResourceReference* GetResourceToken() { return mGlResourceToken.get(); }

private:
    GLResourceToken mGlResourceToken;

    EShaderType mType;
    String      mSource;
  };

  SRX_INLINE GLRenderDevice* GLShader::GetRenderDevice()
  {
    return mGlResourceToken ? mGlResourceToken->GetRenderDevice() : nullptr;
  }

  SRX_INLINE GLShader::GLShader(GLRenderDevice* glRenderDevice,
                                EShaderType     type,
                                StringView      source) SRX_NOEXCEPT
    : mGlResourceToken(AllocateResource(glRenderDevice,
                                        type == EShaderType::Vertex
                                          ? GLResourceType::VertexShader
                                          : GLResourceType::FragmentShader))
    , mType(type)
    , mSource(source)
  {}

  using GLShaderPtr = TSharedPointer<GLShader>;
}
