#include "GLShaderProgram.h"

#include "GLRenderDevice.h"

namespace
{
    using namespace Ruffy;
    using UniformId = TPair<Graphics::GLUniform::Name, hash_t>;

    inline UniformId MakeUniformId(GLStringView str)
    {
        return std::make_pair(Graphics::GLUniform::Name(str), Graphics::GLUniformInfo::Hasher(str));
    }

    const TArray<UniformId, RUFFY_OPENGL_TEXTURE_NUMBER> kSamplers = {
        MakeUniformId("u_sampler_0"), MakeUniformId("u_sampler_1"), MakeUniformId("u_sampler_2"),
        MakeUniformId("u_sampler_3"), MakeUniformId("u_sampler_4"), MakeUniformId("u_sampler_5"),
        MakeUniformId("u_sampler_6"), MakeUniformId("u_sampler_7")
    };

    const TArray<UniformId, 2> kTextCoordTransform = { MakeUniformId("u_scale_tex_0"),
                                                       MakeUniformId("u_scale_tex_1") };

    const Graphics::TextureSampler s_kDefaultTexSampler(Graphics::ETextureWrapping::Repeat,
                                                        Graphics::ETextureFilter::Linear,
                                                        Graphics::ETextureFilter::Linear);
}

namespace Ruffy::Graphics
{
    TUniquePointer<GLShaderProgram> GLShaderProgram::CreateFromSource(GLRenderDevice* glRenderDevice,
                                                                      const GLShaderSource::Instance& vert,
                                                                      const GLShaderSource::Instance& frag,
                                                                      Error*                          error)
    {
        if (glRenderDevice == nullptr)
        {
            RFY_MAKE_ERR(error, Error::Invalid_Argument, "[GLShaderProgram] Invalid render device");
            return nullptr;
        }

        GLShaderPointer vertShader = glRenderDevice->GetOrCreateShader(vert, error);
        if (!vertShader)
            return nullptr;

        GLShaderPointer fragShader = glRenderDevice->GetOrCreateShader(frag, error);
        if (!fragShader)
            return nullptr;

        auto program =
            TUniquePointer<GLShaderProgram>(new GLShaderProgram(glRenderDevice, vertShader, fragShader));

        if (program->Initialize(error) == false)
            return nullptr;

        return program;
    }

    GLShaderProgram::GLShaderProgram(GLRenderDevice* glRenderDevice,
                                     GLShaderPointer vert,
                                     GLShaderPointer frag)
        : _glResourceToken(glRenderDevice ? glRenderDevice->Allocate(GLResourceType::ShaderProgram) : nullptr)
        , _mode(ERenderingMode::Triangles)
        , _shaders({ vert, frag })
    {
        RFY_CHECK_MSG(_glResourceToken, "[GLShaderProgram] Allocation failed");
    }

    GLShaderPointer GLShaderProgram::GetShader(EShaderType shaderType)
    {
        auto it = std::find_if(_shaders.begin(), _shaders.end(), [shaderType](const GLShaderPointer& shader) {
            return (shader && shader->GetShaderType() == shaderType);
        });

        return (it != _shaders.end() ? (*it) : nullptr);
    }

    RFY_NODISCARD bool GLShaderProgram::Initialize(Error* error) RFY_NOEXCEPT
    {
        GLRenderDevice* glDevice = GetRenderDevice();
        if (!glDevice)
        {
            RFY_MAKE_ERR(error, Error::Invalid_State, "Invalid render device");
            return false;
        }

        if (glDevice->BuildShaderProgram(this, _uniforms, error) == false)
            return false;

        return true;
    }

    bool GLShaderProgram::SetTexture(uint32                index,
                                     const Texture2D*      texture,
                                     const TextureSampler* sampler,
                                     Error*                error)
    {
        if (index >= kSamplers.size())
        {
            RFY_MAKE_ERR(error, Error::Out_Of_Range, "invalid sample index");
            return false;
        }

        GLRenderDevice* glRenderDevice = GetRenderDevice();
        if (texture == nullptr || texture->GetRenderDevice() == nullptr
            || texture->GetRenderDevice() != glRenderDevice)
        {
            RFY_MAKE_ERR(error, Error::Invalid_Argument, "invalid texture object");
            return false;
        }

        GLUniform* uniform = FindUniform(kSamplers[index].second);
        if (uniform == nullptr || uniform->GetType() != GL_SAMPLER_2D)
        {
            RFY_MAKE_ERR(error, Error::Invalid_State, "invalid uniform '{}'", kSamplers[index].first);
            return false;
        }

        error_t rc = uniform->SetValue(static_cast<GLenum>(index));
        if (rc != Error::No_Error)
        {
            RFY_MAKE_ERR(error, rc, "writing uniform value failed");
            return false;
        }

        // @note: Convert from ST texture coordinates to UV.
        SizeInt    texSize = texture->GetSize();
        const Vec2 textureCoordTransform(1.f / texSize.width, 1.f / texSize.height);
        if (SetTexCoordTransform(0, textureCoordTransform, error) == false)
            return false;

        if (sampler == nullptr)
            sampler = texture->GetSampler();

        rc = glRenderDevice->SetTexture2D(index, texture, sampler ? sampler : &s_kDefaultTexSampler);
        if (rc != Error::Ok)
        {
            RFY_MAKE_ERR(error, rc, "set texture to render device failed");
            return false;
        }

        return true;
    }

    bool GLShaderProgram::SetTexCoordTransform(uint32 index, const Vector2& transform, Error* error)
    {
        if (index > kTextCoordTransform.size())
        {
            RFY_MAKE_ERR(error, Error::Out_Of_Range, "invalid uniform index");
            return false;
        }

        GLUniform* uniform = FindUniform(kTextCoordTransform[index].second);
        error_t    rc      = uniform ? uniform->SetVector(transform.data) : Error::Not_Found;
        if (rc != Error::No_Error)
        {
            RFY_MAKE_ERR(error, rc, "Writing uniform value failed");
            return false;
        }

        return true;
    }

    GLUniform* GLShaderProgram::GetShaderParam(const String& name)
    {
        const hash_t hash =
            GLUniformInfo::Hasher(GLStringView(static_cast<const GLchar*>(name.data()), name.length()));

        return FindUniform(hash);
    }

    const GLUniform* GLShaderProgram::GetShaderParam(const String& name) const
    {
        const hash_t hash =
            GLUniformInfo::Hasher(GLStringView(static_cast<const GLchar*>(name.data()), name.length()));

        return FindUniform(hash);
    }

    GLUniform* GLShaderProgram::FindUniform(const hash_t hash) const
    {
        auto it = std::find_if(_uniforms.begin(), _uniforms.end(), [hash](const GLUniform* uniform) {
            return uniform && uniform->GetHash() == hash;
        });

        return it != _uniforms.end() ? *it : nullptr;
    }
}
