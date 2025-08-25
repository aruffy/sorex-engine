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

#include <Sorex/Graphics/SxFont.h>

namespace Sorex::Graphics
{
  Font::Charset Font::GetCharset(ECharset charset) SRX_NOEXCEPT
  {
    switch (charset)
    {
    case ECharset::ASCII:
      return Charset(0x20, 0x7F);
    case ECharset::Latin:
      return Charset(0x20, 0x100);
    case ECharset::Cyrillic:
      return Charset(0x400, 0x500);
    default:
      SRX_NOENTRY("unknown charset");
      return Charset(0x20, 0x7F);  // ASCII
    }
  }

  Status Font::Initialize(TUniquePointer<FontData>  data,
                          TSharedPointer<Texture2D> texture)
  {
    if (!data || data->glyphs.empty() || texture == nullptr)
      return SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                            "Font data or texture is invalid");

    // @todo: add spec symbol

    mData    = std::move(data);
    mTexture = std::move(texture);

    return SRX_OK;
  }

  const FontGlyph* Font::GetGlyph(wchar_t c) const
  {
    SRX_CHECK(mData);

    auto it = mData->glyphs.find(static_cast<glyph_t>(c));
    return it != mData->glyphs.end() ? &it->second : nullptr;
  }
}  // namespace Sorex::Graphics