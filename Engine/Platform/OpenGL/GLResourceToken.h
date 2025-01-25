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

#include "GLTypes.h"

namespace Sorex::Graphics
{
  enum class GLResourceType
  {
    Idle,
    Texture2D,
    VertexArray,
    VertexBuffer,
    IndexBuffer,
    ShaderProgram,
    VertexShader,
    FragmentShader
  };

  String ToString(const GLResourceType type) SRX_NOEXCEPT;

  class GLResourceReference;
  struct GLResource
  {
    static constexpr GLuint kInvalidResourceId =
      std::numeric_limits<GLuint>::max();

    GLResourceType type = GLResourceType::Idle;

    GLuint id     = kInvalidResourceId;
    GLenum target = 0;
    GLuint value  = 0;

    bool                 inited = false;
    GLResourceReference* reference;

    SRX_INLINE bool operator==(const GLResource& other) const SRX_NOEXCEPT
    {
      return type == other.type && target == other.target && id == other.id;
    }
    SRX_INLINE bool operator!=(const GLResource& other) const SRX_NOEXCEPT
    {
      return !(*this == other);
    }
  };

  class GLRenderDevice;
  class GLResourceReference final
  {
    friend class GLRenderDevice;

public:
    GLResourceReference(GLRenderDevice* glDevice,
                        GLResource*     glResource) SRX_NOEXCEPT;

    GLResourceReference(const GLResourceReference& other)            = delete;
    GLResourceReference& operator=(const GLResourceReference& other) = delete;

    ~GLResourceReference();

    SRX_INLINE GLRenderDevice*       GetRenderDevice() { return mRenderDevice; }
    SRX_INLINE const GLRenderDevice* GetRenderDevice() const
    {
      return mRenderDevice;
    }

    SRX_INLINE bool IsValid() const { return mResource && mRenderDevice; }

private:
    void        MakeExpired() SRX_NOEXCEPT;
    GLResource* GetDeviceResource() const SRX_NOEXCEPT { return mResource; }

private:
    GLRenderDevice* mRenderDevice;
    GLResource*     mResource;
  };

  using GLResourceToken = TUniquePointer<GLResourceReference>;

  SRX_NODISCARD GLResourceToken AllocateResource(GLRenderDevice* glRenderDevice,
                                                 GLResourceType  type)
    SRX_NOEXCEPT;
}
