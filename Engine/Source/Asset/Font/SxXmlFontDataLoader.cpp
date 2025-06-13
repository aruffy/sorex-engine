
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

#include "SxXmlFontDataLoader.h"

#include <Sorex/Utils/SxString.h>

#include <Utils/XmlReader.h>

namespace
{
  // FIXME: Remove CompareNoCase
  // Use third-party library for XML parsing

  using namespace Sorex;
  bool ParseXmlFontMetrics(const TVector<Utils::XmlAttrib>& attribs,
                           Graphics::FontData::Metrics&     metrics)
  {
    for (const Utils::XmlAttrib& attrib : attribs)
    {
      if (Utils::CompareNoCase(attrib.name, "ascent"))
        Utils::ToInteger(attrib.value, metrics.ascent);

      else if (Utils::CompareNoCase(attrib.name, "descent"))
        Utils::ToInteger(attrib.value, metrics.descent);

      else if (Utils::CompareNoCase(attrib.name, "top"))
        Utils::ToInteger(attrib.value, metrics.top);

      else if (Utils::CompareNoCase(attrib.name, "bottom"))
        Utils::ToInteger(attrib.value, metrics.bottom);

      else if (Utils::CompareNoCase(attrib.name, "leading"))
        Utils::ToInteger(attrib.value, metrics.leading);
    }

    if (metrics.top <= 0)
      return false;

    // @TODO: warnings
    if (metrics.ascent <= 0)
      metrics.ascent = metrics.top;

    if (metrics.leading <= 0)
      metrics.leading = metrics.top;

    return true;
  }
  void ParseXmlFontGlyphs(const TVector<Utils::XmlAttrib>& attribs,
                          Graphics::FontGlyph&             glyph)
  {
    for (const Utils::XmlAttrib& attrib : attribs)
    {
      if (Utils::CompareNoCase(attrib.name, "code"))
        glyph.code = attrib.value.empty() ? 0 : attrib.value[0];

      else if (Utils::CompareNoCase(attrib.name, "width"))
        Utils::ToInteger(attrib.value, glyph.rect.width);

      else if (Utils::CompareNoCase(attrib.name, "height"))
        Utils::ToInteger(attrib.value, glyph.rect.height);

      else if (Utils::CompareNoCase(attrib.name, "x"))
        Utils::ToInteger(attrib.value, glyph.rect.x);

      else if (Utils::CompareNoCase(attrib.name, "y"))
        Utils::ToInteger(attrib.value, glyph.rect.y);

      else if (Utils::CompareNoCase(attrib.name, "bx"))
        Utils::ToInteger(attrib.value, glyph.bearing.x);

      else if (Utils::CompareNoCase(attrib.name, "by"))
        Utils::ToInteger(attrib.value, glyph.bearing.y);

      else if (Utils::CompareNoCase(attrib.name, "advance"))
        Utils::ToInteger(attrib.value, glyph.advance);

      if (!glyph.advance)
        glyph.advance = glyph.rect.width;
    }
  }
}

namespace Sorex::Resource
{
  TPair<TUniquePointer<Graphics::FontData>,
        TUniquePointer<Graphics::TextureBitmap>>
  XMLFontDataLoader::LoadFont(Stream& stream, Status* status)
  {
    Utils::XmlReader xml(stream);

    while (xml.Read())
    {
      if (xml.nodeType == Utils::XMLNODE_Element)
        break;
    }

    if (xml.HasFailed())
    {
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Bad_File,
                         "reading xml descriptor failed");
      return std::make_pair(nullptr, nullptr);
    }

    if (xml.EndOfFile() || !Utils::CompareNoCase(xml.name, "font"))
    {
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Invalid_Format,
                         "invalid root element 'font' expected");
      return std::make_pair(nullptr, nullptr);
    }

    TUniquePointer<Graphics::FontData> font = MakeUnique<Graphics::FontData>();

    // @todo: filter charsets
    font->type = Graphics::EFontType::Bitmap;
    for (size_t i = 0; i < xml.attributes.size(); ++i)
    {
      Utils::XmlAttrib& attrib = xml.attributes[i];
      if (Utils::CompareNoCase(attrib.name, "family"))
        font->family = std::move(attrib.value);

      else if (Utils::CompareNoCase(attrib.name, "style"))
        font->style = std::move(attrib.value);

      else if (Utils::CompareNoCase(attrib.name, "size"))
        Utils::ToInteger(attrib.value, font->size);

      else if (Utils::CompareNoCase(attrib.name, "type"))
        if (Utils::CompareNoCase(attrib.value, "sdf"))
          font->type = Graphics::EFontType::Signed_Distance_Field;
    }

    if (font->family.empty() || !font->size)
    {
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Invalid_Format,
                         "invalid font xml node format");
      return std::make_pair(nullptr, nullptr);
    }

    if (font->style.empty())
      font->style = "Regular";

    while (xml.Read())
    {
      if (xml.nodeType != Utils::XMLNODE_Element)
        continue;

      if (xml.name == "Metrics")
      {
        if (!ParseXmlFontMetrics(xml.attributes, font->metrics))
        {
          SRX_STATUS_PTR_MSG(status,
                             EStatusCode::Invalid_Format,
                             "invalid font metrics");
          return std::make_pair(nullptr, nullptr);
        }
      }
      else if (xml.name == "Glyphs")
      {
        while (xml.Read())
        {
          if (xml.nodeType == Utils::XMLNODE_EndElement && xml.name == "Glyphs")
            break;

          if (xml.nodeType == Utils::XMLNODE_Element)
          {
            SRX_CHECK(xml.name == "Char");

            // @todo: move or placement
            Graphics::FontGlyph glyph;
            ParseXmlFontGlyphs(xml.attributes, glyph);

            SRX_CHECK(glyph.code && glyph.rect.GetSize().IsValid());
            font->glyphs.emplace(glyph.code, glyph);
          }
        }
      }
    }

    return std::make_pair(std::move(font), nullptr);
  }
}