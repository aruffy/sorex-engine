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
#include <Sorex/Utils/SxString.h>

namespace Sorex::Graphics
{
  GLRenderDevice::GLRenderDevice() SRX_NOEXCEPT: mActiveShaderProgram(nullptr)
  {}

  GLRenderDevice::~GLRenderDevice()
  {
    mActiveShaderProgram = nullptr;
    for (auto res : mResources)
    {
      if (res.reference)
        res.reference->MakeExpired();

      DeallocateResource(res);
    }
  }

  Status GLRenderDevice::Initialize()
  {
    // _extensions    = MakeUnique<GLExtensions>();
    mRenderContext = MakeUnique<GLRenderContext>(*this);

#ifdef SOREX_OPENGL_DEBUG_OUTPUT
    // OpenGL::EnableDebugOutput(this);
#endif
    return SRX_OK;
  }

  SRX_TYPENAME GLResourceToken GLRenderDevice::Allocate(GLResourceType type)
    SRX_NOEXCEPT
  {
    SRX_CHECK(Thread::IsMainThread());
    SRX_DEBUG("[GLRenderDevice] Allocate '{}' resource", ToString(type));

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

    mResources.emplace_front();
    GLResource& glResource = mResources.front();
    auto        token      = MakeUnique<GLResourceReference>(this, &glResource);

    glResource.id        = id;
    glResource.type      = type;
    glResource.target    = target;
    glResource.inited    = false;
    glResource.reference = token.get();

    return token;
  }

  void GLRenderDevice::Deallocate(GLResourceReference* glResourceReference)
    SRX_NOEXCEPT
  {
    if (GLResource* resource = GetResource(glResourceReference))
    {
      DeallocateResource(*resource);

      glResourceReference->MakeExpired();
      mResources.remove(*resource);  // @note: resource = nullptr;
    }
  }

  void GLRenderDevice::DeallocateResource(GLResource& resource) SRX_NOEXCEPT
  {
    SRX_CHECK(Thread::IsMainThread());
    SRX_TRACE("[GLRenderDevice] Deallocate '{}' resource",
              ToString(resource.type));

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

      if (mActiveShaderProgram)
      {
        GLResource* program =
          GetResource(mActiveShaderProgram->GetResourceToken());

        if (program && program->id == resource.id)
          mActiveShaderProgram = nullptr;
      }
      break;

    case GLResourceType::Texture2D:
      glDeleteTextures(1, &resource.id);
      break;

    default:
      SRX_NOENTRY("[GLRenderDevice] Deallocate: unknown resource");
      return;
    }

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
              && glResource->GetDeviceResource());

    return bIsValid;
  }

  GLShaderPtr GLRenderDevice::GetOrCreateShader(
    const GLShaderSource& shaderSource) SRX_NOEXCEPT
  {
    const auto key = shaderSource.GetHash();
    if (auto it = mShaders.find(key); it != mShaders.end())
      return it->second;

    if (GLShaderPtr shader = std::make_shared<GLShader>(this, shaderSource))
    {
      mShaders[key] = shader;
      return shader;
    }

    return nullptr;
  }

  Status GLRenderDevice::BuildShaderProgram(
    const GLShaderProgram& shaderProgram,
    TVector<GLUniform>&    uniforms) SRX_NOEXCEPT
  {
    SRX_CHECK(Thread::IsMainThread());

    const GLResourceReference* token   = shaderProgram.GetResourceToken();
    GLResource*                program = GetResource(token);

    if (program == nullptr)
      return SRX_STATUS_MSG(
        EStatusCode::Invalid_Argument,
        "[GLRenderDevice] Shader program invalid or has expired token");

    SRX_DEBUG("[GLRenderDevice] Build {}: {}",
              ToString(program->type),
              program->id);

    if (program->inited)
    {
      SRX_NOENTRY("shader program already inited");
      return SRX_OK;
    }

    Status                   status;
    GLuint                   shaderId;
    TSpan<const GLShaderPtr> shaders = shaderProgram.GetShaders();
    for (const GLShaderPtr& shader : shaders)
    {
      status = CompileShader(shader, shaderId);
      if (!status.Ok())
        return status;

      SRX_OPENGL_CALL(glAttachShader(program->id, shaderId));
    }

    // TODO: From shader_program [attrib, name]
    // TODO: Default Uniform Block
    SRX_OPENGL_CALL(glBindAttribLocation(program->id,
                                         (GLuint)EVertexAttrib::Position,
                                         "a_position"));
    SRX_OPENGL_CALL(glBindAttribLocation(program->id,
                                         (GLuint)EVertexAttrib::Color,
                                         "a_color"));
    SRX_OPENGL_CALL(glBindAttribLocation(program->id,
                                         (GLuint)EVertexAttrib::Color_1,
                                         "a_color_1"));
    SRX_OPENGL_CALL(glBindAttribLocation(program->id,
                                         (GLuint)EVertexAttrib::TexCoord,
                                         "a_texcoord"));
    SRX_OPENGL_CALL(glBindAttribLocation(program->id,
                                         (GLuint)EVertexAttrib::TexCoord_1,
                                         "a_texcoord_1"));

    SRX_OPENGL_CALL(glLinkProgram(program->id));
    SRX_OPENGL_CALL(glValidateProgram(program->id));

    GLint success;
    SRX_OPENGL_CALL(glGetProgramiv(program->id, GL_LINK_STATUS, &success));
    if (success != GL_TRUE)
    {
      GLint length;
      SRX_OPENGL_CALL(glGetProgramiv(program->id, GL_INFO_LOG_LENGTH, &length));

      GLString errmsg(length + 1, GLchar(0));
      SRX_OPENGL_CALL(
        glGetProgramInfoLog(program->id, length, NULL, &errmsg[0]));

      return SRX_STATUS_MSG(EStatusCode::Not_Available,
                            "Shader program linker failed: {}",
                            errmsg);
    }

    status = LoadUniforms(*program, uniforms);
    if (status.Ok())
    {
      program->value  = static_cast<GLuint>(uniforms.size());
      program->inited = true;
    }

    return status;
  }

  Status GLRenderDevice::CompileShader(const GLShaderPtr& shader,
                                       GLuint&            shaderId) SRX_NOEXCEPT
  {
    SRX_CHECK(Thread::IsMainThread());

    GLResourceReference* token = shader ? shader->GetResourceToken() : nullptr;
    GLResource*          resource = GetResource(token);

    if (resource == nullptr)
      return SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                            "[GLRenderDevice] Invalid shader to compile");

    SRX_DEBUG("[GLRenderDevice] Compile {}: {}",
              ToString(resource->type),
              resource->id);

    if (resource->inited)
    {
      shaderId = resource->id;
      return SRX_OK;
    }

    SRX_CHECK(shader->GetShaderSource());
    const GLchar* source = static_cast<const GLchar*>(
      shader->GetShaderSource()->GetScript().c_str());

    SRX_OPENGL_CALL(glShaderSource(resource->id, 1, &source, NULL));
    SRX_OPENGL_CALL(glCompileShader(resource->id));

    Status status;
    GLint  success;
    SRX_OPENGL_CALL(glGetShaderiv(resource->id, GL_COMPILE_STATUS, &success));
    if (success != GL_TRUE)
    {
      GLint length;
      SRX_OPENGL_CALL(glGetShaderiv(resource->id, GL_INFO_LOG_LENGTH, &length));

      GLString errmsg(length, GLchar(0));
      SRX_CHECK(errmsg.length() >= (size_t)length);
      SRX_OPENGL_CALL(
        glGetShaderInfoLog(resource->id, length, NULL, &errmsg[0]));

      // TODO: make error_category OpenGL
      status = SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                              "[GLShader] OpenGL shader compile error: {}",
                              errmsg);
    }
    else
    {
      shaderId         = resource->id;
      resource->inited = true;
    }

    return status;
  }

  Status GLRenderDevice::LoadUniforms(GLResource&         program,
                                      TVector<GLUniform>& uniforms) SRX_NOEXCEPT
  {
    SRX_CHECK(program.type == GLResourceType::ShaderProgram);

    GLint number = 0, maxLength = 0;
    SRX_OPENGL_CALL(glGetProgramiv(program.id, GL_ACTIVE_UNIFORMS, &number));
    SRX_OPENGL_CALL(
      glGetProgramiv(program.id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength));

    uniforms.clear();

    if (number <= 0)
      return SRX_OK;

    SRX_CHECK(maxLength);
    GLString buffer(maxLength + 1, GLchar(0));

    GLchar*       name = &buffer[0];
    GLUniformInfo uniform;
    GLsizei       length;

    uniforms.reserve((size_t)number);

    for (GLuint i = 0; i < static_cast<GLuint>(number); ++i)
    {
      SRX_OPENGL_CALL(glGetActiveUniform(program.id,
                                         i,
                                         maxLength,
                                         &length,
                                         &uniform.count,
                                         &uniform.type,
                                         name));

      // @note: cut the uniform name for arrays
      if (const size_t indx = buffer.find('['); indx != buffer.npos)
        buffer[indx] = GLchar(0);

      uniform.location =
        SRX_OPENGL_CALL(glGetUniformLocation(program.id, name));
      uniform.hash = Sorex::Utils::GetHash(StringView(name));

#ifdef SRX_DEBUG_MEDIUM
      uniform.name.assign(name);
#endif
      uniforms.emplace_back(uniform);
    }

    SRX_DEBUG("[GLRenderDevice] Load program {} uniforms {}",
              program.id,
              uniforms.size());

    return SRX_OK;
  }
}  // namespace
