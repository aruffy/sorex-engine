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
  constexpr Sorex::int32 kDefaultDataAligment = 4;

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
    , mWidth(0)
    , mHeight(0)
    , mInternalFormat(GL_NONE)
    , mFormat(GL_NONE)
    , mDataType(GL_NONE)
    , mAligment(4)
    , mData(nullptr)
    , mMipmaps(bMipmaps)
  {}

  GLTexture2D::~GLTexture2D()
  {}

  Status GLTexture2D::Initialize(TUniquePointer<TextureBitmap> bitmap)
  {
    GLRenderDevice* glDevice = mToken ? mToken->GetRenderDevice() : nullptr;
    if (glDevice == nullptr)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "invalid render device token");

    if (mFormat != GL_NONE)
      return SRX_STATUS_MSG(EStatusCode::Not_Permitted,
                            "texture uploaded to the OpenGL");

    if (!bitmap || bitmap->IsEmpty())
      return SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                            "invalid texture bitmap");

    if (!ApplyTexImageParams(*bitmap))
      return SRX_STATUS_MSG(EStatusCode::Invalid_Format,
                            "texture format: not supported or invalid");

    const SizeInt& origSize = bitmap->GetSize();
    if (mWidth != origSize.width || mHeight != origSize.height)
    {
      SRX_CHECK(mWidth > origSize.width && mHeight > origSize.height);

      // @NOTE: make texture size power of two
      TextureBitmap tmpBitmap{ mWidth, mHeight, bitmap->GetPixelFormat() };
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
              mWidth,
              mHeight,
              mFormat);

    mContentRect = Rect(Point(0.f, 0.f),
                        Size(static_cast<float>(origSize.width),
                             static_cast<float>(origSize.height)));

    Status status = glDevice->InitializeTexture(*this, mMipmaps);
    if (!status.Ok())
      ResetTexImageParams();

    mData = nullptr;
    return status;
  }

  ETextureFormat GLTexture2D::GetFormat() const
  {
    switch (mFormat)
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
    switch (bitmap.GetBytesPerLine() % 4)
    {
    case 1:
    case 3:
      return 1;
    case 2:
      return 2;
    default:
      return kDefaultDataAligment;
    }
  }

  bool GLTexture2D::ApplyTexImageParams(const TextureBitmap& bitmap)
  {
    switch (bitmap.GetPixelFormat())
    {
      // TODO: Add supported ext formats
      // TODO: Add ES3.0 formats
    case EPixelFormat::RGBA8:
      mInternalFormat = GL_RGBA;
      mDataType       = GL_UNSIGNED_BYTE;
      break;
    case EPixelFormat::RGB8:
    case EPixelFormat::BGR8:
      mInternalFormat = GL_RGB;
      mDataType       = GL_UNSIGNED_BYTE;
      break;

    case EPixelFormat::RGB565:
      mInternalFormat = GL_RGB;
      mDataType       = GL_UNSIGNED_SHORT_5_6_5;
      break;

    case EPixelFormat::RGBA4:
      mInternalFormat = GL_RGBA;
      mDataType       = GL_UNSIGNED_SHORT_4_4_4_4;
      break;

    case EPixelFormat::RGBA5551:
      mInternalFormat = GL_RGBA;
      mDataType       = GL_UNSIGNED_SHORT_5_5_5_1;
      break;

    case EPixelFormat::A8:
      mInternalFormat = GL_RED;
      mDataType       = GL_UNSIGNED_BYTE;
      break;

    default:
      return false;
    }

    mFormat = mInternalFormat;  // @FIXME:

    // TODO: Check OpenGL es 3.0 for power of two
    const SizeInt& size = bitmap.GetSize();
    mWidth = IsPowerOfTwo(size.width) ? size.width : GetPowerOfTwo(size.width);
    mHeight =
      IsPowerOfTwo(size.height) ? size.height : GetPowerOfTwo(size.height);
    mAligment = GetDataAlignment(bitmap);
    mData     = reinterpret_cast<const GLubyte*>(bitmap.GetData());
    return true;
  }

  bool GLTexture2D::IsValid() const
  {
    const bool isValidSize =
      (mWidth > 0 && mHeight > 0); /* TODO: Condition for IsPowerOfTwo(width) &&
                                    IsPowerOfTwo(height); */
    return (mData && isValidSize && mInternalFormat != GL_NONE
            && mFormat != GL_NONE && mDataType != GL_NONE);
  }
}  // namespace
