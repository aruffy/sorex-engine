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

#include "GLTexture2D.h"

#include "GLRenderDevice.h"

namespace
{
  SRX_INLINE bool IsPowerOfTwo(const Sorex::int32 x) SRX_NOEXCEPT
  {
    return (x > 0 && ((x & (x - 1)) == 0));
  }

  SRX_INLINE Sorex::int32 GetPowerOfTwo(const Sorex::int32 x) SRX_NOEXCEPT
  {
    Sorex::int32 p = 1;
    while (p < x)
      p <<= 1;
    return p;
  }
}  // namespace

namespace Sorex::Graphics
{
  GLTexture2D::GLTexture2D(StringView      name,
                           GLRenderDevice& glDevice,
                           bool            bMipmaps)
    : Texture2D(name)
    , mToken(AllocateResource(&glDevice, GLResourceType::Texture2D))
  // , mMipmaps(bMipmaps)
  {}

  Status GLTexture2D::Initialize(TUniquePointer<TextureBitmap> bitmap)
  {
    GLRenderDevice* glDevice = mToken ? mToken->GetRenderDevice() : nullptr;
    if (glDevice == nullptr)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "invalid render device token");

    if (mTexParams.format != GL_NONE)
      return SRX_STATUS_MSG(EStatusCode::Not_Permitted,
                            "texture uploaded to the OpenGL");

    if (!bitmap || bitmap->IsEmpty())
      return SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                            "invalid texture bitmap");

    if (GetTextureParameters(*bitmap, mTexParams) == false)
      return SRX_STATUS_MSG(EStatusCode::Invalid_Format,
                            "texture format: not supported or invalid");

    const SizeInt& origSize = bitmap->GetSize();
    if (mTexParams.width != origSize.width
        || mTexParams.height != origSize.height)
    {
      SRX_CHECK(mTexParams.width > origSize.width
                && mTexParams.height > origSize.height);

      // @NOTE: make texture size power of two
      TextureBitmap tmpBitmap{ mTexParams.width,
                               mTexParams.height,
                               bitmap->GetPixelFormat() };
      tmpBitmap.Fill(0xff);
      for (int32 scanline = 0; scanline < origSize.height; ++scanline)
      {
        std::copy_n(bitmap->GetScanLine(scanline).data(),
                    bitmap->GetBytesPerLine(),
                    tmpBitmap.GetScanLine(scanline).data());
      }

      bitmap->Swap(tmpBitmap);
    }

    SRX_DEBUG("[GLTexture2D] Initialize '{}' size {}x{} format {}.",
              GetName(),
              mTexParams.width,
              mTexParams.height,
              (int)mTexParams.format);

    mContentRect = Rect(Point(0.f, 0.f),
                        Size(static_cast<float>(origSize.width),
                             static_cast<float>(origSize.height)));

    // mIsUploaded = glDevice->InitializeTexture2D(this, error);

    return SRX_OK;
  }

  ETextureFormat GLTexture2D::GetFormat() const
  {
    switch (mTexParams.format)
    {
    case GL_RGBA:
      return ETextureFormat::RGBA8;
    case GL_RGB:
      return ETextureFormat::RGB8;
    case GL_RGB565:
      return ETextureFormat::RGB565;
    case GL_RGBA4:
      return ETextureFormat::RGBA4;
    case GL_RGB5_A1:
      return ETextureFormat::RGBA5551;
    case GL_RED:
      return ETextureFormat::A8;
    default:
      return ETextureFormat::None;
    }
  }

  static GLint GetDataAlignment(const TextureBitmap& bitmap)
  {
    constexpr int32 kDefaultAligment = 4;
    switch (bitmap.GetBytesPerLine() % 4)
    {
    case 1:
    case 3:
      return 1;

    case 2:
      return 2;

    default:
      return kDefaultAligment;
    }
  }

  bool GLTexture2D::GetTextureParameters(const TextureBitmap& bitmap,
                                         GLTexture2D::Params& params)
    SRX_NOEXCEPT
  {
    switch (bitmap.GetPixelFormat())
    {
    case EPixelFormat::RGBA8:
      params.internalFormat = GL_RGBA;
      params.dataType       = GL_UNSIGNED_BYTE;
      break;

// TODO: Add supported ext formats
#ifdef GLAD_GL_EXT_texture_format_BGRA8888
    case EPixelFormat::ARGB8888:
      params.internalFormat = GL_BGRA;
      params.dataType       = GL_UNSIGNED_BYTE;
      break;
#endif

    case EPixelFormat::RGB8:
    case EPixelFormat::BGR8:
      params.internalFormat = GL_RGB;
      params.dataType       = GL_UNSIGNED_BYTE;
      break;

    case EPixelFormat::RGB565:
      params.internalFormat = GL_RGB;
      params.dataType       = GL_UNSIGNED_SHORT_5_6_5;
      break;

    case EPixelFormat::RGBA4:
      params.internalFormat = GL_RGBA;
      params.dataType       = GL_UNSIGNED_SHORT_4_4_4_4;
      break;

    case EPixelFormat::RGBA5551:
      params.internalFormat = GL_RGBA;
      params.dataType       = GL_UNSIGNED_SHORT_5_5_5_1;
      break;

    case EPixelFormat::A8:
      params.internalFormat = GL_RED;
      params.dataType       = GL_UNSIGNED_BYTE;
      break;

    default:
      return false;
    }

    params.format = params.internalFormat;
    params.level  = 0;

    const SizeInt& size = bitmap.GetSize();
    // TODO: Check OpenGL es 3.0 for power of two
    params.width =
      IsPowerOfTwo(size.width) ? size.width : GetPowerOfTwo(size.width);
    params.height =
      IsPowerOfTwo(size.height) ? size.height : GetPowerOfTwo(size.height);

    params.aligment = GetDataAlignment(bitmap);
    return true;
  }

  bool GLTexture2D::Params::IsValid() const noexcept
  {
    const bool isValidSize =
      (width > 0 && height > 0); /* TODO: Condition for IsPowerOfTwo(width) &&
                                    IsPowerOfTwo(height); */
    return (isValidSize && internalFormat != GL_NONE && format != GL_NONE
            && dataType != GL_NONE);
  }
}  // namespace
