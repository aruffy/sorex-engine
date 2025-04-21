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

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "GLPrimitiveRenderer.h"
#include "GLTexture2D.h"
#include "GLTypes.h"

namespace
{
  using namespace Sorex;
  const TArray<GLenum, 5> kDrawModes = { GL_NONE,
                                         GL_POINTS,
                                         GL_LINES,
                                         GL_TRIANGLES,
                                         GL_TRIANGLE_STRIP };

  inline GLenum ConvertRenderingMode(const Graphics::ERenderingMode mode)
    SRX_NOEXCEPT
  {
    const uint32 index = static_cast<uint32>(mode);
    return (index < kDrawModes.size()) ? kDrawModes[index] : GL_NONE;
  }
}

namespace Sorex::Graphics
{
  GLRenderDevice::GLRenderDevice() SRX_NOEXCEPT: mActiveShaderProgram(nullptr)
  {}

  GLRenderDevice::~GLRenderDevice()
  {
    SRX_CHECK(mResources.empty());
  }

  Status GLRenderDevice::Initialize()
  {
    SRX_CLSFUN_TRACE();
    SRX_DEBUG("[GLRenderDevice] OpenGL version: {}",
              reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    SRX_DEBUG("[GLRenderDevice] OpenGL renderer: {}",
              reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

#ifndef SOREX_DEBUG_NONE
    mExtensions = MakeUnique<GLExtensions>();
#endif


    mRenderContext = MakeUnique<GLRenderContext>(*this);

#ifdef SOREX_OPENGL_DEBUG_OUTPUT
    SRX_VERIFY(EnableDebugOutput(*this));
#endif
    return SRX_OK;
  }

  void GLRenderDevice::Shutdown()
  {
    SRX_CLSFUN_TRACE();

    mActiveShaderProgram = nullptr;
    for (auto res : mResources)
    {
      if (res.reference)
        res.reference->MakeExpired();

      DeallocateResource(res);
    }

    mRenderContext.reset();
    mResources.clear();
    mShaders.clear();
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

    SRX_TRACE("[GLRenderDevice] Allocate '{}' resource {}", ToString(type), id);

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
    SRX_TRACE("[GLRenderDevice] Deallocate '{}' resource {}",
              ToString(resource.type),
              resource.id);

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
        const GLResource* const program =
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

    GLShaderPtr shader = std::make_shared<GLShader>(this, shaderSource);
    mShaders[key]      = shader;
    return shader;
  }

  TUniquePointer<Texture2D> GLRenderDevice::CreateTexture2D(Path path)
  {
    return MakeUnique<GLTexture2D>(std::move(path), *this, false);
  }

  // cppcheck-suppress functionConst
  Status GLRenderDevice::InitializeTexture(const GLTexture2D& texture,
                                           bool               bMinmaps)
  {
    SRX_CLSFUN_TRACE();

    SRX_CHECK(Thread::IsMainThread());
    SRX_CHECK(glGetError() == GL_NO_ERROR);

    if (!texture.IsValid())
      return SRX_STATUS_MSG(EStatusCode::Invalid_Argument, "invalid texture");

    GLResource* tex = GetResource(texture.GetResourceToken());
    if (tex == nullptr || tex->inited)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "GLTexture2D invalid token or state");

    SRX_OPENGL_CALL(glActiveTexture(GL_TEXTURE0));
    SRX_OPENGL_CALL(glBindTexture(tex->target, tex->id));

    SRX_OPENGL_CALL(glTexParameteri(tex->target, GL_TEXTURE_WRAP_S, GL_REPEAT));
    SRX_OPENGL_CALL(glTexParameteri(tex->target, GL_TEXTURE_WRAP_T, GL_REPEAT));

    SRX_OPENGL_CALL(
      glTexParameteri(tex->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    SRX_OPENGL_CALL(
      glTexParameteri(tex->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    auto [data, alignment] = texture.GetTexImageData();
    SRX_CHECK(data && alignment);

    GLint currentUnpackAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &currentUnpackAlignment);
    if (currentUnpackAlignment != alignment)
      SRX_OPENGL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, alignment));

    GLint  internalFormat;
    GLenum format, type;
    texture.GetTexImageFormat(internalFormat, format, type);
    const SizeInt size = texture.GetSize();
    SRX_OPENGL_CALL(glTexImage2D(tex->target,
                                 0,
                                 internalFormat,
                                 size.width,
                                 size.height,
                                 0,
                                 format,
                                 type,
                                 data));

    // @TODO: Generate with level if ES 2.0
    if (bMinmaps)
      SRX_OPENGL_CALL(glGenerateMipmap(tex->target));

    if (GLenum errc = glGetError(); errc != GL_NO_ERROR)
      return SRX_STATUS_MSG(EStatusCode::Not_Available,
                            "OpenGL texture call error: {}",
                            errc);

    tex->inited = true;
    return SRX_OK;
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
                                       GLuint& shaderId) const SRX_NOEXCEPT
  {
    SRX_CHECK(Thread::IsMainThread());

    const GLResourceReference* token =
      shader ? shader->GetResourceToken() : nullptr;
    GLResource* const resource = GetResource(token);

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

    SRX_DEBUG("[GLRenderDevice] {} {} loads {} uniforms",
              ToString(program.type),
              program.id,
              number);

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
      uniform.hash = Sorex::Utils::GetHash(GLStringView(name));

#ifdef SOREX_DEBUG_MEDIUM
      uniform.name.assign(name);
#endif
      uniforms.emplace_back(uniform);
    }

    return SRX_OK;
  }

  Status GLRenderDevice::ApplyRenderTechnique(
    const GLRenderTechnique& technique) SRX_NOEXCEPT
  {
    SRX_CHECK(mRenderContext);

    if (!technique.program)
      return SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                            "invalid technique shader program");

    mActiveShaderProgram = technique.program;
    return SRX_OK;
  }

  Status GLRenderDevice::ActivateShaderProgram(GLenum& mode) SRX_NOEXCEPT
  {
    SRX_CHECK(Thread::IsMainThread());

    GLResource* program = GetResource(mActiveShaderProgram->GetResourceToken());
    if (program == nullptr)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "invalid shader program");

    // @TODO: Set value=1 if activated to skip following steps, check uniform
    // updates to apply uniforms

    mode = ConvertRenderingMode(mActiveShaderProgram->GetRenderingMode());
    if (mode == GL_NONE)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "invalid rendering mode: {}",
                            (int)mActiveShaderProgram->GetRenderingMode());

    if (!program->inited)
    {
      if (auto status = mActiveShaderProgram->Initialize(); !status.Ok())
        return status;
    }

    SRX_OPENGL_CALL(glUseProgram(program->id));

    TSpan<GLUniform> uniforms = mActiveShaderProgram->GetUniforms();
    for (auto& uniform : uniforms)
    {
#ifdef SOREX_DEBUG_HIGH
      const GLint loc = SRX_OPENGL_CALL(
        glGetUniformLocation(program->id, uniform.GetName().c_str()));
      SRX_ASSERT(loc >= 0 && uniform.GetLocation() == loc);
#endif

      // @FIXME: add camera
      if (uniform.GetType() == GL_FLOAT_MAT4)
      {
        // @FIXME: SxCamera
        const glm::mat4 camera = glm::ortho(0.f, 800.f, 600.f, 0.f, -1.f, 1.f);
        SRX_VERIFY(uniform.SetMatrix(glm::value_ptr(camera), 4, 4)
                   == EStatusCode::Ok);
      }

      if (auto rc = uniform.Apply(); rc != EStatusCode::Ok)
        return SRX_STATUS_MSG(rc,
                              "Applying uniform <{}> type={} loc={} failed",
                              uniform.GetHash(),
                              uniform.GetType(),
                              uniform.GetLocation());

      // @FIXME:
      if (uniform.GetType() == GL_SAMPLER_2D)
      {
        // GLenum textureIndex;
        // uniform->GetValue(textureIndex);
        // if (_renderContext->ActivateTexture(textureIndex, error) == false)
        // return false;
      }
    }

    return SRX_OK;
  }

  Status GLRenderDevice::Bind(const OpenGL::Buffer& buffer) const SRX_NOEXCEPT
  {
    GLResource* bo = GetResource(buffer.GetResourceToken());
    if (bo == nullptr)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "{} has expired resource reference",
                            ToString(buffer.GetType()));

    SRX_OPENGL_CALL(glBindBuffer(bo->target, bo->id));
    if (bo->inited)
    {
      UpdateBufferData(*bo, buffer.GetData());
      return SRX_OK;
    }

    return InitializeBuffer(*bo, buffer.GetData());
  }

  Status GLRenderDevice::InitializeBuffer(GLResource&         resource,
                                          const GLBufferData& buffer)
    SRX_NOEXCEPT
  {
    if (!buffer.capacity)
      return SRX_STATUS_MSG(
        EStatusCode::Invalid_State,
        "[GLRenderDevice] Vertex buffer has invalid capacity");

    // @TODO: Type of buffer static/dynamic ...
    SRX_OPENGL_CALL(
      glBufferData(resource.target, buffer.capacity, NULL, GL_DYNAMIC_DRAW));

    resource.value  = static_cast<uint32>(buffer.capacity);
    resource.inited = true;

    return SRX_OK;
  }

  Status GLRenderDevice::UpdateBufferData(const GLResource&   resource,
                                          const GLBufferData& data) SRX_NOEXCEPT
  {
    if (resource.inited == false
        || static_cast<GLsizei>(resource.value) < data.size)
    {
      SRX_NOENTRY("[GLRenderDevice] Invalid buffer state");
      return SRX_STATUS(EStatusCode::Invalid_State);
    }

    SRX_OPENGL_CALL(
      glBufferSubData(resource.target, 0, data.size, data.memptr));
    return SRX_OK;
  }

  static GLenum ConvertVertexAttrib(EVertexAttribType attribType) SRX_NOEXCEPT
  {
    switch (attribType)
    {
    case EVertexAttribType::Float:
      return GL_FLOAT;
    case EVertexAttribType::Integer:
      return GL_INT;

    case EVertexAttribType::UByte:
      return GL_UNSIGNED_BYTE;
    default:
      SRX_NOENTRY("Invalid vertex attribute type");
      return GL_INT;
    }
  }

  void GLRenderDevice::EnableVertexAttributes(const VertexLayout& vtxLayout)
    SRX_NOEXCEPT
  {
    for (const VertexAttribute& attrib : vtxLayout)
    {
      const GLboolean bEnableNormalization =
        attrib.normalization ? GL_TRUE : GL_FALSE;

      // @TODO: Check pointer
      const intptr_t offset = static_cast<intptr_t>(attrib.offset);
      SRX_OPENGL_CALL(
        glEnableVertexAttribArray(static_cast<GLuint>(attrib.index)));
      SRX_OPENGL_CALL(
        glVertexAttribPointer(static_cast<GLuint>(attrib.index),
                              attrib.number,
                              ConvertVertexAttrib(attrib.type),
                              bEnableNormalization,
                              static_cast<GLsizei>(vtxLayout.GetStride()),
                              reinterpret_cast<const GLvoid*>(offset)));
    }
  }

  Renderer* GLRenderDevice::CreateRenderer(const RuntimeClass& cls,
                                           ssize_t capacity) SRX_NOEXCEPT
  {
    if (capacity == SRX_UNKNOWN_SIZE)
      capacity = 2048;  // @TODO: Magic constant

    // @TODO: Factory
    if (cls.IsA(GetRuntimeType<Graphics::PrimitiveRenderer>()))
      return new GLPrimitiveRenderer(this, capacity);

    return nullptr;
  }

  void GLRenderDevice::Cleanup()
  {
    if (mRenderContext)
      mRenderContext->Clear();
  }
}  // namespace
