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
      bool            IsValid() const SRX_NOEXCEPT;
      SRX_INLINE void Reset() SRX_NOEXCEPT { *this = Params(); }
    };

public:
    GLTexture2D(StringView      name,
                GLRenderDevice& glDevice,
                bool            bMipmaps = false);
    virtual ~GLTexture2D() override;

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
      return SizeInt(static_cast<SizeInt::ValueType>(mWidth),
                     static_cast<SizeInt::ValueType>(mHeight));
    }

    virtual Rectangle GetContentRect() const override { return mContentRect; }
    virtual ETextureFormat GetFormat() const override;

    bool            IsValid() const;
    SRX_INLINE void GetTexImageFormat(GLint&  internalFormat,
                                      GLenum& format,
                                      GLenum& type) const;
    SRX_INLINE TPair<const GLvoid*, GLint> GetTexImageData() const
    {
      return std::make_pair(static_cast<const GLvoid*>(mData), mAligment);
    }

protected:
    virtual Status OnUnload() override { return SRX_OK; }

private:
    bool            ApplyTexImageParams(const TextureBitmap& bitmap);
    SRX_INLINE void ResetTexImageParams();

private:
    GLResourceToken mToken;

    GLsizei mWidth;
    GLsizei mHeight;

    GLint  mInternalFormat;
    GLenum mFormat;
    GLenum mDataType;

    GLint          mAligment;
    const GLubyte* mData;

    Rectangle mContentRect;

    bool mMipmaps;
  };

  SRX_INLINE void GLTexture2D::GetTexImageFormat(GLint&  internalFormat,
                                                 GLenum& format,
                                                 GLenum& type) const
  {
    internalFormat = mInternalFormat;
    format         = mFormat;
    type           = mDataType;
  }

  SRX_INLINE void GLTexture2D::ResetTexImageParams()
  {
    mWidth = mHeight = 0;
    mFormat          = GL_NONE;
    mInternalFormat = mDataType = GL_NONE;
    mData                       = nullptr;
  }
}  // namespace
