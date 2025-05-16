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

#include "GLShaderProgram.h"

#include <Sorex/Utils/SxString.h>

#include "GLRenderDevice.h"

namespace
{
  using namespace Sorex;
  using UniformId = TPair<Graphics::GLString, hash_t>;

  inline UniformId MakeUniformId(Graphics::GLStringView str)
  {
    return std::make_pair(Graphics::GLString(str), Sorex::Utils::GetHash(str));
  }

  // FIXME: @don't keep names for Release
  // FIXME: Take the number of texture from OpenGL Implementation
  const TArray<UniformId, 8> kSamplers = {
    MakeUniformId("u_sampler_0"), MakeUniformId("u_sampler_1"),
    MakeUniformId("u_sampler_2"), MakeUniformId("u_sampler_3"),
    MakeUniformId("u_sampler_4"), MakeUniformId("u_sampler_5"),
    MakeUniformId("u_sampler_6"), MakeUniformId("u_sampler_7")
  };

  const TArray<UniformId, 2> kTextCoordTransform = {
    MakeUniformId("u_scale_tex_0"),
    MakeUniformId("u_scale_tex_1")
  };

  /*
        const Graphics::TextureSampler s_kDefaultTexSampler(
          Graphics::ETextureWrapping::Repeat,
          Graphics::ETextureFilter::Linear,
          Graphics::ETextureFilter::Linear); */
}

namespace Sorex::Graphics
{
  TUniquePointer<GLShaderProgram> GLShaderProgram::Create(
    GLRenderDevice&       glRenderDevice,
    const GLShaderSource& vert,
    const GLShaderSource& frag,
    Status&               status) SRX_NOEXCEPT
  {
    auto vertShader = glRenderDevice.GetOrCreateShader(vert);
    auto fragShader = glRenderDevice.GetOrCreateShader(frag);

    SRX_CHECK(vertShader && fragShader);

    auto program = TUniquePointer<GLShaderProgram>(
      new GLShaderProgram(&glRenderDevice,
                          std::move(vertShader),
                          std::move(fragShader)));

    status = program->Initialize();
    return status.Ok() ? std::move(program) : nullptr;
  }

  GLShaderProgram::GLShaderProgram(GLRenderDevice* glRenderDevice,
                                   GLShaderPtr     vert,
                                   GLShaderPtr     frag) SRX_NOEXCEPT
    : mToken(AllocateResource(glRenderDevice, GLResourceType::ShaderProgram))
    , mMode(ERenderingMode::Triangles)
    , mShaders({ std::move(vert), std::move(frag) })
  {}

  Status GLShaderProgram::Initialize() SRX_NOEXCEPT
  {
    GLRenderDevice* glDevice = GetRenderDevice();
    if (!glDevice)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "Invalid render device");

    return glDevice->BuildShaderProgram(*this, mUniforms);
  }

  GLShaderPtr GLShaderProgram::GetShader(EShaderType shaderType) SRX_NOEXCEPT
  {
    auto it =
      std::find_if(mShaders.begin(),
                   mShaders.end(),
                   [shaderType](const GLShaderPtr& shader) {
                     return (shader && shader->GetShaderType() == shaderType);
                   });

    return (it != mShaders.end() ? (*it) : nullptr);
  }

  Status GLShaderProgram::SetTexture(uint32 index, const Texture2D& texture)
  {
    if (index >= kSamplers.size())
      return SRX_STATUS_MSG(EStatusCode::Out_Of_Range, "invalid sample index");

    GLRenderDevice* glRenderDevice = GetRenderDevice();
    if (!glRenderDevice)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "shader program invalid render device");

    GLUniform* uniform = FindUniform(kSamplers[index].second);
    if (uniform == nullptr || uniform->GetType() != GL_SAMPLER_2D)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "invalid uniform '{}'",
                            kSamplers[index].first);

    auto rc = uniform->SetValue(static_cast<GLenum>(index));
    if (rc != EStatusCode::Ok)
      return SRX_STATUS_MSG(rc, "writing uniform value failed");

    // @note: Convert from ST texture coordinates to UV.
    SizeInt    texSize = texture.GetSize();
    const Vec2 texCoordTransform(1.f / texSize.width, 1.f / texSize.height);
    Status     status = SetTexCoordTransform(0, texCoordTransform);

    if (!status.Ok())
      return status;

    /* if (sampler == nullptr)
      sampler = texture->GetSampler(); */

    // FIXME:     // sampler ? sampler : &s_kDefaultTexSampler
    return glRenderDevice->SetTexture2D(index, &texture);
  }

  Status GLShaderProgram::SetTexCoordTransform(uint32         index,
                                               const Vector2& transform)
  {
    if (index >= kTextCoordTransform.size())
      return SRX_STATUS_MSG(EStatusCode::Out_Of_Range,
                            "invalid texcoord uniform index={}",
                            index);

    GLUniform* uniform = FindUniform(kTextCoordTransform[index].second);
    auto       rc =
      uniform ? uniform->SetVector(transform.data) : EStatusCode::Not_Found;
    if (rc != EStatusCode::No_Error)
      return SRX_STATUS_MSG(rc, "writing texcoord uniform value failed");

    return SRX_OK;
  }

  GLUniform* GLShaderProgram::FindUniform(const hash_t hash)
  {
    auto it = std::find_if(
      mUniforms.begin(),
      mUniforms.end(),
      [hash](const GLUniform& uniform) { return uniform.GetHash() == hash; });

    return it != mUniforms.end() ? &(*it) : nullptr;
  }
}  // namespace
