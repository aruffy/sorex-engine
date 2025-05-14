#include "GLRenderContext.h"


#include "GLRenderDevice.h"

namespace
{
  /*   GLenum ConvBlendFactor(EBlendFactor factor)
    {
      switch (factor)
      {
      case EBlendFactor::Zero:
        return GL_ZERO;
      case EBlendFactor::One:
        return GL_ONE;
      case EBlendFactor::Src_Color:
        return GL_SRC_COLOR;
      case EBlendFactor::One_Minus_Src_Color:
        return GL_ONE_MINUS_SRC_COLOR;
      case EBlendFactor::Dst_Color:
        return GL_DST_COLOR;
      case EBlendFactor::One_Minus_Dst_Color:
        return GL_ONE_MINUS_DST_COLOR;
      case EBlendFactor::Src_Alpha:
        return GL_SRC_ALPHA;
      case EBlendFactor::One_Minus_Src_Alpha:
        return GL_ONE_MINUS_SRC_ALPHA;
      case EBlendFactor::Dst_Alpha:
        return GL_DST_ALPHA;
      case EBlendFactor::One_Minus_Dst_Alpha:
        return GL_ONE_MINUS_DST_ALPHA;
      case EBlendFactor::Const_Color:
        return GL_CONSTANT_COLOR;
      case EBlendFactor::One_Minus_Const_Color:
        return GL_ONE_MINUS_CONSTANT_COLOR;
      case EBlendFactor::Const_Alpha:
        return GL_CONSTANT_ALPHA;
      case EBlendFactor::One_Minus_Const_Alpha:
        return GL_ONE_MINUS_CONSTANT_ALPHA;
      default:
        RFY_NOENTRY("invalid blend factor");
        return GL_ZERO;
      }
    }

    GLenum ConvBlendOperation(EBlendOperation op)
    {
      switch (op)
      {
      case EBlendOperation::Add:
        return GL_FUNC_ADD;
      case EBlendOperation::Subtract:
        return GL_FUNC_SUBTRACT;
      case EBlendOperation::Reverse_Subtract:
        return GL_FUNC_REVERSE_SUBTRACT;
      case EBlendOperation::Min:
        return GL_MIN;
      case EBlendOperation::Max:
        return GL_MAX;
      default:
        RFY_NOENTRY("invalid blend operation");
        return GL_FUNC_ADD;
      }
    }

    inline GLenum ConvTexFilter(ETextureFilter filter)
    {
      RFY_CHECK(filter == ETextureFilter::Nearest
                || filter == ETextureFilter::Linear);
      return (filter == ETextureFilter::Linear) ? GL_LINEAR : GL_NEAREST;
    }

    inline GLenum ConvTexMipmapFilter(ETextureFilter texel, ETextureFilter
    mipmap)
    {
      if (texel == ETextureFilter::Linear)
        return (mipmap == ETextureFilter::Linear) ? GL_LINEAR_MIPMAP_LINEAR
                                                  : GL_NEAREST_MIPMAP_LINEAR;
      else
        return (mipmap == ETextureFilter::Linear) ? GL_LINEAR_MIPMAP_NEAREST
                                                  : GL_NEAREST_MIPMAP_NEAREST;
    }

    GLenum ConvTexWrap(ETextureWrapping wrap)
    {
      switch (wrap)
      {
      case ETextureWrapping::Repeat:
        return GL_REPEAT;
      case ETextureWrapping::Mirrored_Repeat:
        return GL_MIRRORED_REPEAT;
      case ETextureWrapping::Clamp_To_Edge:
        return GL_CLAMP_TO_EDGE;
      case ETextureWrapping::Clamp_To_Border:
        return GL_CLAMP_TO_BORDER;
      default:
        RFY_NOENTRY("invalide texture wrapping");
        return GL_REPEAT;
      }
    } */
}

namespace Sorex::Graphics
{
  GLRenderContext::GLRenderContext(const GLRenderDevice& renderDevice)
    SRX_NOEXCEPT
    : mDevice(renderDevice)
    , mColor(Color(0x1D, 0x18, 0x1D))
  {
    GLint maxTextureUnits = 0;
    SRX_OPENGL_CALL(
      glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits));

    mTextures.resize(maxTextureUnits);
    Reset();
  }

  void GLRenderContext::Reset() SRX_NOEXCEPT
  {
    /*    std::fill(_textures.begin(),
                 _textures.end(),
                 TextureSample{ nullptr, kDefaultTextureSampler, true });
    */

    const Vec4 color = mColor.ToVector();
    glClearColor(color.x, color.y, color.z, color.w);

    // Blend
    // _blend.mode = BlendMode();
    // color = _blend.color.ToVector();
    glDisable(GL_BLEND);
    glBlendColor(color.x, color.y, color.z, color.w);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);

    // Depth testing
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_FALSE);
    glClearDepthf(1.f);

    // Binding
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    // ApplyTextureSampler(GL_TEXTURE_2D, kDefaultTextureSampler, false);
  }

  void GLRenderContext::Clear() SRX_NOEXCEPT
  {
    // TODO: clear other
    glClear(GL_COLOR_BUFFER_BIT);
  }


  void GLRenderContext::Apply(const GLRenderTechnique& technique) SRX_NOEXCEPT
  {
    // ApplyBlendMode(technique.blend);
  }

  /* void GLRenderContext::ApplyBlendMode(BlendMode mode)
  {
    if (_blend.mode == mode)
      return;

    _blend.mode = mode;

    if (!mode.IsEnable())
    {
      glDisable(GL_BLEND);
      return;
    }

    glEnable(GL_BLEND);

    const GLenum alphaOp  = ConvBlendOperation(mode.GetAlphaOperation());
    const GLenum srcAlpha = ConvBlendFactor(mode.GetSrcAlphaFactor());
    const GLenum dstAlpha = ConvBlendFactor(mode.GetDstAlphaFactor());

    if (mode.IsSeparate())
    {
      const GLenum colorOp  = ConvBlendOperation(mode.GetColorOperation());
      const GLenum srcColor = ConvBlendFactor(mode.GetSrcFactor());
      const GLenum dstColor = ConvBlendFactor(mode.GetDstFactor());

      glBlendFuncSeparate(srcColor, dstColor, srcAlpha, dstAlpha);
      glBlendEquationSeparate(colorOp, alphaOp);
    }
    else
    {
      glBlendFunc(srcAlpha, dstAlpha);
      glBlendEquation(alphaOp);
    }
  } */

  /* void GLRenderContext::ApplyTextureSampler(GLenum                target,
                                            const TextureSampler& sampler,
                                            bool                  bMipmaps)
  {
    const GLenum s = ConvTexWrap(sampler.GetWrapS());
    const GLenum t = ConvTexWrap(sampler.GetWrapT());

    OPENGL_CALL(glTexParameteri(target, GL_TEXTURE_WRAP_S, s));
    OPENGL_CALL(glTexParameteri(target, GL_TEXTURE_WRAP_T, t));

    if (s == GL_CLAMP_TO_BORDER || t == GL_CLAMP_TO_BORDER)
    {
      const Vec4 color = sampler.GetBorderColor().ToVector();
      OPENGL_CALL(
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, &color.data[0]));
    }

    const GLenum minf =
      bMipmaps ? ConvTexMipmapFilter(
                   sampler.GetFilter(TextureSampler::EFilterType::Minifying),
                   sampler.GetFilter(TextureSampler::EFilterType::Mipmap))
               : ConvTexFilter(
                   sampler.GetFilter(TextureSampler::EFilterType::Minifying));
    const GLenum magf =
      ConvTexFilter(sampler.GetFilter(TextureSampler::EFilterType::Magnifying));

    OPENGL_CALL(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minf));
    OPENGL_CALL(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magf));
  } */

  Status GLRenderContext::SetTexture(size_t slot, const GLTexture2D& texture)
  {
    if (slot >= mTextures.size())
      return SRX_STATUS_MSG(EStatusCode::Out_Of_Range,
                            "texture sample slot {} is out of range",
                            slot);

    mTextures[slot].texture = &texture;
    return SRX_OK;
  }

  /* error_t GLRenderContext::SetTextureSampler(size_t                slot,
                                             const TextureSampler& sampler)
  {
    if (slot >= _textures.size())
      return Error::Out_Of_Range;

    auto& inst = _textures[slot];

    if (inst.sampler != sampler)
    {
      inst.bUpdateSampler = true;
      inst.sampler        = sampler;
    }

    return Error::Ok;
  } */

  Status GLRenderContext::ActivateTexture(GLenum slot)
  {
    if (slot >= mTextures.size())
      return SRX_STATUS(EStatusCode::Out_Of_Range);

    const TextureSample& texSlot = mTextures[slot];
    const GLResource*    texture =
      texSlot.texture
           ? mDevice.GetDeviceResource(texSlot.texture->GetResourceToken())
           : nullptr;

    if (texture == nullptr || texture->type != GLResourceType::Texture2D
        || !texture->inited)
    {
      SRX_NOENTRY("render context invalid texture");
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "invalid texture state");
    }

    SRX_OPENGL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    SRX_OPENGL_CALL(glBindTexture(texture->target, texture->id));

    /*     if (texSlot.bUpdateSampler)
          ApplyTextureSampler(texture->target,
                              texSlot.sampler,
                              texSlot.texture->HasMipmaps());
     */
    return SRX_OK;
  }

}  // namespace
