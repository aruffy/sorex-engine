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

#include "SxTgaImageLoader.h"

#include <Sorex/SxMemoryStream.h>
#include <Sorex/Utils/SxBytesUtils.h>

namespace
{
  using namespace Sorex::Resource;
  static bool ReadTgaHeader(TgaImageLoader::ImageHeader& header,
                            const Sorex::byte*           data,
                            size_t                       size)
  {
    if (data == nullptr || size < TgaImageLoader::kTgaImageHeaderSize)
      return false;

    const Sorex::ReadOnlyMemoryStream::EParameters params =
      Sorex::Utils::IsLittleEndian()
        ? Sorex::ReadOnlyMemoryStream::EParameters::Integer_BytesSwap_Disable
        : Sorex::ReadOnlyMemoryStream::EParameters::Default;

    Sorex::ReadOnlyMemoryStream stream(data, size, params);
    stream.ReadIntUnsafe(header.idLength);
    stream.ReadIntUnsafe(header.colourMapType);
    stream.ReadIntUnsafe(header.dataTypeCode);
    stream.ReadIntUnsafe(header.colourMapOrigin);
    stream.ReadIntUnsafe(header.colourMapLength);
    stream.ReadIntUnsafe(header.colourMapDepth);
    stream.ReadIntUnsafe(header.xOrigin);
    stream.ReadIntUnsafe(header.yOrigin);
    stream.ReadIntUnsafe(header.width);
    stream.ReadIntUnsafe(header.height);
    stream.ReadIntUnsafe(header.bitsPerPixel);
    stream.ReadIntUnsafe(header.imageDescriptor);

    return true;
  }

  static bool CheckTgaHeader(const TgaImageLoader::ImageHeader& header)
  {
    /*
        0  -  No image data included.
        1  -  Uncompressed, color-mapped images.
        2  -  Uncompressed, RGB images.
        3  -  Uncompressed, black and white images.
        9  -  Runlength encoded color-mapped images.
       10  -  Runlength encoded RGB images.
       11  -  Compressed, black and white images.
       32  -  Compressed color-mapped data, using Huffman, Delta, and
              runlength encoding.
       33  -  Compressed color-mapped data, using Huffman, Delta, and
              runlength encoding.  4-pass quadtree-type process.
    */
    switch (header.dataTypeCode)
    {
    case 1:
    case 2:
    case 3:
      break;

    default:
      return false;
    }

    // 0 if image file contains no color map
    // 1 if present
    // 2�127 reserved by Truevision
    // 128�255 available for developer use
    if (header.colourMapType > 1)
      return false;

    switch (header.bitsPerPixel)
    {
    case 8:
    case 15:
    case 16:
    case 24:
    case 32:
      break;

    default:
      return false;
    }

    return true;
  }
}  // namespace

namespace Sorex::Resource
{
  TUniquePointer<Graphics::TextureBitmap> TgaImageLoader::LoadImage(
    Stream& stream,
    Status* status)
  {
    byte        buffer[kTgaImageHeaderSize];
    ImageHeader header;

    if (!stream.Check(EAccessMode::Read))
    {
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Invalid_Argument,
                         "invalid stream");
      return nullptr;
    }

    if (stream.Read(buffer, kTgaImageHeaderSize) != kTgaImageHeaderSize)
    {
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Invalid_Format,
                         "TGA header reading failed");
      return nullptr;
    }

    ReadTgaHeader(header, buffer, kTgaImageHeaderSize);
    if (!CheckTgaHeader(header))
    {
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Not_Supported,
                         "invalid or not supproted TGA image format");
      return nullptr;
    }

    switch (header.dataTypeCode)
    {
    case 1:
      return LoadUncompressedColorMappedImage(stream, header, status);
    case 2:
      return LoadUncompressedTrueColorImage(stream, header, status);
    case 3:
      return LoadMonochromeImage(stream, header, status);
    default:
      SRX_NOENTRY("invalid tga format");
      return nullptr;
    }
  }

  TUniquePointer<Graphics::TextureBitmap>
  TgaImageLoader::LoadUncompressedColorMappedImage(Stream&            stream,
                                                   const ImageHeader& header,
                                                   Status*            status)
  {
    if (header.colourMapType != 1)
    {
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Invalid_Format,
                         "Invalid image color map type");
      return nullptr;
    }

    const int32   colorSize   = header.colourMapDepth >> 3;
    const int32   paletteSize = header.colourMapLength * colorSize;
    TVector<byte> palette(paletteSize, 0);

    if (header.idLength)
    {
      // @note: skip Image Identification Field
      // use palette buffer
      const bool bResize = header.idLength > paletteSize;
      if (bResize)
        palette.resize(header.idLength);

      if (stream.Read(palette, header.idLength) != header.idLength)
      {
        SRX_STATUS_PTR_MSG(status,
                           EStatusCode::Bad_File,
                           "Read Image Identification Field failed");
        return nullptr;
      }

      if (bResize)
        palette.resize(paletteSize);
    }

    if (int32 n = stream.Read(palette, paletteSize); n != paletteSize)
    {
      SRX_STATUS_PTR_MSG(
        status,
        EStatusCode::Bad_File,
        "Reading failed (color map length {}): {}",
        paletteSize,
        ((n < 0) ? stream.GetStatus().ToString() : "not enough data"));

      return nullptr;
    }

    TUniquePointer<Graphics::TextureBitmap> bitmap =
      MakeUnique<Graphics::TextureBitmap>(header.width,
                                          header.height,
                                          Graphics::EPixelFormat::RGB8);

    byte n;
    for (int32 line = header.height - 1; line >= 0; --line)
    {
      TSpan<byte> scanline = bitmap->GetScanLine(line);
      if (scanline.empty())
      {
        SRX_STATUS_PTR_MSG(status,
                           EStatusCode::No_Space,
                           "no scanline {} index",
                           line);
        return nullptr;
      }

      SRX_CHECK(scanline.size() >= header.width);
      byte* data = scanline.data();
      for (uint16 i = 0; i < header.width; ++i)
      {
        if (!stream.Next(n))
        {
          SRX_STATUS_PTR_MSG(status,
                             EStatusCode::Bad_File,
                             "stream reading failed: pallete color {}/{}",
                             i,
                             header.width);
          return nullptr;
        }

        std::copy_n(&palette[n * colorSize], colorSize, data);
        data += colorSize;
      }
    }

    return bitmap;
  }

  TUniquePointer<Graphics::TextureBitmap>
  TgaImageLoader::LoadUncompressedTrueColorImage(Stream&            stream,
                                                 const ImageHeader& header,
                                                 Status*            status)
  {
    if (header.colourMapType && header.colourMapLength)
    {
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Invalid_Format,
                         "Invalid image color map type");
      return nullptr;
    }

    Graphics::EPixelFormat format;
    switch (header.bitsPerPixel)
    {
    case 16:
      format = Graphics::EPixelFormat::ARGB1555;
      break;
    case 24:
      format = Graphics::EPixelFormat::RGB8;
      break;
    case 32:
      format = Graphics::EPixelFormat::ARGB8;
      break;
    default:
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Not_Supported,
                         "Pixel depth of {} is not supported",
                         (int32)header.bitsPerPixel);
      return nullptr;
    }

    TUniquePointer<Graphics::TextureBitmap> bitmap =
      MakeUnique<Graphics::TextureBitmap>(header.width, header.height, format);

    SRX_CHECK((header.imageDescriptor & 0x10)
              == 0);  // Origin in lower left-hand corner

    const int32 scanlineSize = bitmap->GetBytesPerLine();
    for (int32 i = header.height - 1; i >= 0; --i)
    {
      if (stream.Read(bitmap->GetScanLine(i), scanlineSize) != scanlineSize)
      {
        SRX_STATUS_PTR_MSG(status,
                           EStatusCode::Invalid_Format,
                           "read scanline {} failed",
                           i);
        return nullptr;
      }
    }
    return bitmap;
  }

  TUniquePointer<Graphics::TextureBitmap> TgaImageLoader::LoadMonochromeImage(
    Stream&            stream,
    const ImageHeader& header,
    Status*            status)
  {
    if (header.bitsPerPixel != 8)
    {
      SRX_STATUS_PTR_MSG(
        status,
        EStatusCode::Invalid_Format,
        "pixel depth of {} is not supported for monochrome images",
        header.bitsPerPixel);
      return nullptr;
    }

    TUniquePointer<Graphics::TextureBitmap> bitmap =
      MakeUnique<Graphics::TextureBitmap>(header.width,
                                          header.height,
                                          Graphics::EPixelFormat::A8);

    SRX_CHECK((header.imageDescriptor & 0x10)
              == 0);  // Origin in lower left-hand corner

    const size_t bytesPerLine = bitmap->GetBytesPerLine();
    for (int32 i = header.height - 1; i >= 0; --i)
    {
      if (stream.Read(bitmap->GetScanLine(i), bytesPerLine)
          != static_cast<ssize_t>(bytesPerLine))
      {
        SRX_STATUS_PTR_MSG(status,
                           EStatusCode::Invalid_Format,
                           "read scanline {} failed",
                           i);
        return nullptr;
      }
    }

    return bitmap;
  }
}
