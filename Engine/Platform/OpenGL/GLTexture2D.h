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
#include <Sorex/Graphics/SxTexture2D.h>

#include "GLTypes.h"
#include "GLResourceToken.h"

namespace Sorex::Graphics
{
  class GLRenderDevice;
  class GLTexture2D: public Texture2D
  {
    SRX_RTTI(Graphics::GLTexture2D, Graphics::Texture2D);

public:
    struct Params
    {
      GLint level = 0;

      GLsizei width  = 0;
      GLsizei height = 0;

      GLint  internalFormat = GL_NONE;
      GLenum format         = GL_NONE;
      GLenum dataType       = GL_NONE;

      GLint aligment = 0;

      bool            IsValid() const SRX_NOEXCEPT;
      SRX_INLINE void Reset() SRX_NOEXCEPT { *this = Params(); }
    };

public:
    GLTexture2D(StringView      name,
                GLRenderDevice& glDevice,
                bool            bMipmaps = false);
    virtual ~GLTexture2D() override = default;

    GLTexture2D(const GLTexture2D& other)            = delete;
    GLTexture2D& operator=(const GLTexture2D& other) = delete;

    SRX_INLINE const GLResourceReference* GetResourceToken() const
    {
      return mToken.get();
    }

    // API Texture2D
    virtual Status  Initialize(TUniquePointer<TextureBitmap> bitmap) override;
    virtual SizeInt GetSize() const override
    {
      return SizeInt(static_cast<int>(mTexParams.width),
                     static_cast<int>(mTexParams.height));
    }
    virtual Rectangle GetContentRect() const override { return mContentRect; }
    virtual ETextureFormat GetFormat() const override;

    // } virtual void      SetSampler(const TextureSampler* sampler) override;
    /* virtual const TextureSampler* GetSampler() const override
    {
      return _sampler.get();
    } */

    /* SRX_INLINE const GLTextureParams& GetParameters() const
    {
      return mTexParams;
    } */
    /* SRX_INLINE const uint8* GetData() const
    {
      return mBitmap ? mBitmap->GetData() : nullptr;
    } */

    // SRX_INLINE bool HasMipmaps() const { return (_options & Option_Mipmaps);
    // }

protected:
    virtual Status OnUnload() override { return SRX_OK; }

private:
    static bool GetTextureParameters(const TextureBitmap& bitmap,
                                     Params&              params) SRX_NOEXCEPT;

private:
    GLResourceToken mToken;

    Params    mTexParams;
    Rectangle mContentRect;

    // TUniquePointer<TextureBitmap> mBitmap;
    // TUniquePointer<TextureSampler> _sampler;
    // bool      mIsUploaded;

    // bool mMipmaps;
  };
}  // namespace
