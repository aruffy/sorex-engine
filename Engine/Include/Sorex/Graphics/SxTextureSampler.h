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

namespace Sorex::Graphics
{
  enum class ETextureFilter : uint8
  {
    Nearest = 0u,
    Linear
  };

  enum class ETextureWrapping : uint8
  {
    Repeat = 0u,
    Mirrored_Repeat,
    Clamp_To_Edge,
    Clamp_To_Border,
  };

  class TextureSampler
  {
public:
    enum class EFilterType : uint8
    {
      Minifying = 0u,
      Magnifying,
      Mipmap
    };

public:
    TextureSampler();
    explicit TextureSampler(ETextureWrapping wrap, Color color = Color::Black);
    TextureSampler(ETextureWrapping wrap,
                   ETextureFilter   filters,
                   ETextureFilter   fmipmap = ETextureFilter::Nearest);

    TextureSampler(const TextureSampler& other)            = default;
    TextureSampler& operator=(const TextureSampler& other) = default;

    SRX_INLINE void SetTexWrap(ETextureWrapping s, ETextureWrapping t);
    SRX_INLINE void SetTexWrap(ETextureWrapping w) { SetTexWrap(w, w); }

    /**
     * @brief Retrieves the texture wrapping modes for the S and T coordinates.
     *
     * @return A pair containing the wrapping modes for the S and T texture
     * coordinates.
     */
    SRX_INLINE TPair<ETextureWrapping, ETextureWrapping> GetWrap() const
    {
      return std::make_pair(mWrap[0], mWrap[1]);
    }

    SRX_INLINE ETextureFilter GetFilter(const EFilterType t) const
    {
      return mFIlters[static_cast<uint8>(t)];
    }
    SRX_INLINE void SetFilter(EFilterType t, ETextureFilter f);

    SRX_INLINE void        SetTexCoords(const Vec2& v) { mTexCoords = v; }
    SRX_INLINE const Vec2& GetTexCoords() const { return mTexCoords; }

    SRX_INLINE void  SetBorderColor(Color c) { mBorderColor = c; }
    SRX_INLINE Color GetBorderColor() const { return mBorderColor; }

private:
    TArray<ETextureWrapping, 2> mWrap;
    TArray<ETextureFilter, 3>   mFIlters;
    Vec2                        mTexCoords;
    Color                       mBorderColor;
  };

  SRX_INLINE void TextureSampler::SetTexWrap(ETextureWrapping s,
                                             ETextureWrapping t)
  {
    mWrap[0] = s;
    mWrap[1] = t;
  }

  SRX_INLINE void TextureSampler::SetFilter(EFilterType t, ETextureFilter f)
  {
    mFIlters[static_cast<uint8>(t)] = f;
  }

}  // namespace Sorex::Graphics