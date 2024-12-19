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

#include <Sorex/CoreMinimal.h>

namespace Sorex::Graphics
{
  // @NOTE: used as array index
  enum class EPixelFormat : uint8
  {
    None = 0u,

    ARGB8,  // D3D8 standard.
    RGBA8,
    ABGR8,     // OpenGL ES standard (as is D3D11).
    RGBA4,     // OpenGL ES standard
    RGBA5551,  // OpenGL ES standard
    ARGB4,     // D3D
    ARGB1555,  // D3D
    RGB8,
    BGR8,    // OpenGL ES standard
    RGB565,  // OpenGL ES standard
    BGR565,
    A8
  };

  namespace Utils
  {
    uint8 GetPixelDepth(const EPixelFormat format) SRX_NOEXCEPT;
  }

  // @here must be pixel format
  enum class ETextureFormat
  {
    None,

    ARGB8,  // D3D8 standard.
    RGBA8,
    ABGR8,     // OpenGL ES standard (as is D3D11).
    RGBA4,     // OpenGL ES standard
    RGBA5551,  // OpenGL ES standard
    ARGB4,     // D3D
    ARGB1555,  // D3D
    RGB8,
    BGR8,    // OpenGL ES standard
    RGB565,  // OpenGL ES standard
    BGR565,
    A8,

    ETC1_RGB,
    ETC2_RGB,
    ETC2_RGBA,
    ETC2_RGB_A1,

    DXT1_RGB,
    DXT1_RGBA,
    DXT3_RGBA,
    DXT5_RGBA,

    PVRTC1_2BPP_RGB,
    PVRTC1_4BPP_RGB,
    PVRTC1_2BPP_RGBA,
    PVRTC1_4BPP_RGBA,
    PVRTC2_2BPP_RGBA,
    PVRTC2_4BPP_RGBA,
  };

  /**
   * @class TextureBitmap - represent raw data of a texture.
   *
   */
  class TextureBitmap final
  {
public:
    using Buffer        = TVector<byte>;
    using Iterator      = Buffer::iterator;
    using ConstIterator = Buffer::const_iterator;

public:
    TextureBitmap() = default;
    TextureBitmap(int32 width, int32 height, EPixelFormat format) SRX_NOEXCEPT;
    SRX_INLINE TextureBitmap(const SizeInt& size,
                             EPixelFormat   format) SRX_NOEXCEPT
      : TextureBitmap(size.width, size.height, format)
    {}

    SRX_INLINE uint8 GetPixelDepth() const SRX_NOEXCEPT
    {
      return Utils::GetPixelDepth(mFormat);
    }

    SRX_INLINE Iterator      begin() { return mData.begin(); }
    SRX_INLINE ConstIterator cbegin() const { return mData.cbegin(); }

    SRX_INLINE Iterator      end() { return mData.end(); }
    SRX_INLINE ConstIterator cend() const { return mData.cend(); }

    SRX_INLINE bool         IsEmpty() const SRX_NOEXCEPT;
    SRX_INLINE EPixelFormat GetPixelFormat() const { return mFormat; }

    SRX_INLINE const SizeInt& GetSize() const { return mSize; }
    SRX_INLINE SizeInt::ValueType GetWidth() const { return mSize.width; }
    SRX_INLINE SizeInt::ValueType GetHeight() const { return mSize.height; }

    SRX_INLINE const byte* GetData() const { return mData.data(); }
    SRX_INLINE byte*       GetData() { return mData.data(); }

    SRX_INLINE size_t GetBytesNumber() const { return mData.size(); }
    SRX_INLINE int32  GetBytesPerLine() const SRX_NOEXCEPT;

    // @TODO: Use TSpan
    SRX_INLINE size_t Copy(TSpan<const byte> data) SRX_NOEXCEPT;
    SRX_INLINE size_t Assign(TSpan<const byte> data,
                             const SizeInt&    size,
                             EPixelFormat      format) SRX_NOEXCEPT;

    SRX_INLINE void Fill(const byte value) SRX_NOEXCEPT
    {
      std::fill(mData.begin(), mData.end(), value);
    }

    // @TODO: at in the debug build or SRX_CHECK
    byte& operator[](const size_t i) SRX_NOEXCEPT { return mData[i]; }
    byte  operator[](size_t i) const { return mData[i]; }

    SRX_INLINE size_t Resize(const SizeInt& size) SRX_NOEXCEPT
    {
      return Resize(size.width, size.height);
    }

    SRX_INLINE size_t Resize(const int32 scanlines) SRX_NOEXCEPT
    {
      return Resize(mSize.width, scanlines);
    }

    SRX_INLINE size_t Resize(const SizeInt& s, EPixelFormat f)
    {
      return Resize(s.width, s.height, f);
    }

    SRX_INLINE size_t Resize(int32        width,
                             int32        height,
                             EPixelFormat format) SRX_NOEXCEPT
    {
      mFormat = format;
      return Resize(width, height);
    };

    size_t Resize(int32 width, int32 height) SRX_NOEXCEPT;

    // cppcheck-suppress functionConst
    TSpan<byte>       GetScanLine(const size_t scanline) SRX_NOEXCEPT;
    TSpan<const byte> GetScanLine(const size_t scanline) const SRX_NOEXCEPT;

    void FlipVertically() SRX_NOEXCEPT;

    // Status                        ConvertToFormat(const EPixelFormat format);
    // TUniquePointer<TextureBitmap> Convert(EPixelFormat format,
    // Status*      status) const;

    SRX_INLINE void Reset() SRX_NOEXCEPT;
    SRX_INLINE void Shrink() { mData.shrink_to_fit(); }

private:
    EPixelFormat mFormat = EPixelFormat::None;

    SizeInt       mSize;
    TVector<byte> mData;
  };

  SRX_INLINE size_t TextureBitmap::Copy(TSpan<const byte> buffer) SRX_NOEXCEPT
  {
    const size_t n = std::min(mData.size(), buffer.size());
    if (!n)
      return 0;

    std::copy_n(buffer.begin(), n, mData.begin());
    return n;
  }

  SRX_INLINE size_t TextureBitmap::Assign(TSpan<const byte> data,
                                          const SizeInt&    size,
                                          EPixelFormat      format) SRX_NOEXCEPT
  {
    size_t n = static_cast<size_t>(size.width * size.height);
    if (!size.IsValid() || data.size() < n || format == EPixelFormat::None)
    {
      SRX_NOEXCEPT("invalid argument");
      return 0;
    }

    n = Resize(size, format);
    std::copy_n(data.begin(), n, mData.begin());
    return n;
  }

  SRX_INLINE bool TextureBitmap::IsEmpty() const SRX_NOEXCEPT
  {
    return (mFormat == EPixelFormat::None || mData.empty() || !mSize.IsValid());
  }

  SRX_INLINE void TextureBitmap::Reset() SRX_NOEXCEPT
  {
    mFormat = EPixelFormat::None;
    mData.clear();
  }

  SRX_INLINE int32 TextureBitmap::GetBytesPerLine() const SRX_NOEXCEPT
  {
    SRX_CHECK(mSize.IsValid() && mFormat != EPixelFormat::None);
    return mSize.width * Utils::GetPixelDepth(mFormat);
  }


}
