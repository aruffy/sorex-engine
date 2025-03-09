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

#include <Sorex/Asset/SxTextureLoader.h>

namespace Sorex::Resource
{
  class TgaImageLoader final: public ImageLoader
  {
public:
    static constexpr size_t kTgaImageHeaderSize = 18;
    struct ImageHeader
    {
      uint8  idLength;
      uint8  colourMapType;
      uint8  dataTypeCode;
      uint16 colourMapOrigin;
      uint16 colourMapLength;
      uint8  colourMapDepth;
      uint16 xOrigin;
      uint16 yOrigin;
      uint16 width;
      uint16 height;
      uint8  bitsPerPixel;
      uint8  imageDescriptor;
    };

public:
    virtual TUniquePointer<Graphics::TextureBitmap> LoadImage(
      Stream& stream,
      Status* status) override;

private:
    TUniquePointer<Graphics::TextureBitmap> LoadUncompressedColorMappedImage(
      Stream&            stream,
      const ImageHeader& header,
      Status*            status);
    TUniquePointer<Graphics::TextureBitmap> LoadUncompressedTrueColorImage(
      Stream&            stream,
      const ImageHeader& header,
      Status*            status);
    TUniquePointer<Graphics::TextureBitmap> LoadMonochromeImage(
      Stream&            stream,
      const ImageHeader& header,
      Status*            status);
  };
}  // namespace
