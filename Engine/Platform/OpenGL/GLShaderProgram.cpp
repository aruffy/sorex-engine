#include "GLShaderProgram.h"

#include "GLRenderDevice.h"

namespace
{
  using namespace Sorex;
  /* using UniformId = TPair<Graphics::GLUniform::Name, hash_t>;

  inline UniformId MakeUniformId(GLStringView str)
  {
    return std::make_pair(Graphics::GLUniform::Name(str),
                          Graphics::GLUniformInfo::Hasher(str));
  }

  const TArray<UniformId, RUFFY_OPENGL_TEXTURE_NUMBER> kSamplers = {
    MakeUniformId("u_sampler_0"), MakeUniformId("u_sampler_1"),
    MakeUniformId("u_sampler_2"), MakeUniformId("u_sampler_3"),
    MakeUniformId("u_sampler_4"), MakeUniformId("u_sampler_5"),
    MakeUniformId("u_sampler_6"), MakeUniformId("u_sampler_7")
  };

  const TArray<UniformId, 2> kTextCoordTransform = {
    MakeUniformId("u_scale_tex_0"),
    MakeUniformId("u_scale_tex_1")
  };

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
}  // namespace
