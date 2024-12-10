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

#include <Sorex/Graphics/TextureBitmap.h>

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

  size_t TextureBitmap::Resize(int32 width, int32 height)
  {
    _size.Set(width, height);
    const uint32 bytesPerPixel = GetPixelDepth();
    const size_t size          = _size.height * (_size.width * bytesPerPixel);
    _data.resize(size);
    return size;
  }

  size_t TextureBitmap::Resize(int32 width, int32 height, EPixelFormat format)
  {
    _format = format;
    return Resize(width, height);
  }

  byte* TextureBitmap::GetScanLine(size_t  number,
                                   size_t* length /* = nullptr */)
  {
    const size_t len = _size.width * GetPixelDepth();
    if (!len)
      return nullptr;

    const size_t pos = number * len;
    if (pos + len <= _data.size())
    {
      if (length)
        *length = len;
      return &_data[pos];
    }

    return nullptr;
  }

  const byte* TextureBitmap::GetScanLine(size_t  number,
                                         size_t* length /* = nullptr */) const
  {
    const size_t len = _size.width * GetPixelDepth();
    if (!len)
      return nullptr;

    const size_t pos = number * len;
    if (pos + len <= _data.size())
    {
      if (length)
        *length = len;
      return &_data[pos];
    }

    return nullptr;
  }

  void TextureBitmap::FlipVertically()
  {
    if (IsEmpty())
      return;

    byte*         data         = _data.data();
    const int32   bytesPerLine = GetBytesPerLine();
    TVector<byte> line(bytesPerLine);
    for (int32 i = 0; i < _size.height / 2; ++i)
    {
      byte* first = data + (i * bytesPerLine);
      byte* last  = data + ((_size.height - i - 1) * bytesPerLine);

      std::memcpy(line.data(), first, bytesPerLine);
      std::memcpy(first, last, bytesPerLine);
      std::memcpy(last, line.data(), bytesPerLine);
    }
  }

  bool TextureBitmap::ConvertToFormat(EPixelFormat format, Error* error)
  {
    if (_format == format)
      return true;

    return PixelConverter().ConvertBitmap(*this, format, error);
  }

  TUniquePointer<TextureBitmap> TextureBitmap::Convert(EPixelFormat format,
                                                       Error*       error) const
  {
    return PixelConverter().Convert(this, format, error);
  }
}
