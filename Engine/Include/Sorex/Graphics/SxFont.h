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

#include <Sorex/Asset/SxAsset.h>
#include <Sorex/Utils/SxString.h>
#include <Sorex/Math/SxNumericRange.h>

#include "SxTexture2D.h"

namespace Sorex::Graphics
{
  typedef uint32 glyph_t;

  struct FontGlyph
  {
    FontGlyph() = default;
    explicit FontGlyph(glyph_t c)
      : code(c)
      , advance(0u)
    {}

    glyph_t code = 0;
    RectInt rect;

    VecInt2 bearing;
    uint32  advance = 0u;
  };

  enum class EFontType
  {
    None,
    Bitmap,
    Signed_Distance_Field,
    // @TODO: MSDF
  };

  struct FontData
  {
    struct SDFMetrics
    {
      uint8 onedge       = 0u;
      uint8 padding      = 0u;
      float pxlDistScale = 0.f;
    };

    struct Metrics
    {
      int16 ascent = 0;   //< The recommended distance above the baseline for
                          // singled spaced text
      int16 descent = 0;  //< The recommended distance below the baseline for
                          // singled spaced text
      int16 top =
        0;  //< The maximum distance above the baseline for the tallest glyph
      int16 bottom =
        0;  //< The maximum distance below the baseline for the lowest glyph
      int16 leading =
        0;  //< The recommended additional space to add between lines of text

      TOptional<SDFMetrics> sdf;
    };

    EFontType type = EFontType::None;

    String family;
    String style;

    Metrics metrics;

    uint16                   size = 0u;
    TMap<glyph_t, FontGlyph> glyphs;
  };

  class Font: public Resource::Asset
  {
    SRX_RTTI(Graphics::Font, Resource::Asset);

public:
    using Charset = Math::TNumericRange<glyph_t>;
    enum class ECharset
    {
      ASCII = 0,
      Latin,
      Cyrillic
    };

public:
    explicit Font(StringView name)
      : Resource::Asset(name)
    {}

    static Charset GetCharset(ECharset charset) SRX_NOEXCEPT;

    Status Initialize(TUniquePointer<FontData>  data,
                      TSharedPointer<Texture2D> texture);

    EFontType GetType() const { return mData ? mData->type : EFontType::None; }

    const Texture2D*         GetTexture() const { return mTexture.get(); }
    const FontData::Metrics* GetMetrics() const
    {
      return mData ? &mData->metrics : nullptr;
    }
    const String& GetFamily() const
    {
      return mData ? mData->family : Utils::kEmptyString;
    }
    const String& GetStyle() const
    {
      return mData ? mData->style : Utils::kEmptyString;
    }

    uint16 GetSize() const { return mData ? mData->size : 0u; }
    // @FIXME: check how often this is used, map isn't the best choice
    const FontGlyph* GetGlyph(const wchar_t c) const;

private:
    TUniquePointer<FontData>  mData;
    TSharedPointer<Texture2D> mTexture;
  };
}  // namespace Sorex::Graphics