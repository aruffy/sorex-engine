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

#include "GLRenderDevice.h"

#include <Sorex/SxThread.h>

namespace Sorex::Graphics
{

  Status GLRenderDevice::Initialize()
  {
    // _extensions    = MakeUnique<GLExtensions>();
    // _renderContext = MakeUnique<GLRenderContext>(*this);

#ifdef SOREX_OPENGL_DEBUG_OUTPUT
    OpenGL::EnableDebugOutput(this);
#endif
    return SRX_OK;
  }

  SRX_TYPENAME GLResourceToken GLRenderDevice::Allocate(GLResourceType type)
    SRX_NOEXCEPT
  {
    SRX_CHECK(Thread::IsMainThread());

    GLuint id     = 0;
    GLenum target = 0;
    switch (type)
    {
    case GLResourceType::VertexArray:
      glGenVertexArrays(1, &id);
      target = GL_VERTEX_ARRAY;
      break;

    case GLResourceType::VertexBuffer:
    case GLResourceType::IndexBuffer:
      glGenBuffers(1, &id);
      target = (type == GLResourceType::VertexBuffer ? GL_ARRAY_BUFFER
                                                     : GL_ELEMENT_ARRAY_BUFFER);
      break;

    case GLResourceType::VertexShader:
    case GLResourceType::FragmentShader:
      target = (type == GLResourceType::VertexShader ? GL_VERTEX_SHADER
                                                     : GL_FRAGMENT_SHADER);
      id     = glCreateShader(target);
      break;

    case GLResourceType::ShaderProgram:
      id = glCreateProgram();
      break;

    case GLResourceType::Texture2D:
      glGenTextures(1, &id);
      target = GL_TEXTURE_2D;
      break;

    default:
      SRX_NOENTRY("[GLRenderDevice] Allocation: Invalid type");
      return nullptr;
    }

    GLResource glResource;
    glResource.id     = id;
    glResource.type   = type;
    glResource.target = target;
    auto it = mResources.insert(mResources.cbegin(), std::move(glResource));

    return MakeUnique<GLResourceReference>(this, &(*it));
  }

  void GLRenderDevice::Deallocate(GLResourceReference* glResourceReference)
    SRX_NOEXCEPT
  {
    if (GLResource* resource = GetResource(glResourceReference))
      DeallocateResource(*resource);
  }

  void GLRenderDevice::DeallocateResource(GLResource& resource) SRX_NOEXCEPT
  {
    SRX_CHECK(Thread::IsMainThread());

    if (resource.id == GLResource::kInvalidResourceId)
      return;

    switch (resource.type)
    {
    case GLResourceType::VertexArray:
      glDeleteVertexArrays(1, &resource.id);
      break;

    case GLResourceType::VertexBuffer:
    case GLResourceType::IndexBuffer:
      glDeleteBuffers(1, &resource.id);
      break;

    case GLResourceType::VertexShader:
    case GLResourceType::FragmentShader:
      glDeleteShader(resource.id);
      break;

    case GLResourceType::ShaderProgram:
      glDeleteProgram(resource.id);
      // _uniforms.erase(resource.id);

      /* if (!_isDestroying && _activeShaderProgram)
      {
        GLResource* program =
          GetResource(_activeShaderProgram->GetResourceToken());
        if (program && program->id == resource.id)
          _activeShaderProgram = nullptr;
      } */
      break;

    case GLResourceType::Texture2D:
      glDeleteTextures(1, &resource.id);
      break;

    default:
      SRX_NOENTRY("[GLRenderDevice] Deallocate: unknown resource");
      return;
    }

    // resource.Reset();

#ifdef RUFFY_GAME_DEVELOPMENT
    // _stats.renderDeviceResources.Decrease();
#endif
  }

  GLResource* GLRenderDevice::GetResource(
    const GLResourceReference* glResourceReference) const SRX_NOEXCEPT
  {
    if (!IsValidResourceReference(glResourceReference))
    {
      SRX_NOENTRY("[GLRenderDevice] Invalid graphics resource reference");
      return nullptr;
    }

    return glResourceReference->GetDeviceResource();
  }

  bool GLRenderDevice::IsValidResourceReference(
    const GLResourceReference* glResource) const SRX_NOEXCEPT
  {
    const bool bIsValid = glResource && glResource->IsValid();
    SRX_CHECK(bIsValid && glResource->GetRenderDevice() == this
              && glResource->GetResource());

    return bIsValid;
  }

}  // namespace
