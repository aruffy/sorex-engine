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

#include <Sorex/SxAssert.h>
#include <Sorex/Graphics/SxTextureBitmap.h>

namespace Sorex::Graphics
{
  namespace Utils
  {
    uint8 GetPixelDepth(const EPixelFormat format) SRX_NOEXCEPT
    {
      constexpr static TArray<uint8, 13> kPixelDepths = { 0, 4, 4, 4, 2, 2, 2,
                                                          2, 3, 3, 2, 2, 1 };

      const size_t indx = static_cast<uint8>(format);
      if (indx >= kPixelDepths.size())
      {
        SRX_NOENTRY("invalid pixel format");
        return 0;
      }

      return kPixelDepths[indx];
    }
  }  // namespace

  TextureBitmap::TextureBitmap(int32        width,
                               int32        height,
                               EPixelFormat format) SRX_NOEXCEPT
  {
    Resize(width, height, format);
  }

  size_t TextureBitmap::Resize(int32 width, int32 height) SRX_NOEXCEPT
  {
    const SizeInt newSize(width, height);
    if (!newSize.IsValid())
    {
      SRX_NOENTRY("invalid size");
      return mData.size();
    }

    SRX_CHECK(GetPixelDepth() > 0);
    const size_t size = newSize.height * newSize.width * GetPixelDepth();
    mSize             = std::move(newSize);
    mData.resize(size);
    return size;
  }

  TSpan<byte> TextureBitmap::GetScanLine(const size_t scanline) SRX_NOEXCEPT
  {
    const size_t bytesPerLine = GetBytesPerLine();
    if (!bytesPerLine)
      return TSpan<byte>{};

    const size_t pos = scanline * bytesPerLine;
    if (pos + bytesPerLine > mData.size())
      return TSpan<byte>{};

    return TSpan<byte>{ &mData[pos], bytesPerLine };
  }

  TSpan<const byte> TextureBitmap::GetScanLine(const size_t scanline) const
    SRX_NOEXCEPT
  {
    const size_t bytesPerLine = GetBytesPerLine();
    if (!bytesPerLine)
      return TSpan<const byte>{};

    const size_t pos = scanline * bytesPerLine;
    if (pos + bytesPerLine > mData.size())
      return TSpan<const byte>{};

    return TSpan<const byte>{ &mData[pos], bytesPerLine };
  }

  void TextureBitmap::FlipVertically() SRX_NOEXCEPT
  {
    if (IsEmpty())
      return;

    byte*         data         = mData.data();
    const int32   bytesPerLine = GetBytesPerLine();
    TVector<byte> line(bytesPerLine);
    for (int32 i = 0; i < mSize.height / 2; ++i)
    {
      byte* first = data + (i * bytesPerLine);
      byte* last  = data + ((mSize.height - i - 1) * bytesPerLine);

      std::memcpy(line.data(), first, bytesPerLine);
      std::memcpy(first, last, bytesPerLine);
      std::memcpy(last, line.data(), bytesPerLine);
    }
  }

  /* bool TextureBitmap::ConvertToFormat(EPixelFormat format, Error* error)
  {
    if (_format == format)
      return true;

    return PixelConverter().ConvertBitmap(*this, format, error);
  }

  TUniquePointer<TextureBitmap> TextureBitmap::Convert(EPixelFormat format,
                                                       Error*       error) const
  {
    return PixelConverter().Convert(this, format, error);
  } */
}
