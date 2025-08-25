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
#include <Sorex/Graphics/SxFont.h>

namespace Sorex::Graphics
{
  using FontMetrics = FontData::Metrics;
  enum class EFontTransform
  {
    None,
    Upppercase,
    Lowercase
    // @todo: Capitalize
  };

  struct FontOutline
  {
    Color color     = Color::White;
    uint8 thickness = 0u;
  };

  // @TODO: text-shadow: 2px 2px 4px #000
  class FontDecorator final
  {
public:
    FontDecorator();

    void        Apply(const Font* font) { mFont = font; }
    const Font* GetFont() const { return mFont; }

    // Builder
    /**
     * @brief Set the Color of the font glyphs
     *
     * @param color - desired color
     * @return FontDecorator& - reference to self
     */
    SRX_INLINE FontDecorator& SetColor(const Color color);

    /**
     * @brief Set the desired font size in pixels;
     *
     * If the font size is set, the scale factor will be ignored
     *
     * @param size - desired font size in pixels
     * @return FontDecorator& - reference to self
     */
    FontDecorator& SetSize(const uint16 size);

    /**
     * @brief Use the default font size
     *
     * @return FontDecorator& - reference to self
     */
    SRX_INLINE FontDecorator& UseDefaulSize();

    /**
     * @brief Set the Font Scale factor
     *
     * If the font sacle factor is set, the desired font size will be ignored
     *
     * @param scale - font scale factor
     * @return FontDecorator& - reference to self
     */
    SRX_INLINE FontDecorator& SetScale(const float scale);

    /**
     * @brief Set the letter spacing in pixels.
     *
     * @param spacing - letter spacing in pixels
     * @return FontDecorator& - reference to self
     */
    SRX_INLINE FontDecorator& SetLetterSpacing(int32 spacing);

    /**
     * @brief Set the letter spacing in em units (0 - 1).
     *
     * @param spacing - letter spacing in em units
     * @return FontDecorator& - reference to self
     */
    SRX_INLINE FontDecorator& SetLetterSpacingUnit(const float em);

    /**
     * @brief Set the line height in pixels.
     *
     * @param height - line height in pixels
     * @return FontDecorator& - reference to self
     */
    SRX_INLINE FontDecorator& SetLineHeight(uint16 height);

    /**
     * @brief Set the line height multiplier, related from current font size.
     *
     * Value 1.5f => height = font_size * 1.5;
     *
     * @param height - line height in units
     * @return FontDecorator& - reference to self
     */
    SRX_INLINE FontDecorator& SetLineHeightFactor(const float factor);

    /**
     * @brief Set text trasformation
     *
     * @param transform - type of transformation
     * @return FontDecorator& - reference to self
     */
    SRX_INLINE FontDecorator& SetTextTransform(const EFontTransform transform);

    /**
     * @brief Set colored outline area around glyphs
     *
     * To disable use 0 values;
     *
     * @param thickness - desired outline thickness
     * @param color - outline color
     * @return FontDecorator& - rederence to self
     */
    SRX_INLINE FontDecorator& SetOutline(uint8 thickness,
                                         Color color = Color::White);

    // Getters
    SRX_INLINE const FontGlyph* GetGlyph(glyph_t) const;
    float                       GetScale() const SRX_NOEXCEPT;
    Color                       GetColor() const SRX_NOEXCEPT;
    int32                       GetLetterSpacing() const SRX_NOEXCEPT;
    int32                       GetLineHeight() const SRX_NOEXCEPT;
    SRX_INLINE EFontTransform   GetTextTransform() const;
    FontOutline                 GetOutline() const SRX_NOEXCEPT;

    bool CalcFontMetrics(FontMetrics& metrics) const SRX_NOEXCEPT;

private:
    union attrib_t
    {
      int   ival;
      float fval;
    };

    struct Attrib
    {
      Attrib();

      // @note: used as array index
      enum EType : uint8
      {
        Attrib_Desired_Size = 0,
        Attrib_Color,
        Attrib_Line_Height,
        Attrib_Letter_Spacing,
        Attrib_Transform,
        Attrib_Outline,

        Attrib_Number  // @note: must be last
      };

      union value_type
      {
        int32 ival;
        float fval;
      };

      value_type value;
      uint8      enable : 1;
      uint8      floating : 1;
      uint8      format : 6;
    };


private:
    SRX_INLINE uint16 GetFontSize() const
    {
      return mFont ? mFont->GetSize() : 0u;
    }

    SRX_INLINE void DisableAttrib(const Attrib::EType attrib)
    {
      mAttribs[attrib].enable = 0;
    }

    template<typename T,
             typename Enable = SRX_TYPENAME std::enable_if_t<
               (std::is_floating_point_v<T> || std::is_integral_v<T>)>>
    SRX_INLINE void SetAttrib(const Attrib::EType attrib,
                              T                   value,
                              uint8               format = 0);

private:
    const Font*                           mFont;
    TArray<Attrib, Attrib::Attrib_Number> mAttribs;
  };


  template<typename T, typename Enable>
  SRX_INLINE void FontDecorator::SetAttrib(const Attrib::EType type,
                                           T                   value,
                                           uint8               format)
  {
    Attrib& attrib = mAttribs[type];
    if constexpr (std::is_floating_point_v<T>)
    {
      attrib.value.fval = static_cast<float>(value);
      attrib.floating   = 1;
    }
    else
    {
      attrib.value.ival = static_cast<int>(value);
      attrib.floating   = 0;
    }

    attrib.enable = 1;
    attrib.format = format;
  }

  SRX_INLINE FontDecorator& FontDecorator::SetSize(const uint16 size)
  {
    SetAttrib(Attrib::Attrib_Desired_Size, size);
    return *this;
  }

  SRX_INLINE FontDecorator& FontDecorator::SetScale(const float scale)
  {
    SetAttrib(Attrib::Attrib_Desired_Size, scale);
    return *this;
  }

  SRX_INLINE FontDecorator& FontDecorator::SetColor(const Color color)
  {
    SetAttrib(Attrib::Attrib_Color, color.value);
    return *this;
  }

  SRX_INLINE FontDecorator& FontDecorator::UseDefaulSize()
  {
    SetScale(1.f);
    return *this;
  }

  SRX_INLINE FontDecorator& FontDecorator::SetLetterSpacing(int32 spacing)
  {
    if (spacing == 0)
      DisableAttrib(Attrib::Attrib_Letter_Spacing);
    else
      SetAttrib(Attrib::Attrib_Letter_Spacing, spacing);
    return *this;
  }

  SRX_INLINE FontDecorator& FontDecorator::SetLetterSpacingUnit(const float em)
  {
    const float value = Math::Clamp(em, -1.f, 1.f);
    if (value == 0.f)
      DisableAttrib(Attrib::Attrib_Letter_Spacing);
    else
      SetAttrib(Attrib::Attrib_Letter_Spacing, value);
    return *this;
  }

  SRX_INLINE FontDecorator& FontDecorator::SetLineHeight(uint16 height)
  {
    SetAttrib(Attrib::Attrib_Line_Height, height);
    return *this;
  }

  SRX_INLINE FontDecorator& FontDecorator::SetLineHeightFactor(
    const float factor)
  {
    SetAttrib(Attrib::Attrib_Line_Height, factor);
    return *this;
  }

  SRX_INLINE const FontGlyph* FontDecorator::GetGlyph(glyph_t codepoint) const
  {
    SRX_CHECK(mFont);
    return mFont->GetGlyph(codepoint);
  }

  SRX_INLINE FontDecorator& FontDecorator::SetTextTransform(
    const EFontTransform transform)
  {
    if (transform == EFontTransform::None)
      DisableAttrib(Attrib::Attrib_Transform);
    else
      SetAttrib(Attrib::Attrib_Transform, static_cast<int32>(transform));

    return *this;
  }

  SRX_INLINE EFontTransform FontDecorator::GetTextTransform() const
  {
    const Attrib& attrib = mAttribs[Attrib::Attrib_Transform];
    return attrib.enable ? static_cast<EFontTransform>(attrib.value.ival)
                         : EFontTransform::None;
  }

  SRX_INLINE FontDecorator& FontDecorator::SetOutline(
    uint8 thickness,
    Color color /* = Color::White */)
  {
    if (thickness == 0)
      DisableAttrib(Attrib::Attrib_Outline);
    else
      SetAttrib(Attrib::Attrib_Outline,
                color.value,
                std::min<uint8>(thickness, 0x3f));

    return *this;
  }
}  // namespace Ruffy::Graphics