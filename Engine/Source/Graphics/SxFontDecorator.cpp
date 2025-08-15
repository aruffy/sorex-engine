#include <Sorex/Graphics/SxFontDecorator.h>

namespace Sorex::Graphics
{
  FontDecorator::FontDecorator()
    : mFont(nullptr)
  {
    UseDefaulSize();
  }

  FontDecorator::Attrib::Attrib()
    : enable(0)
    , floating(0)
    , format(0)
  {}

  float FontDecorator::GetScale() const SRX_NOEXCEPT
  {
    const Attrib& attrib = mAttribs[Attrib::Attrib_Desired_Size];
    if (attrib.floating)
      return attrib.value.fval;

    const uint16 fontSize = GetFontSize();
    if (!fontSize)
      return 1.f;

    return static_cast<float>(attrib.value.ival) / fontSize;
  }

  Color FontDecorator::GetColor() const SRX_NOEXCEPT
  {
    Color color = Color::White;
    if (const auto& attrib = mAttribs[Attrib::Attrib_Color]; attrib.enable)
      color.value = static_cast<uint32>(attrib.value.ival);

    return color;
  }

  int32 FontDecorator::GetLetterSpacing() const SRX_NOEXCEPT
  {
    const auto& attrib = mAttribs[Attrib::Attrib_Letter_Spacing];
    if (!attrib.enable)
      return 0;

    if (!attrib.floating)
      return attrib.value.ival;

    const uint16 fontSize = GetFontSize();
    if (!fontSize)
      return 0;

    const float emval = attrib.value.fval;
    return static_cast<int32>(roundf(emval * fontSize));
  }

  int32 FontDecorator::GetLineHeight() const SRX_NOEXCEPT
  {
    const auto& attrib = mAttribs[Attrib::Attrib_Line_Height];
    if (!attrib.enable)
    {
      const auto* metrics = mFont ? mFont->GetMetrics() : nullptr;
      return metrics ? (metrics->ascent - metrics->descent) : 0;
    }

    if (!attrib.floating)
      return attrib.value.ival;

    const uint16 fontSize = GetFontSize();
    if (!fontSize)
      return 0;

    return static_cast<int32>(roundf(attrib.value.fval * fontSize));
  }

  bool FontDecorator::CalcFontMetrics(FontMetrics& metrics) const SRX_NOEXCEPT
  {
    const FontMetrics* fontMetrics = mFont ? mFont->GetMetrics() : nullptr;
    if (fontMetrics == nullptr)
      return false;

    const float scale = GetScale();
    if (scale == 1.f)
    {
      metrics = *fontMetrics;
    }
    else
    {
      metrics.ascent  = static_cast<int16>(fontMetrics->ascent * scale);
      metrics.descent = static_cast<int16>(fontMetrics->descent * scale);
      metrics.top     = static_cast<int16>(fontMetrics->top * scale);
      metrics.bottom  = static_cast<int16>(fontMetrics->bottom * scale);
      metrics.leading = static_cast<int16>(fontMetrics->leading * scale);
    }

    if (const auto& attrib = mAttribs[Attrib::Attrib_Line_Height];
        attrib.enable)
      metrics.leading = static_cast<int16>(attrib.value.ival & 0xffff);

    return true;
  }

  FontOutline FontDecorator::GetOutline() const SRX_NOEXCEPT
  {
    FontOutline   outline;
    const Attrib& attrib = mAttribs[Attrib::Attrib_Outline];
    if (!attrib.enable)
      return outline;

    outline.color.value = attrib.value.ival;
    outline.thickness   = attrib.format;
    return outline;
  }
}