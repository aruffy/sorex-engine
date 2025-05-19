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

#include "SxDefaultImageLoader.h"
#include "Sorex/Graphics/SxTextureBitmap.h"

#define STBI_NO_PSD
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PNM
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace
{
  int StbReadCallback(void* file, char* data, int size)
  {
    SRX_CHECK(file && data && size > 0);

    if (size <= 0)
      return 0;

    Sorex::Stream* const stream = reinterpret_cast<Sorex::Stream*>(file);

    const Sorex::ssize_t n = stream->Read(
      Sorex::TSpan<Sorex::byte>{ reinterpret_cast<Sorex::byte*>(data),
                                 static_cast<size_t>(size) });

    if (n == SRX_READ_ERROR)
      return -1;

    return static_cast<int>(n);
  }

  void StbSkipCallback(void* file, int n)
  {
    SRX_CHECK(file);
    Sorex::Stream* const stream = reinterpret_cast<Sorex::Stream*>(file);
    SRX_VERIFY(stream->Seek(n, Sorex::ESeekMode::Current));
  }

  int StbEndOfFileCallback(void* file)
  {
    SRX_CHECK(file);
    return reinterpret_cast<const Sorex::Stream*>(file)->EndOfFile();
  }

  const stbi_io_callbacks stbiCallbacks = { StbReadCallback,
                                            StbSkipCallback,
                                            StbEndOfFileCallback };
}

namespace Sorex::Resource
{
  TUniquePointer<Graphics::TextureBitmap> DefaultImageLoader::LoadImage(
    Stream& stream,
    Status* status)
  {
    int      x = 0, y = 0, channels = 0;
    stbi_uc* imgData = stbi_load_from_callbacks(&stbiCallbacks,
                                                (void*)(&stream),
                                                &x,
                                                &y,
                                                &channels,
                                                0);

    if (!imgData)
    {
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Not_Available,
                         "stb image laoding failed");
      return nullptr;
    }

    SRX_CHECK(x > 0 && y > 0 && channels > 0);
    Graphics::EPixelFormat pxlFormat = Graphics::EPixelFormat::None;
    switch (channels)
    {
    case 1:
      pxlFormat = Graphics::EPixelFormat::A8;
      break;
    case 2:
      pxlFormat = Graphics::EPixelFormat::RGBA4;
      break;
    case 3:
      pxlFormat = Graphics::EPixelFormat::RGB8;
      break;
    case 4:
      pxlFormat = Graphics::EPixelFormat::RGBA8;
      break;
    default:
      SRX_NOENTRY("ivalid number of image channels");
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Invalid_Format,
                         "stb image channels number is not supported");
      stbi_image_free(imgData);
      return nullptr;
    }

    TUniquePointer<Graphics::TextureBitmap> bitmap =
      MakeUnique<Graphics::TextureBitmap>(x, y, pxlFormat);

    const size_t dataSize = static_cast<size_t>(x) * y * channels;
    SRX_ASSERT(bitmap->GetBytesNumber() == dataSize);

    bitmap->Copy({ static_cast<const byte*>(imgData), dataSize });
    stbi_image_free(imgData);

    return bitmap;
  }
}  // namespace
