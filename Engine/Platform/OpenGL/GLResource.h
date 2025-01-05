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

  class GLRenderDevice;
  class GLResourceReference
  {
public:
    static constexpr size_t kInvalidReferenceId =
      std::numeric_limits<size_t>::max();

public:
    GLResourceReference() = default;
    GLResourceReference(GLRenderDevice* glRenderDevice, size_t rid);

    GLResourceReference(const GLResourceReference& other)            = delete;
    GLResourceReference& operator=(const GLResourceReference& other) = delete;

    ~GLResourceReference();

    SRX_INLINE size_t GetId() const { return _id; }

    SRX_INLINE GLRenderDevice*       GetRenderDevice() { return _glDevice; }
    SRX_INLINE const GLRenderDevice* GetRenderDevice() const
    {
      return _glDevice;
    }

    bool IsValid() const;
    void OnExpired();

private:
    size_t          _id       = kInvalidReferenceId;
    GLRenderDevice* _glDevice = nullptr;
  };

  struct GLResource
  {
    static constexpr unsigned int kInvalidResourceId =
      std::numeric_limits<unsigned int>::max();

    uint32 id = kInvalidResourceId;

    GLResourceReference* token = nullptr;

    uint32         value  = 0;
    unsigned int   target = 0u;
    GLResourceType type   = GLResourceType::Idle;

    bool isInited = false;

    void Reset();
  };

  using GLResourceToken = TUniquePointer<GLResourceReference>;

  /* RFY_NODISCARD GLResourceToken AllocateResource(GLRenderDevice*
     glRenderDevice, GLResourceType  type); */
}
